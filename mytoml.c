//-------------------------------------------------------------------------
// [SECTION] INCLUDES
//-------------------------------------------------------------------------

#include "mytoml.h"
#include <time.h>    //
#include <math.h>    //
#include <stdio.h>   // for printf
#include <stdarg.h>  //
#include <stdlib.h>  // for realloc
#include <string.h>  // for strdup strlen
#include <stdbool.h> //

#pragma region Internal

//-----------------------------------------------------------------------------
// [SECTION] Defines
//-----------------------------------------------------------------------------

/**
 * @def LOG_ERR
 * @brief Macro to log error message to stderr.
 * @note It also specifies which file, line and function the error was raised in.
 */
#define LOG_ERR(...)                           \
    do                                         \
    {                                          \
        fprintf(stderr, "%s:%d [%s]: ",        \
                __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__);          \
    } while (0)

/**
 * @def RETURN_IF_FAILED
 * @brief Macro to check `COND` and return if it fails.
 * @param COND expression to check.
 * @note returns NULL from the location where it is called.
 */
#define RETURN_IF_FAILED(COND, ...) \
    do                              \
    {                               \
        if (!(COND))                \
        {                           \
            LOG_ERR(__VA_ARGS__);   \
            return NULL;            \
        }                           \
    } while (0)

/**
 * @def FUNC_IF_FAILED
 * @brief Macro to check `COND` and calls `FUNC` with args if it fails.
 * @param COND expression to check.
 * @param FUNC fuction to call.
 * @note returns NULL from the location where it is called.
 */
#define FUNC_IF_FAILED(COND, FUNC, ...) \
    do                                  \
    {                                   \
        if (!(COND))                    \
        {                               \
            FUNC(__VA_ARGS__);          \
        }                               \
    } while (0)

/**
 * @def CHECK_DATETIME
 * @brief Macro to check date and time.
 * @param VAR variable to check.
 * @param LEN expected variable lenght.
 * @note returns NULL from the location where it is called.
 */
#define CHECK_DATETIME(VAR, LEN, ...)                      \
    do                                                     \
    {                                                      \
        RETURN_IF_FAILED(strlen(VAR) == LEN, __VA_ARGS__); \
        num = strtoul(VAR, &end, 10);                      \
        RETURN_IF_FAILED(end == VAR + LEN, __VA_ARGS__);   \
    } while (0)

/**
 * @def CHECK_DATE
 * @brief Macro to check date.
 * @note This macro is used in check/validate year, month and day.
 */
#define CHECK_DATE()                               \
    do                                             \
    {                                              \
        CHECK_DATETIME(year, 4, "invalid year\n"); \
        time->tm_year = num - 1900;                \
        CHECK_DATETIME(mon, 2, "invalid month\n"); \
        time->tm_mon = num - 1;                    \
        CHECK_DATETIME(mday, 2, "invalid day\n");  \
        time->tm_mday = num;                       \
    } while (0)

/**
 * @def CHECK_TIME
 * @brief Macro to check time.
 * @note This macro is used in check/validate hour, minute and seconds.
 */
#define CHECK_TIME()                                \
    do                                              \
    {                                               \
        CHECK_DATETIME(hour, 2, "invalid hour\n");  \
        time->tm_hour = num;                        \
        CHECK_DATETIME(min, 2, "invalid minute\n"); \
        time->tm_min = num;                         \
        CHECK_DATETIME(sec, 2, "invalid second\n"); \
        time->tm_sec = num;                         \
    } while (0)

//-----------------------------------------------------------------------------
// [SECTION] Data Structures
//-----------------------------------------------------------------------------

/**
 * @defgroup Parser Basis Types
 * @brief Core types and data structures for Parser.
 * @{
 */

/*
    Struct `tokenizer` handles the input stream
    by reading and returning tokens for the parser.
    However, this currently returns each character
    as a token. It allows each state of the parser
    to determine if it is an acceptable token, and
    stop parsing as soon as it is not.

    TODO: Refactor to parse tokens instead of characters
*/

/**
 * @enum InputType
 * @brief Enumerates all TOML input types supported by the parser.
 * @details Used to distinguish between FILE* , char* file and char* string.
 */
typedef enum InputType
{

    I_FILE,  /**< `FILE *` File input type  */
    I_File,  /**< `basic.toml` File input type */
    I_STREAM /**< `char *` Stream input type  */

} InputType;

/**
 * @name Parser Input type
 * @{
 */

/**
 * @struct Input
 * @brief Represents a TOML input for parser.
 */
typedef struct Input
{
    InputType type; /**< The file input type. */
                    /** Standard ( `FILE*` or `char *` file) input. */
    union
    {
        const char *name; /**< The `char*` file input filename. */
        FILE *pointer;    /**< The `FILE*` file input pointer. */
    } file;

    char *stream; /**< Pointer for storing the input buffer */
} Input;

/** @} */

/**
 * @name Parser Input type
 * @{
 */

/**
 * @struct Tokenizer
 * @brief Represents a TOML parser.
 */
typedef struct Tokenizer
{
    Input input;
    int cursor;                      /**< The location in the input buffer */
    char token;                      /**< The last read in token */
    char prev;                       /**< The token read in before `token` */
    char prev_prev;                  /**< The token read in before `prev` */
    bool is_null;                    /**< Boolean to indicate if `token` is non-NULL */
    bool newline;                    /**< To keep track if we are on a newline */
    int line;                        /**< The current line number in the stream */
    int col;                         /**< The current column number in the stream */
    int lines[MYTOML_MAX_NUM_LINES]; /**, The array where index=line and lines[index]=length */
} Tokenizer;

/** @} */

/**
 * @name Number data type
 * @{
 */

/**
 * @struct Number
 * @brief Represent parsed number values.
 * It also stores precision and scientific notation flag for floats,
 * for testing
 * @note Used for TOML_INT and TOML_FLOAT value types.
 * @note number can be either integer or floating-point.
 */
typedef struct Number
{
    TomlValueType type; /**< */
    int precision;      /**<  */
    bool scientific;    /**<  */
} Number;

/** @} */

/**
 * @name Datetime data type
 * @{
 */

/**
 * @struct Datetime
 * @brief Represent a generic type for a parsed datetime values.
 * number can be either integer or floating-point.
 * It also stores the matching format, again for compliance testing.
 * @note Used for DATETIME, DATELOCAL, TIMELOCAL and DATETIMELOCAL value types.
 */
typedef struct Datetime
{
    struct tm *dt;                       /**  */
    TomlValueType type;                  /**  */
    char format[MYTOML_MAX_DATE_FORMAT]; /**  */
    int millis;                          /**  */
} Datetime;

/** @} */

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] C Only Functions
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    //-----------------------------------------------------------------------------
    // [SECTION] Declarations
    //-----------------------------------------------------------------------------

    // Helper function to append formatted text to a string buffer
    void _Mytoml_AppendToBuffer(char **buffer, size_t *size, const char *format, ...);

    static inline void _Mytoml_StringDump(const char *s, char **buffer, size_t *size);

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Tokenizer
    //-----------------------------------------------------------------------------

    /*
        Function `_Mytoml_NewTokenizer` allocates memory for the
        tokenizer, assigns some default initial values to
        the various attributes and returns a pointer to the
        newly allocated memory region.
    */
    /**
     * @ Create and initialize a tokenizer.
     *
     * @param[in,out]  input  An `Input` object.
     * @returns a pointer to a `tokenizer` object.
     */
    Tokenizer *_Mytoml_NewTokenizer(Input input);

    /*
        Function `_Mytoml_TokenizerLoadInput` loads the data from an the input
        stream onto a char buffer. It also checks to make sure
        that the input is not too large. Upon any error, it
        returns false and returns true if everything succeeds.
    */
    bool _Mytoml_TokenizerLoadInput(Tokenizer *tok);

    /*
        Function `_Mytoml_NextTokenizerToken` reads the next character from the
        input stream. It then stores it in the `token` attribute.
        If we have reached the EOF, the `_Mytoml_TokenizerHasToken` attribute is
        set to false. This also updates all the attributes of the
        tokenizer appropriately (except `stream`). If we read in
        a token, i.e. we have not reached EOF, returns 1, else
        returns 0.
    */
    int _Mytoml_NextTokenizerToken(Tokenizer *tok);

    /*
        Function `_Mytoml_TokenizerBacktrace` is used to move the `cursor` back in
        the input stream. This allows look-ahead operations to make
        parsing decisions. For example, let's say we want to decide
        whether to parse something as a date or a number. To know if
        there is a `-` a few chars ahead, we call `_Mytoml_NextTokenizerToken`.
        However, in the case that there isn't, we need to go back
        to where we were and re-parse it as a number. This function
        takes an argument `count` and backtracks that many characters.
        To maintain the values of `prev` and `prev_prev`, this goes
        back 2 extra characters and calls `_Mytoml_NextTokenizerToken` twice to
        re-populate them.
    */
    void _Mytoml_TokenizerBacktrace(Tokenizer *tok, int count);

    /*
        Function `_Mytoml_TokenizerHasToken` returns true if the boolean attribute
        is set to true. This should be used callers to query if
        the tokenizer has a non-EOF token waiting to be parsed.
    */
    bool _Mytoml_TokenizerHasToken(Tokenizer *tok);

    /*
        Functions `_Mytoml_TokenizerGetToken`, `_Mytoml_TokenizerGetPrev` and `_Mytoml_TokenizerGetPrevPrev`
        returns the `token`, `prev` and `prev_prev` attributes
        held by the tokenizer, respectively. This should be
        used by callers to access the tokens read in by the
        tokenizer.
    */
    char _Mytoml_TokenizerGetToken(Tokenizer *tok);

    char _Mytoml_TokenizerGetPrev(Tokenizer *tok);

    char _Mytoml_TokenizerGetPrevPrev(Tokenizer *tok);

    /**
     * @brief Free any memory allocated for a `tokenizer` object.
     *
     * @param[in,out]   tok A tokenizer object.
     */
    void _Mytoml_FreeTokenizer(Tokenizer *tok);

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Value
    //-----------------------------------------------------------------------------

    /*
        Function `_Mytoml_FreeValue` frees up all the memory
        that is associated with value `v`.
    */
    void _Mytoml_FreeValue(TomlValue *v);

    /*
        Function `_Mytoml_NewArrayValue` allocates a buffer for an
        array of `TomlValue` and returns a pointer to it.
        This is stored in the `arr` attribute.
    */
    TomlValue *_Mytoml_NewArrayValue();

    /*
        Function `_Mytoml_NewTableValue` takes a key `k` as
        it's argument which can contain one or many key
        value pairs, including subkeys. It then allocates
        some new memory for a key and adds all subkeys of
        `k` as subkeys of this newly allocated key. It
        returns a pointer to the newly allocated value.
        The `data` attribute contains the newly created key.
    */
    TomlValue *_Mytoml_NewTableValue(TomlKey *k);

    /*
        Functions `_Mytoml_NewStringValue`, `_Mytoml_NewDatetimeValue` and `_Mytoml_NewNumberValue`
        allocates some memory for each of these datatypes
        respectively. It then copies over the passed in data
        into the `data` attribute of the value. Finally, like
        the other functions, it returns a pointer to the newly
        allocated value.
    */
    TomlValue *_Mytoml_NewStringValue(const char *s);

    TomlValue *_Mytoml_NewDatetimeValue(struct tm *dt, TomlValueType type, char *format, int millis);

    TomlValue *_Mytoml_NewNumberValue(double *d, TomlValueType type, size_t precision, bool scientific);

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Key
    //-----------------------------------------------------------------------------

    /*
        Function `_Mytoml_NewKey` allocates memory to create
        a new key/node in the AST. It takes the key type
        as an argument and initializes everything else
        to NULL and idx to 0. Returns a pointer to the
        newly allocated key.
    */
    TomlKey *_Mytoml_NewKey(TomlKeyType type);

    /*
        Function `_Mytoml_DeleteKey` frees up all the memory allocated
        by this key. It first recursively frees up all the
        memory allocated by the keys in `children` if any. Then
        it frees up all the memory allocated by `value` if any.
        Finally, it frees up the memory allocated by itself.
    */
    void _Mytoml_DeleteKey(TomlKey *key);

    /*
        Function `_Mytoml_HasSubKey` checks if a `key` has a `subkey`
        in its list of `children` by iterating through it
        and matching against the `id`. Returns a pointer
        to the key if it exists, else returns NULL.
    */
    TomlKey *_Mytoml_HasSubKey(TomlKey *key, TomlKey *subkey);

    /*
        Function `_Mytoml_AddSubKey` tries to add `subkey` in the
        list of `children` of `key`. There are checks to do
        memory initialization as well. If a `subkey` is already
        there in `children`, then the `_Mytoml_CompatibleKeys` function
        is used to check if the new key is "compatible" with
        the old "key". If yes, or if the `subkey` did not exist
        in the first place, a pointer to the existing or newly
        added subkey is returned respectively. Otherwise, it
        returns a NULL pointer on failure or buffer overflow.
    */
    TomlKey *_Mytoml_AddSubKey(TomlKey *key, TomlKey *subkey);

    /*
        Function `_Mytoml_CompatibleKeys` is used to decide if the
        re-definition of a key is acceptable by TOML specs.
        It uses the `TomlKeyType_t` of the existing key and the
        key being re-defined and returns `true` if it is valid
        and `false` otherwise.
    */
    bool _Mytoml_CompatibleKeys(TomlKeyType existing, TomlKeyType current);

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Parser Utils
    //-----------------------------------------------------------------------------

    /*
        All of these are utility functions to
        check character criteria. Most of these
        are defined according to TOML spec.
        Some of these check the same thing, but
        they are defined multitple times for code
        readability.
    */
    bool _Mytoml_IsDot(char c);
    bool _Mytoml_IsEqual(char c);
    bool _Mytoml_IsDigit(char c);
    bool _Mytoml_IsEscape(char c);
    bool _Mytoml_IsReturn(char c);
    bool _Mytoml_Iscontrol(char c);
    bool _Mytoml_IsNewline(char c);
    bool _Mytoml_IsArrayEnd(char c);
    bool _Mytoml_IsArraySep(char c);
    bool _Mytoml_IsTableEnd(char c);
    bool _Mytoml_IsHexDigit(char c);
    bool _Mytoml_IsUnderscore(char c);
    bool _Mytoml_IsArrayStart(char c);
    bool _Mytoml_IsWhitespace(char c);
    bool _Mytoml_IsTableStart(char c);
    bool _Mytoml_IsBareAscii(char c);
    bool _Mytoml_IsNumberStart(char c);
    bool _Mytoml_IsCommentStart(char c);
    bool _Mytoml_IsDecimalPoint(char c);
    bool _Mytoml_IsControlMulti(char c);
    bool _Mytoml_IsInlineTableEnd(char c);
    bool _Mytoml_IsInlineTableSep(char c);
    bool _Mytoml_IsControlLiteral(char c);
    bool _Mytoml_IsBasicStringStart(char c);
    bool _Mytoml_IsInlineTableStart(char c);
    bool _Mytoml_IsLiteralStringStart(char c);
    bool _Mytoml_IsDate(int year, int month, int day);
    bool _Mytoml_IsNumberEnd(char c, const char *end);
    bool _Mytoml_IsValidDatetime(struct tm *datetime);

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Parser Key
    //-----------------------------------------------------------------------------

    /*
        Functions `_Mytoml_ParserBareKey`, `_Mytoml_ParserBAsicQuotedKey`
        and `_Mytoml_ParserLiteralQuotedKey` parses a "bare key",
        a "basic quoted key" and a "literal quoted key"
        respectively, as defined by the TOML spec. The caller
        should decide which one is being parsed. They use `.`
        and `=` as delimiters. Once it has successfully parsed
        a key, it creates a key and returns a pointer to it.
        All of them log errors and return NULL on parsing
        failure. The key types `branch` and `leaf` passed as
        arguments determine the key types of keys created upon
        encountering a `.` and a `=` respectively. The `end`
        argument determines which character marks the termination
        of parsing.
    */
    TomlKey *_Mytoml_ParserBareKey(Tokenizer *tok, char end, TomlKeyType branch, TomlKeyType leaf);

    TomlKey *_Mytoml_ParserBAsicQuotedKey(Tokenizer *tok, char end, TomlKeyType branch, TomlKeyType leaf);

    TomlKey *_Mytoml_ParserLiteralQuotedKey(Tokenizer *tok, char end, TomlKeyType branch, TomlKeyType leaf);

    /*
        Functions `_Mytoml_ParseKey`, `_Mytoml_ParseTable` and
        `_Mytoml_ParseArrayTable` tries to parse a TOML
        key, table and arraytable respectively. It
        takes a `key` as its argument meaning the
        new key(s) that is(are) parsed will be added
        to the `subkeys` of `key`. Setting `expecting`
        to `true` will throw an error if we encounter
        a delimiter like `.` or `=`. Returns NULL on
        parsing failure.
    */
    TomlKey *_Mytoml_ParseKey(Tokenizer *tok, TomlKey *key, bool expecting);

    TomlKey *_Mytoml_ParseTable(Tokenizer *tok, TomlKey *key, bool expecting);

    TomlKey *_Mytoml_ParseArrayTable(Tokenizer *tok, TomlKey *key, bool expecting);

    /*
        Function `_Mytoml_ParseKeyValue` tries to parse a key
        and a value as defined by TOML. This is the entry
        point to parsing the TOML file. It takes a `key`
        and adds all the newly parsed keys as its `subkeys`.
        Since we end up creating a tree structure with the
        keys, we use a `root` key to refer to the whole
        tree. All keys are descendents of this key and this
        key refers to the entire TOML file. This function
        returns NULL on failure to parse. It returns a
        pointer to the last table or arraytable it parsed
        and returns `key` if neither.
    */
    TomlKey *_Mytoml_ParseKeyValue(Tokenizer *tok, TomlKey *key, TomlKey *root);

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Parser Value
    //-----------------------------------------------------------------------------

    /*
        Functions `parse_<TYPE>` parses a TOML value of type
        TYPE. They take the tokenizer and parses one character
        at a time. Numerical values and datetimes have a list
        of characters to mark the end of parsing. Strings take
        in pre-allocated buffers. Arrays repeatedly parse values.
        Inline tables repeatedly parse key-value pairs. Everything
        returns a pointer to what it parsed and NULL on parsing
        failure. `_Mytoml_ParseComment` returns true if a valid comment
        was parsed and `_Mytoml_ParseNewline` returns true if a newline
        was successfully parsed.
    */
    bool _Mytoml_ParseComment(Tokenizer *tok);

    void _Mytoml_ParseWhitespace(Tokenizer *tok);

    bool _Mytoml_ParseNewline(Tokenizer *tok);

    double _Mytoml_ParseBoolean(Tokenizer *tok);

    TomlKey *_Mytoml_ParseInlineTable(Tokenizer *tok);

    int _Mytoml_ParseEscape(Tokenizer *tok, char *escaped, int len);

    int _Mytoml_ParseUnicode(Tokenizer *tok, char *escaped, int len);

    char *_Mytoml_ParseBasicString(Tokenizer *tok, char *value, bool multi);

    char *_Mytoml_ParseLiteralString(Tokenizer *tok, char *value, bool multi);

    double _Mytoml_ParseInfNan(Tokenizer *tok, bool negative);

    double _Mytoml_ParseBaseUnit(Tokenizer *tok, int base, char *value, const char *num_end);

    Number *_Mytoml_ParseNumber(Tokenizer *tok, char *value, double *d, const char *num_end, Number *n);

    Datetime *_Mytoml_ParseDatetime(Tokenizer *tok, char *value, const char *num_end, struct tm *time);

    TomlValue *_Mytoml_ParseArray(Tokenizer *tok, TomlValue *arr);

    /*
        Function `_Mytoml_ParseValue` looks at a character
        and decides what `TYPE` it is. Depending on that,
        it calls the appropriate `parse_<TYPE>` function.
        Since the `num_end` character set changes based
        on parsing context (for example, a number can end
        when we see a `,` when parsing an array), this
        is added as an argument to this function. This
        allows it to be used anywhere a value needs to be
        parsed.
    */
    TomlValue *_Mytoml_ParseValue(Tokenizer *tok, const char *num_end);

    //-----------------------------------------------------------------------------
    // [SECTION] Definations
    //-----------------------------------------------------------------------------

    void _Mytoml_AppendToBuffer(char **buffer, size_t *size, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        size_t needed = vsnprintf(NULL, 0, format, args) + 1;
        va_end(args);

        *buffer = (char *)realloc(*buffer, *size + needed);
        va_start(args, format);
        vsnprintf(*buffer + *size, needed, format, args);
        va_end(args);

        *size += needed - 1;
    }

    static inline void _Mytoml_StringDump(const char *s, char **buffer, size_t *size)
    {
        for (const char *c = s; *c != '\0'; c++)
        {
            switch (*c)
            {
            case '\b':
                _Mytoml_AppendToBuffer(buffer, size, "\\b");
                continue;
            case '\n':
                _Mytoml_AppendToBuffer(buffer, size, "\\n");
                continue;
            case '\r':
                _Mytoml_AppendToBuffer(buffer, size, "\\r");
                continue;
            case '\t':
                _Mytoml_AppendToBuffer(buffer, size, "\\t");
                continue;
            case '\f':
                _Mytoml_AppendToBuffer(buffer, size, "\\f");
                continue;
            case '\\':
                _Mytoml_AppendToBuffer(buffer, size, "\\\\");
                continue;
            case '"':
                _Mytoml_AppendToBuffer(buffer, size, "\\\"");
                continue;
            default:
                break;
            }
            _Mytoml_AppendToBuffer(buffer, size, "%c", *c);
        }
    }

    //-----------------------------------------------------------------------------
    // [SECTION] Tokenizer
    //-----------------------------------------------------------------------------

    Tokenizer *_Mytoml_NewTokenizer(Input input)
    {
        Tokenizer *tok = (Tokenizer *)calloc(1, sizeof(Tokenizer));
        tok->input = input;
        tok->cursor = 0;
        tok->token = '\0';
        tok->prev = '\0';
        tok->prev_prev = '\0';
        tok->line = 0;
        tok->col = 0;
        tok->is_null = true;
        memset(tok->lines, 0, MYTOML_MAX_NUM_LINES);
        return tok;
    }

    int _Mytoml_NextTokenizerToken(Tokenizer *tok)
    {
        tok->prev_prev = tok->prev;
        tok->prev = tok->token;
        if (tok->is_null || tok->cursor == 0)
        {
            tok->token = tok->input.stream[tok->cursor++];
            // if we parsed some non-whitespace character since we saw
            // the newline, then we aren't on a newline anymore
            if (tok->newline && tok->prev && tok->prev != ' ' &&
                tok->prev != '\t' && tok->prev != '\n')
            {
                tok->newline = false;
            }
            if (tok->token == '\n')
            {
                tok->newline = true;
            }
            if (tok->prev == '\n')
            {
                if (tok->line < MYTOML_MAX_NUM_LINES)
                {
                    tok->lines[tok->line] = tok->col;
                }
                tok->line++;
                tok->col = 0;
            }
            else
            {
                tok->col++;
            }
            if (tok->token == EOF)
            {
                tok->token = '\0';
                tok->is_null = false;
            }
            return 1;
        }
        return 0;
    }

    void _Mytoml_TokenizerBacktrace(Tokenizer *tok, int count)
    {
        int pre_count = count + 2;
        if (count > 0 && tok->cursor > pre_count)
        {
            tok->cursor -= pre_count;
            tok->is_null = true;
            int col = tok->col;
            while (tok->line >= 0 && pre_count > col)
            {
                pre_count -= col;
                col = tok->lines[--tok->line];
            }
            tok->col = col - pre_count;
            if (tok->line < 0)
                tok->line = 0;
            if (tok->col < 0)
                tok->col = 0;
            _Mytoml_NextTokenizerToken(tok);
            _Mytoml_NextTokenizerToken(tok);
        }
        else
        {
            LOG_ERR("not enough characters to _Mytoml_TokenizerBacktrace %d\n", count);
        }
    }

    bool _Mytoml_TokenizerLoadInput(Tokenizer *tok)
    {
        FILE *stream;
        if (tok->input.type == I_STREAM)
        {
            return true;
        }
        else if (tok->input.type == I_FILE)
        {
            stream = tok->input.file.pointer;
        }
        else if (tok->input.type == I_File)
        {
            stream = fopen(tok->input.file.name, "r");
        }
        else
        {
            stream = stdin;
        }

        fseek(stream, 0L, SEEK_END);
        long size = ftell(stream);
        fseek(stream, 0L, SEEK_SET);

        if (size >= MYTOML_MAX_FILE_SIZE)
        {
            LOG_ERR("input size is too big\n");
            return false;
        }

        char *buffer = (char *)calloc(1, size + 1);
        if (size > 0 && 1 != fread(buffer, size, 1, stream))
        {
            LOG_ERR("could not read input\n");
            return false;
        }
        buffer[size] = EOF;
        tok->input.stream = buffer;
#undef MYTOML_MAX_FILE_SIZE
        return true;
    }

    bool _Mytoml_TokenizerHasToken(Tokenizer *tok)
    {
        return tok->is_null;
    }

    char _Mytoml_TokenizerGetToken(Tokenizer *tok)
    {
        return tok->token;
    }

    char _Mytoml_TokenizerGetPrev(Tokenizer *tok)
    {
        return tok->prev;
    }

    char _Mytoml_TokenizerGetPrevPrev(Tokenizer *tok)
    {
        return tok->prev_prev;
    }

    void _Mytoml_FreeTokenizer(Tokenizer *tok)
    {
        free(tok->input.stream);
        free(tok);
    }

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Value
    //-----------------------------------------------------------------------------

    TomlValue *_Mytoml_NewStringValue(const char *s)
    {
        TomlValue *v = (TomlValue *)calloc(1, sizeof(TomlValue));
        v->type = TOML_STRING;
        v->data = calloc(1, strlen(s) + 1);
        memcpy(v->data, s, strlen(s));
        return v;
    }

    TomlValue *_Mytoml_NewNumberValue(double *d, TomlValueType type, size_t precision, bool scientific)
    {
        TomlValue *v = (TomlValue *)calloc(1, sizeof(TomlValue));
        v->type = type;
        v->scientific = scientific;
        v->precision = precision;
        v->data = calloc(1, sizeof(double));
        memcpy(v->data, d, sizeof(double));
        return v;
    }

    TomlValue *
    _Mytoml_NewDatetimeValue(struct tm *dt, TomlValueType type, char *format, int millis)
    {
        TomlValue *v = (TomlValue *)calloc(1, sizeof(TomlValue));
        v->type = type;
        v->precision = millis;
        v->data = calloc(1, sizeof(struct tm));
        memset(v->format, 0, MYTOML_MAX_DATE_FORMAT);
        if (strlen(format) < MYTOML_MAX_DATE_FORMAT)
        {
            memcpy(v->format, format, strlen(format));
        }
        memcpy(v->data, dt, sizeof(struct tm));
        return v;
    }

    TomlValue *_Mytoml_NewArrayValue()
    {
        TomlValue *v = (TomlValue *)calloc(1, sizeof(TomlValue));
        v->type = TOML_ARRAY;
        v->arr = (TomlValue **)calloc(1, sizeof(TomlValue *) * MYTOML_MAX_ARRAY_LENGTH);
        v->len = 0;
        return v;
    }

    TomlValue *_Mytoml_NewTableValue(TomlKey *k)
    {
        TomlValue *v = (TomlValue *)calloc(1, sizeof(TomlValue));
        v->type = TOML_INLINETABLE;
        TomlKey *h = _Mytoml_NewKey(TOML_KEY);
        for (khiter_t ki = kh_begin(k->subkeys);
             ki != kh_end(k->subkeys);
             ++ki)
        {
            if (kh_exist(k->subkeys, ki))
            {
                _Mytoml_AddSubKey(h, kh_value(k->subkeys, ki));
            }
        }
        v->data = h;
        return v;
    }

    void _Mytoml_FreeValue(TomlValue *v)
    {
        if (!v)
            return;
        if (v->arr)
        {
            for (TomlValue **iter = v->arr; *iter != NULL; iter++)
            {
                _Mytoml_FreeValue(*iter);
            }
            free(v->arr);
        }
        if (v->data)
        {
            free(v->data);
        }
        free(v);
    }

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Key
    //-----------------------------------------------------------------------------

    TomlKey *_Mytoml_NewKey(TomlKeyType type)
    {
        TomlKey *k = (TomlKey *)calloc(1, sizeof(TomlKey));
        k->type = type;
        k->value = NULL;
        k->idx = -1;
        k->subkeys = kh_init(str);
        memset(k->id, 0, MYTOML_MAX_ID_LENGTH);
        return k;
    }

    TomlKey *_Mytoml_HasSubKey(TomlKey *key, TomlKey *subkey)
    {
        khiter_t k = kh_get(str, key->subkeys, subkey->id);
        if (k == kh_end(key->subkeys))
            return NULL;
        return kh_value(key->subkeys, k);
    }

    TomlKey *_Mytoml_AddSubKey(TomlKey *key, TomlKey *subkey)
    {
        TomlKey *s = _Mytoml_HasSubKey(key, subkey);
        if (s)
        {
            if (_Mytoml_CompatibleKeys(s->type, subkey->type))
            {
                // re-defining a TABLE as a TABLELEAF
                // is allowed only once
                if (subkey->type == TOML_TABLELEAF)
                {
                    s->type = TOML_TABLELEAF;
                }
                return s;
            }
            else
            {
                RETURN_IF_FAILED(0,
                                 "failed to add subkey\n"
                                 "existing subkey - key: %s type: %d\n"
                                 "new subkey: key: %s type: %d\n",
                                 s->id, (int)(s->type),
                                 subkey->id, (int)(subkey->type));
            }
        }
        if (kh_size(key->subkeys) < MYTOML_MAX_SUBKEYS)
        {
            if (key->type == TOML_ARRAYTABLE)
            {
                // since an ARRAYTABLE is a list of a map of key-value,
                // and re-defining an ARRAYTABLE means adding another map
                // of key-value to the list, we use the `value->arr`
                // attribute of the key to store each map of key-values
                TomlKey *a = _Mytoml_AddSubKey((TomlKey *)key->value->arr[key->idx]->data, subkey);
                return a;
            }
            else
            {
                int ret;
                khiter_t k = kh_put(str, key->subkeys, subkey->id, &ret);
                kh_value(key->subkeys, k) = subkey;
                return subkey;
            }
        }
        else
        {
            LOG_ERR("buffer overflow\n");
        }
        return NULL;
    }

    bool _Mytoml_CompatibleKeys(TomlKeyType existing, TomlKeyType current)
    {
        // re-definition rules
        // [existing]
        // [current]

        // `a = b`
        // `a = c`
        if (existing == TOML_KEYLEAF)
        {
            return false;
        }
        // `[a.b]`
        // `[a.b]`
        if (existing == TOML_TABLELEAF && current == TOML_TABLELEAF)
        {
            return false;
        }
        // `[a.b] or b.c = d`
        // `[a.b.e] or [b.e]`
        if ((existing == TOML_TABLELEAF || existing == TOML_KEY) &&
            current == TOML_TABLE)
        {
            return true;
        }
        // `[a.b]`
        // `[a]`
        if (existing == TOML_TABLE && current == TOML_TABLELEAF)
        {
            return true;
        }
        // `[[t]]`
        // `[t.s]`
        if (existing == TOML_ARRAYTABLE && current == TOML_TABLE)
        {
            return true;
        }
        if (current == existing)
        {
            return true;
        }
        return false;
    }

    void _Mytoml_DeleteKey(TomlKey *key)
    {
        if (!key)
            return;
        kh_destroy(str, key->subkeys);
        if (key->value)
        {
            _Mytoml_FreeValue(key->value);
        }
        free(key);
    }

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Parser Utils
    //-----------------------------------------------------------------------------

    bool _Mytoml_IsWhitespace(char c)
    {
        return (c == ' ' || c == '\t');
    }

    bool _Mytoml_IsNewline(char c)
    {
        return (c == '\n');
    }

    bool _Mytoml_IsReturn(char c)
    {
        return (c == '\r');
    }

    bool _Mytoml_IsCommentStart(char c)
    {
        return (c == '#');
    }

    bool _Mytoml_IsEqual(char c)
    {
        return (c == '=');
    }

    bool _Mytoml_IsEscape(char c)
    {
        return (c == '\\');
    }

    bool _Mytoml_IsBasicStringStart(char c)
    {
        return (c == '"');
    }

    bool _Mytoml_IsLiteralStringStart(char c)
    {
        return (c == '\'');
    }

    bool _Mytoml_IsTableStart(char c)
    {
        return (c == '[');
    }

    bool _Mytoml_IsTableEnd(char c)
    {
        return (c == ']');
    }

    bool _Mytoml_IsInlineTableStart(char c)
    {
        return (c == '{');
    }

    bool _Mytoml_IsInlineTableEnd(char c)
    {
        return (c == '}');
    }

    bool _Mytoml_IsInlineTableSep(char c)
    {
        return (c == ',');
    }

    bool _Mytoml_IsDot(char c)
    {
        return (c == '.');
    }

    bool _Mytoml_IsDigit(char c)
    {
        return (c >= '0' && c <= '9');
    }

    bool _Mytoml_IsHexDigit(char c)
    {
        return (
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'));
    }

    bool _Mytoml_IsNumberStart(char c)
    {
        return (
            (c == '+' || c == '-') ||
            _Mytoml_IsDigit(c));
    }

    bool _Mytoml_IsBareAscii(char c)
    {
        return (
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c == '_' || c == '-') ||
            _Mytoml_IsDigit(c));
    }

    bool _Mytoml_Iscontrol(char c)
    {
        return (
            (c >= 0x0 && c <= 0x8) ||
            (c >= 0xA && c <= 0x1F) ||
            (c == 0x7F));
    }

    bool _Mytoml_IsControlMulti(char c)
    {
        return (
            (c >= 0x0 && c <= 0x8) ||
            (c == 0xB || c == 0xC) ||
            (c >= 0xE && c <= 0x1F) ||
            (c == 0x7F));
    }

    bool _Mytoml_IsControlLiteral(char c)
    {
        return (
            ((c != 0x9) &&
             (c != 0xA) &&
             (c >= 0x0 && c <= 0x1F)) ||
            (c == 0x7F));
    }

    bool _Mytoml_IsNumberEnd(char c, const char *end)
    {
        for (size_t i = 0; i < strlen(end); i++)
        {
            if (c == end[i])
                return true;
        }
        return false;
    }

    bool _Mytoml_IsDecimalPoint(char c)
    {
        return (c == '.');
    }

    bool _Mytoml_IsUnderscore(char c)
    {
        return (c == '_');
    }

    bool _Mytoml_IsArrayStart(char c)
    {
        return (c == '[');
    }

    bool _Mytoml_IsArrayEnd(char c)
    {
        return (c == ']');
    }

    bool _Mytoml_IsArraySep(char c)
    {
        return (c == ',');
    }

    bool _Mytoml_IsDate(int year, int month, int day)
    {
        switch (month)
        {
        case 0: // January
            return (day >= 1 && day <= 31);
        case 1: // February
            return ((day >= 1 && day <= 28) ||
                    (day == 29 && year % 4 == 0 && year % 100 != 0) ||
                    (day == 29 && year % 4 == 0 && year % 100 == 0 && year % 400 == 0));
        case 2: // March
            return (day >= 1 && day <= 31);
        case 3: // April
            return (day >= 1 && day <= 30);
        case 4: // May
            return (day >= 1 && day <= 31);
        case 5: // June
            return (day >= 1 && day <= 30);
        case 6: // July
            return (day >= 1 && day <= 31);
        case 7: // August
            return (day >= 1 && day <= 31);
        case 8: // September
            return (day >= 1 && day <= 30);
        case 9: // October
            return (day >= 1 && day <= 31);
        case 10: // November
            return (day >= 1 && day <= 30);
        case 11: // December
            return (day >= 1 && day <= 31);
        default:
            return false;
        }
        return false;
    }

    bool _Mytoml_IsValidDatetime(struct tm *datetime)
    {
        return ((datetime->tm_hour >= 0 && datetime->tm_hour <= 23) &&
                (datetime->tm_min >= 0 && datetime->tm_min <= 59) &&
                (datetime->tm_sec >= 0 && datetime->tm_sec <= 59) &&
                _Mytoml_IsDate(datetime->tm_year + 1900, datetime->tm_mon, datetime->tm_mday));
    }

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Parser Key
    //-----------------------------------------------------------------------------

    TomlKey *_Mytoml_ParserBareKey(Tokenizer *tok, char end, TomlKeyType branch, TomlKeyType leaf)
    {
        char id[MYTOML_MAX_ID_LENGTH] = {0};
        int idx = 0;
        bool done = false;

        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(idx < MYTOML_MAX_ID_LENGTH, "buffer overflow\n");
            if (_Mytoml_IsDot(_Mytoml_TokenizerGetToken(tok)))
            {
                RETURN_IF_FAILED(idx != 0, "key cannot be empty\n");
                TomlKey *subkey = _Mytoml_NewKey(branch);
                memcpy(subkey->id, id, strlen(id));
                return subkey;
            }
            else if (_Mytoml_TokenizerGetToken(tok) == end)
            {
                RETURN_IF_FAILED(idx != 0, "key cannot be empty\n");
                TomlKey *subkey = _Mytoml_NewKey(leaf);
                memcpy(subkey->id, id, strlen(id));
                return subkey;
            }
            else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
            {
                // bare keys cannot contain whitespace inside each
                // key, so we use the `done` variable to track it
                done = true;
                _Mytoml_ParseWhitespace(tok);
            }
            else if (_Mytoml_IsBareAscii(_Mytoml_TokenizerGetToken(tok)) && !done)
            {
                id[idx++] = _Mytoml_TokenizerGetToken(tok);
                _Mytoml_NextTokenizerToken(tok);
            }
            else
            {
                LOG_ERR("unknown character %c\n", _Mytoml_TokenizerGetToken(tok));
                break;
            }
        }
        return NULL;
    }

    TomlKey *_Mytoml_ParserBAsicQuotedKey(Tokenizer *tok, char end, TomlKeyType branch, TomlKeyType leaf)
    {
        char id[MYTOML_MAX_ID_LENGTH] = {0};
        int idx = 0;

        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(idx < MYTOML_MAX_ID_LENGTH, "buffer overflow\n");
            if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
                {
                    _Mytoml_ParseWhitespace(tok);
                }
                if (_Mytoml_IsDot(_Mytoml_TokenizerGetToken(tok)))
                {
                    TomlKey *subkey = _Mytoml_NewKey(branch);
                    memcpy(subkey->id, id, strlen(id));
                    return subkey;
                }
                else if (_Mytoml_TokenizerGetToken(tok) == end)
                {
                    TomlKey *subkey = _Mytoml_NewKey(leaf);
                    memcpy(subkey->id, id, strlen(id));
                    return subkey;
                }
                LOG_ERR("unknown character %c after end of key\n", _Mytoml_TokenizerGetToken(tok));
                break;
            }
            else if (_Mytoml_IsNewline(_Mytoml_TokenizerGetToken(tok)))
            {
                LOG_ERR("unescaped newline while parsing key\n");
                break;
            }
            else if (_Mytoml_IsEscape(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                char escaped[5] = {0};
                int c = _Mytoml_ParseEscape(tok, escaped, 5);
                RETURN_IF_FAILED(c != 0, "unknown escape sequence \\%c\n", _Mytoml_TokenizerGetToken(tok));
                RETURN_IF_FAILED(c < 5, "parsed escape sequence is too long\n");
                for (int i = 0; i < c; i++)
                {
                    id[idx++] = escaped[i];
                    RETURN_IF_FAILED(idx < MYTOML_MAX_ID_LENGTH, "buffer overflow\n");
                }
                // _Mytoml_ParseEscape will parse everything and move on to the next token
                // so we call _Mytoml_TokenizerBacktrace here to offset the _Mytoml_NextTokenizerToken call outside
                _Mytoml_TokenizerBacktrace(tok, 1);
            }
            else if (_Mytoml_Iscontrol(_Mytoml_TokenizerGetToken(tok)))
            {
                LOG_ERR("control characters need to be escaped\n");
                break;
            }
            else
            {
                id[idx++] = _Mytoml_TokenizerGetToken(tok);
            }
            _Mytoml_NextTokenizerToken(tok);
        }
        return NULL;
    }

    TomlKey *_Mytoml_ParserLiteralQuotedKey(Tokenizer *tok, char end, TomlKeyType branch, TomlKeyType leaf)
    {
        char id[MYTOML_MAX_ID_LENGTH] = {0};
        int idx = 0;

        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(idx < MYTOML_MAX_ID_LENGTH, "buffer overflow\n");
            if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
                {
                    _Mytoml_ParseWhitespace(tok);
                }
                if (_Mytoml_IsDot(_Mytoml_TokenizerGetToken(tok)))
                {
                    TomlKey *subkey = _Mytoml_NewKey(branch);
                    memcpy(subkey->id, id, strlen(id));
                    return subkey;
                }
                else if (_Mytoml_TokenizerGetToken(tok) == end)
                {
                    TomlKey *subkey = _Mytoml_NewKey(leaf);
                    memcpy(subkey->id, id, strlen(id));
                    return subkey;
                }
                LOG_ERR("unknown character %c after end of key\n", _Mytoml_TokenizerGetToken(tok));
                break;
            }
            else if (_Mytoml_IsNewline(_Mytoml_TokenizerGetToken(tok)))
            {
                LOG_ERR("unescaped newline while parsing key\n");
                break;
            }
            else if (_Mytoml_IsControlLiteral(_Mytoml_TokenizerGetToken(tok)))
            {
                LOG_ERR("control characters need to be escaped\n");
                break;
            }
            else
            {
                id[idx++] = _Mytoml_TokenizerGetToken(tok);
            }
            _Mytoml_NextTokenizerToken(tok);
        }
        return NULL;
    }

    TomlKey *_Mytoml_ParseKey(Tokenizer *tok, TomlKey *key, bool expecting)
    {
        while (_Mytoml_TokenizerHasToken(tok))
        {
            if (_Mytoml_IsEqual(_Mytoml_TokenizerGetToken(tok)))
            {
                RETURN_IF_FAILED(!expecting, "found = while expecting a key\n");
                _Mytoml_NextTokenizerToken(tok);
                return key;
            }
            else if (_Mytoml_IsDot(_Mytoml_TokenizerGetToken(tok)))
            {
                RETURN_IF_FAILED(!expecting, "found . while expecting a key\n");
                _Mytoml_NextTokenizerToken(tok);
                return _Mytoml_ParseKey(tok, key, true);
            }
            else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_ParseWhitespace(tok);
            }
            else if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                TomlKey *subkey = _Mytoml_ParserBAsicQuotedKey(tok, '=', TOML_KEY, TOML_KEYLEAF);
                RETURN_IF_FAILED(subkey, "failed to parse basic quoted key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add subkey to key %s\n", key->id);
                return _Mytoml_ParseKey(tok, subkey, false);
            }
            else if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                TomlKey *subkey = _Mytoml_ParserLiteralQuotedKey(tok, '=', TOML_KEY, TOML_KEYLEAF);
                RETURN_IF_FAILED(subkey, "failed to parse literal quoted key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add subkey to key %s\n", key->id);
                return _Mytoml_ParseKey(tok, subkey, false);
            }
            else
            {
                TomlKey *subkey = _Mytoml_ParserBareKey(tok, '=', TOML_KEY, TOML_KEYLEAF);
                RETURN_IF_FAILED(subkey, "failed to parse bare key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add subkey to key %s\n", key->id);
                return _Mytoml_ParseKey(tok, subkey, false);
            }
        }
        return NULL;
    }

    TomlKey *_Mytoml_ParseTable(Tokenizer *tok, TomlKey *key, bool expecting)
    {
        while (_Mytoml_TokenizerHasToken(tok))
        {
            if (_Mytoml_IsTableEnd(_Mytoml_TokenizerGetToken(tok)))
            {
                RETURN_IF_FAILED(!expecting, "found ] while expecting a key\n");
                _Mytoml_NextTokenizerToken(tok);
                return key;
            }
            else if (_Mytoml_IsDot(_Mytoml_TokenizerGetToken(tok)))
            {
                RETURN_IF_FAILED(!expecting, "found . while expecting a key\n");
                _Mytoml_NextTokenizerToken(tok);
                return _Mytoml_ParseTable(tok, key, true);
            }
            else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_ParseWhitespace(tok);
            }
            else if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                TomlKey *subkey = _Mytoml_ParserBAsicQuotedKey(tok, ']', TOML_TABLE, TOML_TABLELEAF);
                RETURN_IF_FAILED(subkey, "failed to parse basic quoted key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add key to subkey %s\n", key->id);
                return _Mytoml_ParseTable(tok, subkey, false);
            }
            else if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                TomlKey *subkey = _Mytoml_ParserLiteralQuotedKey(tok, ']', TOML_TABLE, TOML_TABLELEAF);
                RETURN_IF_FAILED(subkey, "failed to parse literal quoted key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add key to subkey %s\n", key->id);
                return _Mytoml_ParseTable(tok, subkey, false);
            }
            else
            {
                TomlKey *subkey = _Mytoml_ParserBareKey(tok, ']', TOML_TABLE, TOML_TABLELEAF);
                RETURN_IF_FAILED(subkey, "failed to parse bare key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add key to subkey %s\n", key->id);
                return _Mytoml_ParseTable(tok, subkey, false);
            }
        }
        return NULL;
    }

    TomlKey *_Mytoml_ParseArrayTable(Tokenizer *tok, TomlKey *key, bool expecting)
    {
        while (_Mytoml_TokenizerHasToken(tok))
        {
            if (_Mytoml_IsTableEnd(_Mytoml_TokenizerGetToken(tok)))
            {
                RETURN_IF_FAILED(!expecting, "found ] while expecting a key\n");
                _Mytoml_NextTokenizerToken(tok);
                RETURN_IF_FAILED(_Mytoml_IsTableEnd(_Mytoml_TokenizerGetToken(tok)), "found ] while expecting a key\n");
                _Mytoml_NextTokenizerToken(tok);
                return key;
            }
            else if (_Mytoml_IsDot(_Mytoml_TokenizerGetToken(tok)))
            {
                RETURN_IF_FAILED(!expecting, "found . while expecting a key\n");
                _Mytoml_NextTokenizerToken(tok);
                return _Mytoml_ParseArrayTable(tok, key, true);
            }
            else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_ParseWhitespace(tok);
            }
            else if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                TomlKey *subkey = _Mytoml_ParserBAsicQuotedKey(tok, ']', TOML_TABLE, TOML_ARRAYTABLE);
                RETURN_IF_FAILED(subkey, "failed to parse basic quoted key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add key to subkey %s\n", key->id);
                return _Mytoml_ParseArrayTable(tok, subkey, false);
            }
            else if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                TomlKey *subkey = _Mytoml_ParserLiteralQuotedKey(tok, ']', TOML_TABLE, TOML_ARRAYTABLE);
                RETURN_IF_FAILED(subkey, "failed to parse literal quoted key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add key to subkey %s\n", key->id);
                return _Mytoml_ParseArrayTable(tok, subkey, false);
            }
            else
            {
                TomlKey *subkey = _Mytoml_ParserBareKey(tok, ']', TOML_TABLE, TOML_ARRAYTABLE);
                RETURN_IF_FAILED(subkey, "failed to parse bare key\n");
                subkey = _Mytoml_AddSubKey(key, subkey);
                RETURN_IF_FAILED(subkey, "failed to add key to subkey %s\n", key->id);
                return _Mytoml_ParseArrayTable(tok, subkey, false);
            }
        }
        return NULL;
    }

    TomlKey *_Mytoml_ParseKeyValue(Tokenizer *tok, TomlKey *key, TomlKey *root)
    {
        if (_Mytoml_IsCommentStart(_Mytoml_TokenizerGetToken(tok)))
        {
            bool ok = _Mytoml_ParseComment(tok);
            RETURN_IF_FAILED(ok, "invalid comment\n");
            return key;
        }
        else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
        {
            _Mytoml_ParseWhitespace(tok);
            return key;
        }
        else if (_Mytoml_ParseNewline(tok))
        {
            _Mytoml_NextTokenizerToken(tok);
            return key;
        }
        else if (_Mytoml_IsTableStart(_Mytoml_TokenizerGetToken(tok)))
        {
            _Mytoml_NextTokenizerToken(tok);
            TomlKey *table;
            // [[ means we are parsing an arraytable
            if (_Mytoml_IsTableStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                table = _Mytoml_ParseArrayTable(tok, root, true);
                RETURN_IF_FAILED(table, "failed to parse array of tables\n");
                // Since an arraytable is a map of key-value pairs, we
                // store it in the `value->arr` attribute of the `key`.
                // Each redefinition marks an new element in that array.
                // The key-value pairs are added to the `subkeys` of a
                // "pseudo" key that lives at `table->value->arr[ table->idx ].
                if (table->value == NULL)
                {
                    table->value = _Mytoml_NewArrayValue();
                }
                RETURN_IF_FAILED(table->idx < MYTOML_MAX_ARRAY_LENGTH - 1, "buffer overflow\n");
                table->value->arr[++(table->idx)] = _Mytoml_NewTableValue(_Mytoml_NewKey(TOML_TABLE));
            }
            else
            {
                table = _Mytoml_ParseTable(tok, root, true);
                RETURN_IF_FAILED(table, "failed to parse table\n");
            }
            return table;
        }
        else if (_Mytoml_TokenizerGetPrev(tok) == '\0' ||
                 _Mytoml_IsNewline(_Mytoml_TokenizerGetPrev(tok)) ||
                 // ignore white space found at the beginning of
                 // a line while parsing a key
                 (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetPrev(tok)) &&
                  tok->newline))
        {
            TomlKey *subkey = _Mytoml_ParseKey(tok, key, true);
            RETURN_IF_FAILED(subkey, "failed to parse key\n");
            TomlValue *v = _Mytoml_ParseValue(tok, "# \n");
            RETURN_IF_FAILED(v, "failed to parse value\n");
            // If we parsed an inlinetable, to keep it in sync
            // with our datastructure, we add the keys from the
            // parsed key-value pairs as `subkeys` of the "active"
            // `key`. Since the inline table is defined as
            // `a = b`, the type would be a KEYLEAF. Since KEYLEAF
            // re-definitions are not allowed, we "unlock" it as a
            // KEY, add the `subkeys` and "lock" it again as a
            // `KEYLEAF` to prevent re-definition.
            if (v->type == TOML_INLINETABLE)
            {
                TomlKey *h = (TomlKey *)(v->data);
                subkey->type = TOML_KEY;
                for (khiter_t ki = kh_begin(h->subkeys);
                     ki != kh_end(h->subkeys);
                     ++ki)
                {
                    if (kh_exist(h->subkeys, ki))
                    {
                        TomlKey *e = _Mytoml_AddSubKey(subkey, kh_value(h->subkeys, ki));
                        RETURN_IF_FAILED(e, "could not add inline table key %s\n", kh_value(h->subkeys, ki)->id);
                    }
                }
                subkey->type = TOML_KEYLEAF;
            }
            else
            {
                subkey->value = v;
            }
            _Mytoml_ParseWhitespace(tok);
            return key;
        }
        else
            LOG_ERR("unhandled character %c\n", _Mytoml_TokenizerGetToken(tok));
        return NULL;
    }

    //-----------------------------------------------------------------------------
    // [SECTION] Myjson Parser Value
    //-----------------------------------------------------------------------------

    char *_Mytoml_ParseBasicString(Tokenizer *tok, char *value, bool multi)
    {
        int idx = 0;
        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(idx < MYTOML_MAX_STRING_LENGTH, "buffer overflow\n");
            if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                if (!multi)
                {
                    _Mytoml_NextTokenizerToken(tok);
                    return value;
                }
                else
                {
                    int a = _Mytoml_NextTokenizerToken(tok);
                    int b = _Mytoml_NextTokenizerToken(tok);
                    if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)) &&
                        _Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetPrev(tok)))
                    {
                        _Mytoml_NextTokenizerToken(tok);
                        if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
                        {
                            value[idx++] = '"';
                            _Mytoml_NextTokenizerToken(tok);
                        }
                        RETURN_IF_FAILED(idx < MYTOML_MAX_STRING_LENGTH, "buffer overflow\n");
                        if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
                        {
                            value[idx++] = '"';
                            _Mytoml_NextTokenizerToken(tok);
                        }
                        return value;
                    }
                    else
                    {
                        value[idx++] = '"';
                        _Mytoml_TokenizerBacktrace(tok, a + b - 1);
                        continue;
                    }
                }
            }
            else if (_Mytoml_ParseNewline(tok) && !multi)
            {
                LOG_ERR("newline before end of string\n");
                break;
            }
            else if (_Mytoml_ParseNewline(tok) && multi && idx == 0)
                ;
            else if (_Mytoml_IsEscape(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                char escaped[5] = {0};
                int c = _Mytoml_ParseEscape(tok, escaped, 5);
                if (multi && c == 0)
                {
                    bool hit = false;
                    while (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)) ||
                           _Mytoml_ParseNewline(tok))
                    {
                        if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
                        {
                            _Mytoml_ParseWhitespace(tok);
                        }
                        if (_Mytoml_ParseNewline(tok))
                        {
                            hit = true;
                            _Mytoml_NextTokenizerToken(tok);
                        }
                    }
                    RETURN_IF_FAILED(hit, "cannot have characters on same line after \\\n");
                    continue;
                }
                else
                {
                    RETURN_IF_FAILED(c != 0, "unknown escape sequence \\%c\n", _Mytoml_TokenizerGetToken(tok));
                    RETURN_IF_FAILED(c < 5, "parsed escape sequence is too long\n");
                    for (int i = 0; i < c; i++)
                    {
                        value[idx++] = escaped[i];
                        RETURN_IF_FAILED(idx < MYTOML_MAX_STRING_LENGTH, "buffer overflow\n");
                    }
                    // _Mytoml_ParseEscape will parse everything and move on to the next token
                    // so we call _Mytoml_TokenizerBacktrace here to offset the _Mytoml_NextTokenizerToken call outside
                    _Mytoml_TokenizerBacktrace(tok, 1);
                }
            }
            else if (!multi && _Mytoml_Iscontrol(_Mytoml_TokenizerGetToken(tok)))
            {
                LOG_ERR("control characters need to be escaped\n");
                break;
            }
            else if (multi && _Mytoml_IsControlMulti(_Mytoml_TokenizerGetToken(tok)))
            {
                LOG_ERR("control characters need to be escaped\n");
                break;
            }
            else
            {
                value[idx++] = _Mytoml_TokenizerGetToken(tok);
            }
            _Mytoml_NextTokenizerToken(tok);
        }
        return NULL;
    }

    char *_Mytoml_ParseLiteralString(Tokenizer *tok, char *value, bool multi)
    {
        int idx = 0;
        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(idx < MYTOML_MAX_STRING_LENGTH, "buffer overflow\n");
            if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                if (!multi)
                {
                    _Mytoml_NextTokenizerToken(tok);
                    return value;
                }
                else
                {
                    int a = _Mytoml_NextTokenizerToken(tok);
                    int b = _Mytoml_NextTokenizerToken(tok);
                    if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)) &&
                        _Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetPrev(tok)))
                    {
                        _Mytoml_NextTokenizerToken(tok);
                        if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
                        {
                            value[idx++] = '\'';
                            _Mytoml_NextTokenizerToken(tok);
                        }
                        RETURN_IF_FAILED(idx < MYTOML_MAX_STRING_LENGTH, "buffer overflow\n");
                        if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
                        {
                            value[idx++] = '\'';
                            _Mytoml_NextTokenizerToken(tok);
                        }
                        return value;
                    }
                    else
                    {
                        value[idx++] = '\'';
                        _Mytoml_TokenizerBacktrace(tok, a + b - 1);
                        continue;
                    }
                }
            }
            else if (_Mytoml_ParseNewline(tok) && !multi)
            {
                LOG_ERR("newline before end of string\n");
                break;
            }
            else if (_Mytoml_ParseNewline(tok) && multi && idx == 0)
                ;
            else if (_Mytoml_IsControlLiteral(_Mytoml_TokenizerGetToken(tok)))
            {
                LOG_ERR("control characters need to be escaped\n");
                break;
            }
            else
            {
                value[idx++] = _Mytoml_TokenizerGetToken(tok);
            }
            _Mytoml_NextTokenizerToken(tok);
        }
        return NULL;
    }

    Datetime *_Mytoml_ParseDatetime(Tokenizer *tok, char *value, const char *num_end, struct tm *time)
    {
        Datetime *dt = NULL;
        int idx = 0;
        // check to allow only 1 whitespace character
        int spaces = 0;
        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(idx < MYTOML_MAX_STRING_LENGTH, "buffer overflow\n");
            if ((_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)) && spaces) ||
                (!_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)) && _Mytoml_IsNumberEnd(_Mytoml_TokenizerGetToken(tok), num_end)))
            {
                int millis = 0;
                char year[5] = {0};
                char mon[3] = {0};
                char mday[3] = {0};
                char hour[3] = {0};
                char min[3] = {0};
                char sec[3] = {0};
                char delim[2] = {0};
                char tz[2] = {0};
                char off_s[2] = {0};
                char off_h[3] = {0};
                char off_m[3] = {0};

                char *end;
                unsigned long num;
                int t;

                // DATETIME with millisecond and offset
                t = sscanf(value, "%4c-%2c-%2c%1c%2c:%2c:%2c.%d%1c%2c:%2c",
                           year, mon, mday, delim, hour, min, sec, &millis, off_s, off_h, off_m);
                if (t == 11)
                {
                    RETURN_IF_FAILED(strlen(delim) == 1, "invalid delimiter\n");
                    RETURN_IF_FAILED(strlen(off_s) == 1, "invalid offset sign\n");
                    RETURN_IF_FAILED((delim[0] == 'T' || delim[0] == 't' || delim[0] == ' '), "invalid delimiter\n");
                    RETURN_IF_FAILED((off_s[0] == '+' || off_s[0] == '-'), "invalid offset sign\n");

                    CHECK_DATE();
                    CHECK_TIME();
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified offset datetime is not valid\n");

                    CHECK_DATETIME(off_h, 2, "invalid offset hour\n");
                    RETURN_IF_FAILED((num >= 0Ul && num <= 23Ul), "invalid offset hour\n");
                    // TODO : Fix
                    //  time->tm_gmtoff = num * 60 * 60;
                    CHECK_DATETIME(off_m, 2, "invalid offset minute\n");
                    RETURN_IF_FAILED((num >= 0Ul && num <= 59Ul), "invalid offset minute\n");
                    // TODO : Fix
                    //  time->tm_gmtoff += num;

                    // TODO : Fix
                    // if (off_s[0] == '-')
                    // {
                    //     time->tm_gmtoff *= -1;
                    // }
                    if (delim[0] == ' ')
                    {
                        spaces = 0;
                    }

                    double mlen = floor(log10(abs(millis))) + 1;
                    if (mlen == 1)
                        millis *= 100;
                    if (mlen == 2)
                        millis *= 10;

                    RETURN_IF_FAILED((strlen(value) == (strlen("YYYY-mm-DDTHH:MM:SS.-HH:MM") + mlen + spaces)),
                                     "datetime has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_DATETIME;
                    dt->dt = time;
                    mlen = (mlen > 3) ? mlen : 3;
                    dt->millis = millis;
                    int sz = strlen("%Y-%m-%dT%H:%M:%S.-HH:MM") + mlen + 1;
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    snprintf(dt->format, sz, "%%Y-%%m-%%dT%%H:%%M:%%S.%d%c%s:%s", millis, off_s[0], off_h, off_m);
                    return dt;
                }
                // DATETIME with offset
                t = sscanf(value, "%4c-%2c-%2c%1c%2c:%2c:%2c%1c%2c:%2c",
                           year, mon, mday, delim, hour, min, sec, off_s, off_h, off_m);
                if (t == 10)
                {
                    RETURN_IF_FAILED(strlen(delim) == 1, "invalid delimiter\n");
                    RETURN_IF_FAILED(strlen(off_s) == 1, "invalid offset sign\n");
                    RETURN_IF_FAILED((delim[0] == 'T' || delim[0] == 't' || delim[0] == ' '), "invalid delimiter\n");
                    RETURN_IF_FAILED((off_s[0] == '+' || off_s[0] == '-'), "invalid offset sign\n");

                    CHECK_DATE();
                    CHECK_TIME();
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified offset datetime is not valid\n");

                    CHECK_DATETIME(off_h, 2, "invalid offset hour\n");
                    RETURN_IF_FAILED((num >= 0 && num <= 23), "invalid offset hour\n");
                    // time->tm_gmtoff = num * 60 * 60;
                    CHECK_DATETIME(off_m, 2, "invalid offset minute\n");
                    RETURN_IF_FAILED((num >= 0 && num <= 59), "invalid offset minute\n");
                    // time->tm_gmtoff += num;

                    // if (off_s[0] == '-')
                    // {
                    //     time->tm_gmtoff *= -1;
                    // }
                    if (delim[0] == ' ')
                    {
                        spaces = 0;
                    }

                    RETURN_IF_FAILED((strlen(value) == (strlen("YYYY-mm-DDTHH:MM:SS-HH:MM") + spaces)),
                                     "datetime has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_DATETIME;
                    dt->dt = time;
                    int sz = strlen("%Y-%m-%dT%H:%M:%S-HH:MM") + 1;
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    snprintf(dt->format, sz, "%%Y-%%m-%%dT%%H:%%M:%%S%c%s:%s", off_s[0], off_h, off_m);
                    return dt;
                }
                // DATETIME with millisecond and timezone
                t = sscanf(value, "%4c-%2c-%2c%1c%2c:%2c:%2c.%d%1c",
                           year, mon, mday, delim, hour, min, sec, &millis, tz);
                if (t == 9)
                {
                    RETURN_IF_FAILED(strlen(delim) == 1, "invalid delimiter\n");
                    RETURN_IF_FAILED(strlen(tz) == 1, "invalid timezone\n");
                    RETURN_IF_FAILED((delim[0] == 'T' || delim[0] == 't' || delim[0] == ' '), "invalid delimiter\n");
                    RETURN_IF_FAILED((tz[0] == 'Z' || tz[0] == 'z'), "invalid timezone\n");

                    CHECK_DATE();
                    CHECK_TIME();
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified offset datetime is not valid\n");

                    // time->tm_zone = "UTC";

                    if (delim[0] == ' ')
                    {
                        spaces = 0;
                    }

                    double mlen = floor(log10(abs(millis))) + 1;
                    if (mlen == 1)
                        millis *= 100;
                    if (mlen == 2)
                        millis *= 10;

                    RETURN_IF_FAILED((strlen(value) == (strlen("YYYY-mm-DDTHH:MM:SS.Z") + mlen + spaces)),
                                     "datetime has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_DATETIME;
                    dt->dt = time;
                    mlen = (mlen > 3) ? mlen : 3;
                    dt->millis = millis;
                    int sz = strlen("%Y-%m-%dT%H:%M:%S.Z") + mlen + 1;
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    snprintf(dt->format, sz, "%%Y-%%m-%%dT%%H:%%M:%%S.%dZ", millis);
                    return dt;
                }
                // DATETIMELOCAL with millisecond
                t = sscanf(value, "%4c-%2c-%2c%1c%2c:%2c:%2c.%d",
                           year, mon, mday, delim, hour, min, sec, &millis);
                if (t == 8)
                {
                    RETURN_IF_FAILED(strlen(delim) == 1, "invalid delimiter\n");
                    RETURN_IF_FAILED((delim[0] == 'T' || delim[0] == 't' || delim[0] == ' '), "invalid delimiter\n");

                    CHECK_DATE();
                    CHECK_TIME();
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified local datetime is not valid\n");

                    if (delim[0] == ' ')
                    {
                        spaces = 0;
                    }

                    double mlen = floor(log10(abs(millis))) + 1;
                    if (mlen == 1)
                        millis *= 100;
                    if (mlen == 2)
                        millis *= 10;

                    RETURN_IF_FAILED((strlen(value) == (strlen("YYYY-mm-DDTHH:MM:SS.") + mlen + spaces)),
                                     "datetime has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_DATETIMELOCAL;
                    dt->dt = time;
                    mlen = (mlen > 3) ? mlen : 3;
                    dt->millis = millis;
                    int sz = strlen("%Y-%m-%dT%H:%M:%S.") + mlen + 1;
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    snprintf(dt->format, sz, "%%Y-%%m-%%dT%%H:%%M:%%S.%d", millis);
                    return dt;
                }
                // DATETIME with timezone
                t = sscanf(value, "%4c-%2c-%2c%1c%2c:%2c:%2c%1c",
                           year, mon, mday, delim, hour, min, sec, tz);
                if (t == 8)
                {
                    RETURN_IF_FAILED(strlen(delim) == 1, "invalid delimiter\n");
                    RETURN_IF_FAILED(strlen(tz) == 1, "invalid timezone\n");
                    RETURN_IF_FAILED((delim[0] == 'T' || delim[0] == 't' || delim[0] == ' '), "invalid delimiter\n");
                    RETURN_IF_FAILED((tz[0] == 'Z' || tz[0] == 'z'), "invalid timezone\n");

                    CHECK_DATE();
                    CHECK_TIME();
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified offset datetime is not valid\n");

                    // time->tm_zone = "UTC";

                    if (delim[0] == ' ')
                    {
                        spaces = 0;
                    }

                    double mlen = floor(log10(abs(millis))) + 1;
                    if (mlen == 1)
                        millis *= 100;
                    if (mlen == 2)
                        millis *= 10;

                    RETURN_IF_FAILED((strlen(value) == (strlen("YYYY-mm-DDTHH:MM:SSZ") + spaces)),
                                     "datetime has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_DATETIME;
                    dt->dt = time;
                    int sz = strlen("%Y-%m-%dT%H:%M:%SZ") + 1;
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    snprintf(dt->format, sz, "%%Y-%%m-%%dT%%H:%%M:%%SZ");
                    return dt;
                }
                // DATETIMELOCAL
                t = sscanf(value, "%4c-%2c-%2c%1c%2c:%2c:%2c",
                           year, mon, mday, delim, hour, min, sec);
                if (t == 7)
                {
                    RETURN_IF_FAILED(strlen(delim) == 1, "invalid delimiter\n");
                    RETURN_IF_FAILED((delim[0] == 'T' || delim[0] == 't' || delim[0] == ' '), "invalid delimiter\n");

                    CHECK_DATE();
                    CHECK_TIME();
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified local datetime is not valid\n");

                    if (delim[0] == ' ')
                    {
                        spaces = 0;
                    }

                    RETURN_IF_FAILED((strlen(value) == (strlen("YYYY-mm-DDTHH:MM:SS") + spaces)),
                                     "datetime has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_DATETIMELOCAL;
                    dt->dt = time;
                    int sz = strlen("%Y-%m-%dT%H:%M:%S");
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    memcpy(dt->format, "%Y-%m-%dT%H:%M:%S", sz);
                    return dt;
                }
                // DATELOCAL
                t = sscanf(value, "%4c-%2c-%2c",
                           year, mon, mday);
                if (t == 3)
                {
                    CHECK_DATE();
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified local date is not valid\n");

                    RETURN_IF_FAILED((strlen(value) == (strlen("YYYY-mm-DD") + spaces)),
                                     "date has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_DATELOCAL;
                    dt->dt = time;
                    int sz = strlen("%Y-%m-%d");
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    memcpy(dt->format, "%Y-%m-%d", sz);
                    return dt;
                }
                // TIMELOCAL with millisecond
                t = sscanf(value, "%2c:%2c:%2c.%d",
                           hour, min, sec, &millis);
                if (t == 4)
                {
                    CHECK_TIME();
                    time->tm_year = 0;
                    time->tm_mon = 0;
                    time->tm_mday = 1;
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified local time is not valid\n");

                    double mlen = floor(log10(abs(millis))) + 1;
                    if (mlen == 1)
                        millis *= 100;
                    if (mlen == 2)
                        millis *= 10;

                    RETURN_IF_FAILED((strlen(value) == (strlen("HH:MM:SS.") + mlen + spaces)),
                                     "time has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_TIMELOCAL;
                    dt->dt = time;
                    mlen = (mlen > 3) ? mlen : 3;
                    dt->millis = millis;
                    int sz = strlen("%H:%M:%S.") + mlen + 1;
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    snprintf(dt->format, sz, "%%H:%%M:%%S.%d", millis);
                    return dt;
                }
                // TIMELOCAL
                t = sscanf(value, "%2c:%2c:%2c",
                           hour, min, sec);
                if (t == 3)
                {
                    CHECK_TIME();
                    time->tm_year = 0;
                    time->tm_mon = 0;
                    time->tm_mday = 1;
                    RETURN_IF_FAILED(_Mytoml_IsValidDatetime(time), "specified local time is not valid\n");

                    RETURN_IF_FAILED((strlen(value) == (strlen("HH:MM:SS") + spaces)),
                                     "time has incorrect number of characters\n");

                    dt = (Datetime *)calloc(1, sizeof(Datetime));
                    dt->type = TOML_TIMELOCAL;
                    dt->dt = time;
                    int sz = strlen("%H:%M:%S");
                    FUNC_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, free, dt);
                    RETURN_IF_FAILED(sz < MYTOML_MAX_DATE_FORMAT, "datetime string is too long");
                    memcpy(dt->format, "%H:%M:%S", sz);
                    return dt;
                }
#undef CHECK_TIME
#undef CHECK_DATE
#undef CHECK_DATETIME
                LOG_ERR("could not parse %s as datetime\n", value);
                break;
            }
            else
            {
                if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
                {
                    spaces++;
                }
                value[idx++] = _Mytoml_TokenizerGetToken(tok);
            }
            _Mytoml_NextTokenizerToken(tok);
        }
        return NULL;
    }

    double
    _Mytoml_ParseInfNan(
        Tokenizer *tok,
        bool negative)
    {
        double ret = 0.0;
        if (_Mytoml_TokenizerGetToken(tok) == 'i')
        {
            _Mytoml_NextTokenizerToken(tok);
            _Mytoml_NextTokenizerToken(tok);
            if (_Mytoml_TokenizerGetPrev(tok) == 'n' && _Mytoml_TokenizerGetToken(tok) == 'f')
            {
                if (negative)
                {
                    ret = (double)-INFINITY;
                }
                else
                {
                    ret = (double)INFINITY;
                }
            }
        }
        if (_Mytoml_TokenizerGetToken(tok) == 'n')
        {
            _Mytoml_NextTokenizerToken(tok);
            _Mytoml_NextTokenizerToken(tok);
            if (_Mytoml_TokenizerGetPrev(tok) == 'a' && _Mytoml_TokenizerGetToken(tok) == 'n')
            {
                if (negative)
                {
                    ret = (double)-NAN;
                }
                else
                {
                    ret = (double)NAN;
                }
            }
        }
        _Mytoml_NextTokenizerToken(tok);
        return ret;
    }

    TomlValue *
    _Mytoml_ParseArray(
        Tokenizer *tok,
        TomlValue *arr)
    {
        bool sep = true;
        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(arr->len < MYTOML_MAX_ARRAY_LENGTH, "buffer overflow\n");
            if (_Mytoml_IsArrayEnd(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                return arr;
            }
            else if (_Mytoml_IsArraySep(_Mytoml_TokenizerGetToken(tok)))
            {
                RETURN_IF_FAILED(!sep, "expected value but got , instead\n");
                sep = true;
                _Mytoml_NextTokenizerToken(tok);
            }
            else if (_Mytoml_ParseNewline(tok))
            {
                _Mytoml_NextTokenizerToken(tok);
            }
            else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_ParseWhitespace(tok);
            }
            else if (_Mytoml_IsCommentStart(_Mytoml_TokenizerGetToken(tok)))
            {
                bool ok = _Mytoml_ParseComment(tok);
                RETURN_IF_FAILED(ok, "invalid comment\n");
            }
            else
            {
                RETURN_IF_FAILED(sep, "expected , between elements\n");
                TomlValue *v = _Mytoml_ParseValue(tok, "#,] \n");
                RETURN_IF_FAILED(v, "could not parse value\n");
                arr->arr[arr->len++] = v;
                sep = false;
            }
        }
        return NULL;
    }

    double
    _Mytoml_ParseBoolean(Tokenizer *tok)
    {
        double ret = 2.0;
        if (_Mytoml_TokenizerGetToken(tok) == 't')
        {
            _Mytoml_NextTokenizerToken(tok);
            _Mytoml_NextTokenizerToken(tok);
            _Mytoml_NextTokenizerToken(tok);
            if (_Mytoml_TokenizerGetPrevPrev(tok) != 'r' ||
                _Mytoml_TokenizerGetPrev(tok) != 'u' ||
                _Mytoml_TokenizerGetToken(tok) != 'e')
            {
                ret = 2.0;
            }
            else
            {
                ret = 1.0;
            }
        }
        else if (_Mytoml_TokenizerGetToken(tok) == 'f')
        {
            _Mytoml_NextTokenizerToken(tok);
            _Mytoml_NextTokenizerToken(tok);
            _Mytoml_NextTokenizerToken(tok);
            if (_Mytoml_TokenizerGetPrevPrev(tok) != 'a' ||
                _Mytoml_TokenizerGetPrev(tok) != 'l' ||
                _Mytoml_TokenizerGetToken(tok) != 's')
            {
                ret = 2.0;
            }
            else
            {
                _Mytoml_NextTokenizerToken(tok);
                if (_Mytoml_TokenizerGetToken(tok) == 'e')
                {
                    ret = 0.0;
                }
            }
        }
        _Mytoml_NextTokenizerToken(tok);
        return ret;
    }

    TomlKey *
    _Mytoml_ParseInlineTable(Tokenizer *tok)
    {
        TomlKey *keys = _Mytoml_NewKey(TOML_TABLE);
        bool sep = true;
        bool first = true;
        while (_Mytoml_TokenizerHasToken(tok))
        {
            if (_Mytoml_IsInlineTableEnd(_Mytoml_TokenizerGetToken(tok)))
            {
                FUNC_IF_FAILED((!sep || first), _Mytoml_DeleteKey, keys);
                RETURN_IF_FAILED((!sep || first), "cannot have trailing comma in inline table\n");
                _Mytoml_NextTokenizerToken(tok);
                return keys;
            }
            else if (_Mytoml_IsInlineTableSep(_Mytoml_TokenizerGetToken(tok)))
            {
                FUNC_IF_FAILED(!sep, _Mytoml_DeleteKey, keys);
                RETURN_IF_FAILED(!sep, "expected key-value but got , instead");
                sep = true;
                _Mytoml_NextTokenizerToken(tok);
            }
            else if (_Mytoml_ParseNewline(tok))
            {
                LOG_ERR("found newline in inline table\n");
                break;
            }
            else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_ParseWhitespace(tok);
            }
            else
            {
                FUNC_IF_FAILED(sep, _Mytoml_DeleteKey, keys);
                RETURN_IF_FAILED(sep, "expected , between elements\n");
                TomlKey *k = _Mytoml_ParseKey(tok, keys, true);
                FUNC_IF_FAILED(k, _Mytoml_DeleteKey, keys);
                RETURN_IF_FAILED(k, "failed to parse key\n");
                TomlValue *v = _Mytoml_ParseValue(tok, ", }");
                FUNC_IF_FAILED(v, _Mytoml_DeleteKey, keys);
                RETURN_IF_FAILED(v, "failed to parse value\n");
                // refer to inline table comment in `keys.c`
                if (v->type == TOML_INLINETABLE)
                {
                    TomlKey *h = (TomlKey *)(v->data);
                    k->type = TOML_KEY;
                    for (khiter_t ki = kh_begin(h->subkeys);
                         ki != kh_end(h->subkeys);
                         ++ki)
                    {
                        if (kh_exist(h->subkeys, ki))
                        {
                            TomlKey *e = _Mytoml_AddSubKey(k, kh_value(h->subkeys, ki));
                            FUNC_IF_FAILED(e, _Mytoml_DeleteKey, keys);
                            RETURN_IF_FAILED(e, "could not add inline table key %s\n", kh_value(h->subkeys, ki)->id);
                        }
                    }
                    k->type = TOML_KEYLEAF;
                }
                else
                {
                    k->value = v;
                }
                _Mytoml_ParseWhitespace(tok);
                sep = false;
                first = false;
            }
        }
        return NULL;
    }

    bool _Mytoml_ParseComment(Tokenizer *tok)
    {
        while (_Mytoml_TokenizerHasToken(tok))
        {
            _Mytoml_NextTokenizerToken(tok);
            if (_Mytoml_ParseNewline(tok))
            {
                _Mytoml_NextTokenizerToken(tok);
                return true;
            }
            if (_Mytoml_Iscontrol(_Mytoml_TokenizerGetToken(tok)))
            {
                return false;
            }
        }
        return true;
    }

    void _Mytoml_ParseWhitespace(Tokenizer *tok)
    {
        while (_Mytoml_TokenizerHasToken(tok))
        {
            if (!_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
            {
                break;
            }
            _Mytoml_NextTokenizerToken(tok);
        }
    }

    bool _Mytoml_ParseNewline(Tokenizer *tok)
    {
        if (_Mytoml_IsNewline(_Mytoml_TokenizerGetToken(tok)))
        {
            return true;
        }
        else if (_Mytoml_IsReturn(_Mytoml_TokenizerGetToken(tok)))
        {
            bool a = _Mytoml_NextTokenizerToken(tok);
            if (_Mytoml_IsNewline(_Mytoml_TokenizerGetToken(tok)))
            {
                return true;
            }
            else
            {
                _Mytoml_TokenizerBacktrace(tok, a);
            }
        }
        return false;
    }

    int _Mytoml_ParseUnicode(Tokenizer *tok, char *escaped, int len)
    {
        int digits = 0;
        char code[9] = {0};
        while (_Mytoml_TokenizerHasToken(tok))
        {
            if (digits > 8)
            {
                LOG_ERR("Invalid unicode escape code\n");
                break;
            }
            if (_Mytoml_IsHexDigit(_Mytoml_TokenizerGetToken(tok)) ||
                _Mytoml_IsDigit(_Mytoml_TokenizerGetToken(tok)))
            {
                code[digits++] = _Mytoml_TokenizerGetToken(tok);
                _Mytoml_NextTokenizerToken(tok);
                continue;
            }
            else
            {
                if (digits != 4 && digits != 8)
                {
                    LOG_ERR("Invalid unicode escape code\n");
                    break;
                }
                char *end;
                unsigned long num = strtoul(code, &end, 16);
                if (end != code + digits)
                {
                    LOG_ERR("Invalid unicode escape code\n");
                    break;
                }
                // Unicode Scalar Values: %x80-D7FF / %xE000-10FFFF
                if ((num >= 0x0 && num <= 0xD7FF) || (num >= 0xE000 && num <= 0x10FFFF))
                {
                    // UTF-8 encoding
                    if (num <= 0x0 && num <= 0x7F)
                    {
                        if (len < 1)
                        {
                            LOG_ERR("escaped array is not long enough\n");
                            break;
                        }
                        escaped[0] = (num) & 0b01111111;
                        return 1;
                    }
                    else if (num >= 0x80 && num <= 0x7FF)
                    {
                        if (len < 2)
                        {
                            LOG_ERR("escaped array is not long enough\n");
                            break;
                        }
                        escaped[0] = (0b11000000 | (num >> 6)) & 0b11011111;
                        escaped[1] = (0b10000000 | (num)) & 0b10111111;
                        return 2;
                    }
                    else if ((num >= 0x800 && num <= 0xFFFF))
                    {
                        if (len < 3)
                        {
                            LOG_ERR("escaped array is not long enough\n");
                            break;
                        }
                        escaped[0] = (0b11100000 | (num >> 12)) & 0b11101111;
                        escaped[1] = (0b10000000 | (num >> 6)) & 0b10111111;
                        escaped[2] = (0b10000000 | (num)) & 0b10111111;
                        return 3;
                    }
                    else
                    {
                        if (len < 4)
                        {
                            LOG_ERR("escaped array is not long enough\n");
                            break;
                        }
                        escaped[0] = (0b11110000 | (num >> 18)) & 0b11110111;
                        escaped[1] = (0b10000000 | (num >> 12)) & 0b10111111;
                        escaped[2] = (0b10000000 | (num >> 6)) & 0b10111111;
                        escaped[3] = (0b10000000 | (num)) & 0b10111111;
                        return 4;
                    }
                    return 0;
                }
                else
                {
                    LOG_ERR("Invalid unicode escape code\n");
                    break;
                }
            }
        }
        return 0;
    }

    int _Mytoml_ParseEscape(
        Tokenizer *tok,
        char *escaped,
        int len)
    {
        if (len < 1)
        {
            LOG_ERR("escaped array is not long enough\n");
            return 0;
        }
        switch (_Mytoml_TokenizerGetToken(tok))
        {
        case 'b':
        {
            escaped[0] = '\b';
            _Mytoml_NextTokenizerToken(tok);
            return 1;
        }
        case 't':
        {
            escaped[0] = '\t';
            _Mytoml_NextTokenizerToken(tok);
            return 1;
        }
        case 'n':
        {
            escaped[0] = '\n';
            _Mytoml_NextTokenizerToken(tok);
            return 1;
        }
        case 'f':
        {
            escaped[0] = '\f';
            _Mytoml_NextTokenizerToken(tok);
            return 1;
        }
        case 'r':
        {
            escaped[0] = '\r';
            _Mytoml_NextTokenizerToken(tok);
            return 1;
        }
        case '"':
        {
            escaped[0] = '\"';
            _Mytoml_NextTokenizerToken(tok);
            return 1;
        }
        case '\\':
        {
            escaped[0] = '\\';
            _Mytoml_NextTokenizerToken(tok);
            return 1;
        }
        case 'u':
        {
            _Mytoml_NextTokenizerToken(tok);
            int u = _Mytoml_ParseUnicode(tok, escaped, len);
            return u;
        }
        case 'U':
        {
            _Mytoml_NextTokenizerToken(tok);
            int u = _Mytoml_ParseUnicode(tok, escaped, len);
            return u;
        }
        default:
            return 0;
        }
        return 0;
    }

    double _Mytoml_ParseBaseUnit(Tokenizer *tok, int base, char *value, const char *num_end)
    {
        int idx = 0;
        double d = -1;
        while (_Mytoml_TokenizerHasToken(tok))
        {
            if (idx >= MYTOML_MAX_STRING_LENGTH)
            {
                LOG_ERR("buffer overflow\n");
                break;
            }
            if (_Mytoml_IsNumberEnd(_Mytoml_TokenizerGetToken(tok), num_end))
            {
                if (idx == 0)
                {
                    LOG_ERR("incomplete non-decimal number\n");
                    break;
                }
                char *end;
                unsigned long num = strtoul(value, &end, base);
                if (end == value + idx)
                {
                    d = (double)num;
                }
                else
                {
                    LOG_ERR("could not convert %s to base %d\n", value, base);
                }
                break;
            }
            else if (_Mytoml_IsUnderscore(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                if ((_Mytoml_IsDigit(_Mytoml_TokenizerGetToken(tok)) ||
                     (base == 16 && _Mytoml_IsHexDigit(_Mytoml_TokenizerGetToken(tok)))) &&
                    (_Mytoml_IsDigit(_Mytoml_TokenizerGetPrevPrev(tok)) ||
                     (base == 16 && _Mytoml_IsHexDigit(_Mytoml_TokenizerGetPrevPrev(tok)))))
                {
                    value[idx++] = _Mytoml_TokenizerGetToken(tok);
                }
                else
                {
                    LOG_ERR("stray %c character\n", _Mytoml_TokenizerGetPrev(tok));
                    break;
                }
            }
            else
            {
                value[idx++] = _Mytoml_TokenizerGetToken(tok);
            }
            _Mytoml_NextTokenizerToken(tok);
        }
        return d;
    }

    Number *
    _Mytoml_ParseNumber(
        Tokenizer *tok,
        char *value,
        double *d,
        const char *num_end,
        Number *n)
    {
        int idx = 0;
        n->type = TOML_INT;
        n->scientific = false;
        n->precision = 0;
        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(idx < MYTOML_MAX_STRING_LENGTH, "buffer overflow\n");
            if (_Mytoml_IsNumberEnd(_Mytoml_TokenizerGetToken(tok), num_end))
            {
                char *end;
                double num = strtod(value, &end);
                RETURN_IF_FAILED(end == value + idx, "could not convert %s to double\n", value);
                *d = num;
                if (n->precision > 0)
                    n->precision--;
                if (n->type == TOML_INT && num != 0)
                {
                    RETURN_IF_FAILED(value[0] != '0', "cannot have leading zero for integers");
                    if (value[0] == '+' || value[0] == '-')
                    {
                        RETURN_IF_FAILED(value[1] != '0', "cannot have leading zero for signed integers");
                    }
                }
                return n;
            }
            else if (idx == 0 && _Mytoml_TokenizerGetToken(tok) == '0')
            {
                double b;
                _Mytoml_NextTokenizerToken(tok);
                if (_Mytoml_TokenizerGetToken(tok) == 'x')
                {
                    // hexadecimal
                    _Mytoml_NextTokenizerToken(tok);
                    b = _Mytoml_ParseBaseUnit(tok, 16, value, num_end);
                }
                else if (_Mytoml_TokenizerGetToken(tok) == 'o')
                {
                    // octal
                    _Mytoml_NextTokenizerToken(tok);
                    b = _Mytoml_ParseBaseUnit(tok, 8, value, num_end);
                }
                else if (_Mytoml_TokenizerGetToken(tok) == 'b')
                {
                    // binary
                    _Mytoml_NextTokenizerToken(tok);
                    b = _Mytoml_ParseBaseUnit(tok, 2, value, num_end);
                }
                else
                {
                    value[idx++] = '0';
                    continue;
                }
                RETURN_IF_FAILED(b != -1, "invalid non-decimal number\n");
                *d = b;
                return n;
            }
            else if (_Mytoml_IsDecimalPoint(_Mytoml_TokenizerGetToken(tok)) ||
                     _Mytoml_IsUnderscore(_Mytoml_TokenizerGetToken(tok)))
            {
                if (_Mytoml_IsDecimalPoint(_Mytoml_TokenizerGetToken(tok)))
                {
                    value[idx++] = _Mytoml_TokenizerGetToken(tok);
                    n->type = TOML_FLOAT;
                    n->precision = 1;
                }
                RETURN_IF_FAILED(idx < MYTOML_MAX_STRING_LENGTH, "buffer overflow\n");
                _Mytoml_NextTokenizerToken(tok);
                if (_Mytoml_IsDigit(_Mytoml_TokenizerGetToken(tok)) &&
                    _Mytoml_IsDigit(_Mytoml_TokenizerGetPrevPrev(tok)))
                {
                    if (n->precision > 0)
                        n->precision++;
                    value[idx++] = _Mytoml_TokenizerGetToken(tok);
                }
                else
                {
                    LOG_ERR("stray %c character\n", _Mytoml_TokenizerGetPrev(tok));
                    return NULL;
                }
            }
            else if (_Mytoml_TokenizerGetToken(tok) == 'i' || _Mytoml_TokenizerGetToken(tok) == 'n')
            {
                if (idx == 1 && (_Mytoml_TokenizerGetPrev(tok) == '+' || _Mytoml_TokenizerGetPrev(tok) == '-'))
                {
                    double f = _Mytoml_ParseInfNan(tok, (_Mytoml_TokenizerGetPrev(tok) == '-'));
                    if (f == 0)
                        break;
                    *d = f;
                    n->type = TOML_FLOAT;
                    n->precision = 0;
                    return n;
                }
                else
                {
                    LOG_ERR("unknown or invalid number\n");
                    return NULL;
                }
            }
            else if (_Mytoml_TokenizerGetToken(tok) == 'x' || _Mytoml_TokenizerGetToken(tok) == 'X' ||
                     _Mytoml_TokenizerGetToken(tok) == 'b' || _Mytoml_TokenizerGetToken(tok) == 'B' ||
                     _Mytoml_TokenizerGetToken(tok) == 'o' || _Mytoml_TokenizerGetToken(tok) == 'O')
            {
                LOG_ERR("invalid decimal number, found stray character %c\n", _Mytoml_TokenizerGetToken(tok));
                break;
            }
            else
            {
                value[idx++] = _Mytoml_TokenizerGetToken(tok);
                if (n->precision > 0)
                    n->precision++;
                if (_Mytoml_TokenizerGetToken(tok) == 'e' || _Mytoml_TokenizerGetToken(tok) == 'E')
                {
                    n->type = TOML_FLOAT;
                    n->scientific = true;
                }
            }
            _Mytoml_NextTokenizerToken(tok);
        }
        char *end;
        double num = strtod(value, &end);
        RETURN_IF_FAILED(end == value + idx, "could not convert %s to double\n", value);
        *d = num;
        if (n->precision > 0)
            n->precision--;
        if (n->type == TOML_INT && num != 0)
        {
            RETURN_IF_FAILED(value[0] != '0', "cannot have leading zero for integers");
            if (value[0] == '+' || value[0] == '-')
            {
                RETURN_IF_FAILED(value[1] != '0', "cannot have leading zero for signed integers");
            }
        }
        return n;
    }

    TomlValue *
    _Mytoml_ParseValue(
        Tokenizer *tok,
        const char *num_end)
    {
        while (_Mytoml_TokenizerHasToken(tok))
        {
            RETURN_IF_FAILED(!_Mytoml_ParseNewline(tok), "got a newline before any value\n");
            if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_ParseWhitespace(tok);
                continue;
            }
            else if (_Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                char value[MYTOML_MAX_STRING_LENGTH] = {0};
                char *s;
                _Mytoml_NextTokenizerToken(tok);
                if (_Mytoml_TokenizerHasToken(tok) && _Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
                {
                    _Mytoml_NextTokenizerToken(tok);
                    if (_Mytoml_TokenizerHasToken(tok) && _Mytoml_IsBasicStringStart(_Mytoml_TokenizerGetToken(tok)))
                    {
                        _Mytoml_NextTokenizerToken(tok);
                        s = _Mytoml_ParseBasicString(tok, value, true);
                    }
                    else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)) || _Mytoml_ParseNewline(tok))
                    {
                        s = value;
                    }
                    else
                    {
                        LOG_ERR("cannot start string with 2 double-quotes\n");
                        break;
                    }
                }
                else
                {
                    s = _Mytoml_ParseBasicString(tok, value, false);
                }
                RETURN_IF_FAILED(s, "could not parse basic string\n");
                TomlValue *v = _Mytoml_NewStringValue(value);
                return v;
            }
            else if (_Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
            {
                char value[MYTOML_MAX_STRING_LENGTH] = {0};
                char *s;
                _Mytoml_NextTokenizerToken(tok);
                if (_Mytoml_TokenizerHasToken(tok) && _Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
                {
                    _Mytoml_NextTokenizerToken(tok);
                    if (_Mytoml_TokenizerHasToken(tok) && _Mytoml_IsLiteralStringStart(_Mytoml_TokenizerGetToken(tok)))
                    {
                        _Mytoml_NextTokenizerToken(tok);
                        s = _Mytoml_ParseLiteralString(tok, value, true);
                    }
                    else if (_Mytoml_IsWhitespace(_Mytoml_TokenizerGetToken(tok)) || _Mytoml_ParseNewline(tok))
                    {
                        s = value;
                    }
                    else
                    {
                        LOG_ERR("cannot start string with 2 single-quotes\n");
                        break;
                    }
                }
                else
                {
                    s = _Mytoml_ParseLiteralString(tok, value, false);
                }
                RETURN_IF_FAILED(s, "could not parse literal string\n");
                TomlValue *v = _Mytoml_NewStringValue(value);
                return v;
            }
            else if (_Mytoml_IsNumberStart(_Mytoml_TokenizerGetToken(tok)))
            {
                char value[MYTOML_MAX_STRING_LENGTH] = {0};
                // try parsing date time
                bool a = _Mytoml_NextTokenizerToken(tok);
                bool b = _Mytoml_NextTokenizerToken(tok);
                if (_Mytoml_TokenizerHasToken(tok) && _Mytoml_TokenizerGetToken(tok) == ':')
                {
                    _Mytoml_TokenizerBacktrace(tok, a + b);
                    struct tm *time = (struct tm *)calloc(1, sizeof(struct tm));
                    Datetime *dt = _Mytoml_ParseDatetime(tok, value, num_end, time);
                    FUNC_IF_FAILED(dt, free, time);
                    RETURN_IF_FAILED(dt, "could not parse time\n");
                    TomlValue *v = _Mytoml_NewDatetimeValue(dt->dt, dt->type, dt->format, dt->millis);
                    free(dt);
                    free(time);
                    return v;
                }
                else if (!_Mytoml_IsDigit(_Mytoml_TokenizerGetPrev(tok)) || !_Mytoml_IsDigit(_Mytoml_TokenizerGetToken(tok)))
                {
                    _Mytoml_TokenizerBacktrace(tok, a + b);
                }
                else
                {
                    bool c = _Mytoml_NextTokenizerToken(tok);
                    bool d = _Mytoml_NextTokenizerToken(tok);
                    if (_Mytoml_TokenizerHasToken(tok) && _Mytoml_TokenizerGetToken(tok) == '-')
                    {
                        _Mytoml_TokenizerBacktrace(tok, a + b + c + d);
                        struct tm *time = (struct tm *)calloc(1, sizeof(struct tm));
                        Datetime *dt = _Mytoml_ParseDatetime(tok, value, num_end, time);
                        FUNC_IF_FAILED(dt, free, time);
                        RETURN_IF_FAILED(dt, "could not parse datetime\n");
                        TomlValue *v = _Mytoml_NewDatetimeValue(dt->dt, dt->type, dt->format, dt->millis);
                        free(dt);
                        free(time);
                        return v;
                    }
                    else
                    {
                        _Mytoml_TokenizerBacktrace(tok, a + b + c + d);
                    }
                }
                double *d = (double *)calloc(1, sizeof(double));
                Number *num = (Number *)calloc(1, sizeof(Number));
                Number *n = _Mytoml_ParseNumber(tok, value, d, num_end, num);
                FUNC_IF_FAILED(n, free, d);
                FUNC_IF_FAILED(n, free, n);
                RETURN_IF_FAILED(n, "could not parse number\n");
                TomlValue *v = _Mytoml_NewNumberValue(d, n->type, n->precision, n->scientific);
                free(d);
                free(n);
                return v;
            }
            else if (_Mytoml_IsArrayStart(_Mytoml_TokenizerGetToken(tok)))
            {
                TomlValue *v = _Mytoml_NewArrayValue();
                _Mytoml_NextTokenizerToken(tok);
                TomlValue *val = _Mytoml_ParseArray(tok, v);
                FUNC_IF_FAILED(val, _Mytoml_FreeValue, v);
                RETURN_IF_FAILED(val, "could not parse array\n");
                return val;
            }
            else if (_Mytoml_IsInlineTableStart(_Mytoml_TokenizerGetToken(tok)))
            {
                _Mytoml_NextTokenizerToken(tok);
                TomlKey *keys = _Mytoml_ParseInlineTable(tok);
                RETURN_IF_FAILED(keys, "could not parse inline table\n");
                TomlValue *v = _Mytoml_NewTableValue(keys);
                return v;
            }
            else if (_Mytoml_TokenizerGetToken(tok) == 't' || _Mytoml_TokenizerGetToken(tok) == 'f')
            {
                double b = _Mytoml_ParseBoolean(tok);
                RETURN_IF_FAILED((b == 1 || b == 0), "expecting true or false but could not parse\n");
                TomlValue *v = _Mytoml_NewNumberValue(&b, TOML_BOOL, 0, false);
                return v;
            }
            else if (_Mytoml_TokenizerGetToken(tok) == 'i' || _Mytoml_TokenizerGetToken(tok) == 'n')
            {
                double f = _Mytoml_ParseInfNan(tok, false);
                RETURN_IF_FAILED(f, "expecting inf or nan but could not parse\n");
                TomlValue *v = _Mytoml_NewNumberValue(&f, TOML_FLOAT, 0, false);
                return v;
            }
            else
            {
                LOG_ERR("unknown value type\n");
                break;
            }
        }
        return NULL;
    }

#ifdef __cplusplus
}
#endif // __cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Only Classes
//-----------------------------------------------------------------------------

#ifdef __cplusplus

//-----------------------------------------------------------------------------
// [SECTION] Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] Definations
//-----------------------------------------------------------------------------

#endif //__cplusplus

#pragma endregion

#pragma region MyToml

//-----------------------------------------------------------------------------
// [SECTION] C Only Functions
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    MYTOML_API TomlKey *tomlLoadFile(char *file)
    {
        TomlKey *root = _Mytoml_NewKey(TOML_TABLE);
        memcpy(root->id, "root", strlen("root"));

        Input input = {.type = I_File, .file.name = file};
        Tokenizer *tok = _Mytoml_NewTokenizer(input);
        bool ok = _Mytoml_TokenizerLoadInput(tok);
        RETURN_IF_FAILED(ok, "Failed to load input from %s\n", input.file.name);
        _Mytoml_NextTokenizerToken(tok);

        int line, col;
        TomlKey *key = root;
        while (_Mytoml_TokenizerHasToken(tok) != 0)
        {
            key = _Mytoml_ParseKeyValue(tok, key, root);
            line = tok->line;
            col = tok->col;
            FUNC_IF_FAILED(key, _Mytoml_FreeTokenizer, tok);
            FUNC_IF_FAILED(key, tomlFree, root);
            RETURN_IF_FAILED(key, "Encountered an error while parsing %s\n"
                                  "At line %d column %d\n",
                             file, line + 1, col);
        }

        _Mytoml_FreeTokenizer(tok);
        return root;
    };

    MYTOML_API TomlKey *tomlLoadFILE(FILE *file)
    {
        TomlKey *root = _Mytoml_NewKey(TOML_TABLE);
        memcpy(root->id, "root", strlen("root"));

        Input input = {.type = I_File, .file.pointer = file};
        Tokenizer *tok = _Mytoml_NewTokenizer(input);
        bool ok = _Mytoml_TokenizerLoadInput(tok);
        RETURN_IF_FAILED(ok, "Failed to load input from %s\n", "FILE");
        _Mytoml_NextTokenizerToken(tok);

        int line, col;
        TomlKey *key = root;
        while (_Mytoml_TokenizerHasToken(tok) != 0)
        {
            key = _Mytoml_ParseKeyValue(tok, key, root);
            line = tok->line;
            col = tok->col;
            FUNC_IF_FAILED(key, _Mytoml_FreeTokenizer, tok);
            FUNC_IF_FAILED(key, tomlFree, root);
            RETURN_IF_FAILED(key, "Encountered an error while parsing %s\n"
                                  "At line %d column %d\n",
                             "FILE", line + 1, col);
        }

        _Mytoml_FreeTokenizer(tok);
        return root;
    };

    MYTOML_API TomlKey *tomlLoads(const char *toml)
    {
        TomlKey *root = _Mytoml_NewKey(TOML_TABLE);
        memcpy(root->id, "root", strlen("root"));

        Input input = {.type = I_STREAM, .stream = strdup(toml)};
        Tokenizer *tok = _Mytoml_NewTokenizer(input);
        _Mytoml_NextTokenizerToken(tok);

        int line, col;
        TomlKey *key = root;
        while (_Mytoml_TokenizerHasToken(tok) != 0)
        {
            key = _Mytoml_ParseKeyValue(tok, key, root);
            line = tok->line;
            col = tok->col;
            FUNC_IF_FAILED(key, _Mytoml_FreeTokenizer, tok);
            FUNC_IF_FAILED(key, tomlFree, root);
            RETURN_IF_FAILED(key, "Encountered an error while parsing %s\n"
                                  "At line %d column %d\n",
                             "FILE", line + 1, col);
        }

        _Mytoml_FreeTokenizer(tok);
        return root;
    };

    MYTOML_API void tomlKeyDumpFILE(TomlKey *object, FILE *file)
    {
        fprintf(file, "%s", tomlKeyDumps(object));
    };

    MYTOML_API void tomlKeyDumpFile(TomlKey *object, const char *file)
    {
        FILE *stream;
        stream = fopen(file, "w");
        const char *buffer = tomlKeyDumps(object);
        fwrite(buffer, strlen(buffer), 1, stream);
    };

    MYTOML_API void tomlValueDumpFILE(TomlValue *object, FILE *file)
    {
        fprintf(file, "%s", tomlValueDumps(object));
    };

    MYTOML_API void tomlValueDumpFile(TomlValue *object, const char *file)
    {
        FILE *stream;
        stream = fopen(file, "w");
        const char *buffer = tomlValueDumps(object);
        fwrite(buffer, strlen(buffer), 1, stream);
    };

    MYTOML_API const char *tomlKeyDumps(TomlKey *k)
    {
        char *buffer = "";
        size_t size = 0;
        tomlKeyDumpBuffer(k, &buffer, &size);
        return buffer;
    };

    MYTOML_API const char *tomlValueDumps(TomlValue *v)
    {
        char *buffer = "";
        size_t size = 0;
        tomlValueDumpBuffer(v, &buffer, &size);
        return buffer;
    };

    MYTOML_API void tomlKeyDumpBuffer(TomlKey *k, char **buffer, size_t *size)
    {
        if (k->type == TOML_KEYLEAF &&
            k->value != NULL &&
            k->value->type != TOML_INLINETABLE)
        {
            _Mytoml_AppendToBuffer(buffer, size, "\"");
            _Mytoml_StringDump(k->id, buffer, size);
            _Mytoml_AppendToBuffer(buffer, size, "\": ");
            tomlValueDumpBuffer(k->value, buffer, size);
        }
        else if (k->type == TOML_ARRAYTABLE)
        {
            _Mytoml_AppendToBuffer(buffer, size, "\"");
            _Mytoml_StringDump(k->id, buffer, size);
            _Mytoml_AppendToBuffer(buffer, size, "\": [\n");
            for (size_t i = 0; i <= k->idx; i++)
            {
                tomlValueDumpBuffer(k->value->arr[i], buffer, size);
                if (i != k->idx)
                {
                    _Mytoml_AppendToBuffer(buffer, size, ",\n");
                }
            }
            _Mytoml_AppendToBuffer(buffer, size, "\n]");
        }
        else
        {
            _Mytoml_AppendToBuffer(buffer, size, "\"");
            _Mytoml_StringDump(k->id, buffer, size);

            _Mytoml_AppendToBuffer(buffer, size, "\": {\n");
            int total = kh_size(k->subkeys);
            for (khiter_t ki = kh_begin(k->subkeys);
                 ki != kh_end(k->subkeys);
                 ++ki)
            {
                if (kh_exist(k->subkeys, ki))
                {
                    tomlKeyDumpBuffer(kh_value(k->subkeys, ki), buffer, size);
                    if (--total > 0)
                    {
                        _Mytoml_AppendToBuffer(buffer, size, ",\n");
                    }
                }
            }
            _Mytoml_AppendToBuffer(buffer, size, "\n}");
        }
    }

    MYTOML_API void tomlValueDumpBuffer(TomlValue *v, char **buffer, size_t *size)
    {
        switch (v->type)
        {
        case TOML_STRING:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\"type\": \"string\", \"value\": ");
            _Mytoml_StringDump((char *)v->data, buffer, size);
            _Mytoml_AppendToBuffer(buffer, size, "\"}");
            break;
        }
        case TOML_FLOAT:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\"type\": \"float\", \"value\": ");
            double f = *(double *)(v->data);
            if (f == (double)INFINITY)
            {
                _Mytoml_AppendToBuffer(buffer, size, "\"}");
                _Mytoml_AppendToBuffer(buffer, size, "\"inf\"}");
            }
            else if (f == (double)-INFINITY)
            {
                _Mytoml_AppendToBuffer(buffer, size, "\"-inf\"}");
            }
            else if (isnan(f))
            {
                _Mytoml_AppendToBuffer(buffer, size, "\"nan\"}");
            }
            else if (v->scientific)
            {
                _Mytoml_AppendToBuffer(buffer, size, "\"%g\"}", f);
            }
            else if (f == 0.0)
            {
                _Mytoml_AppendToBuffer(buffer, size, "\"0.0\"}");
            }
            else
            {
                _Mytoml_AppendToBuffer(buffer, size, "\"%.*lf\"}", (int)v->precision, f);
            }
            break;
        }
        case TOML_INT:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\"type\": \"integer\", \"value\": ");
            _Mytoml_AppendToBuffer(buffer, size, "\"%.0lf\"}", *(double *)(v->data));
            break;
        }
        case TOML_BOOL:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\"type\": \"bool\", \"value\": ");
            if (*(double *)(v->data))
            {
                _Mytoml_AppendToBuffer(buffer, size, "\"true\"}");
            }
            else
            {
                _Mytoml_AppendToBuffer(buffer, size, "\"false\"}");
            }
            break;
        }
        case TOML_DATETIME:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\"type\": \"datetime\", \"value\": ");
            char buf[255] = {0};
            strftime(buf, sizeof(buf), v->format,
                     (struct tm *)v->data);
            _Mytoml_AppendToBuffer(buffer, size, "\"%s\"}", buf);
            break;
        }
        case TOML_DATETIMELOCAL:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\"type\": \"datetime-local\", \"value\": ");
            char buf[255] = {0};
            strftime(buf, sizeof(buf), v->format,
                     (struct tm *)v->data);
            _Mytoml_AppendToBuffer(buffer, size, "\"%s\"}", buf);
            break;
        }
        case TOML_DATELOCAL:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\"type\": \"date-local\", \"value\": ");
            char buf[255] = {0};
            strftime(buf, sizeof(buf), v->format,
                     (struct tm *)v->data);
            _Mytoml_AppendToBuffer(buffer, size, "\"%s\"}", buf);
            break;
        }
        case TOML_TIMELOCAL:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\"type\": \"time-local\", \"value\": ");
            char buf[255] = {0};
            strftime(buf, sizeof(buf), v->format,
                     (struct tm *)v->data);
            _Mytoml_AppendToBuffer(buffer, size, "\"%s\"}", buf);
            break;
        }
        case TOML_ARRAY:
        {
            _Mytoml_AppendToBuffer(buffer, size, "[\n");
            for (TomlValue **iter = v->arr; *iter != NULL; iter++)
            {
                tomlValueDumpBuffer(*iter, buffer, size);
                if (*(iter + 1) != NULL)
                {
                    _Mytoml_AppendToBuffer(buffer, size, ",\n");
                }
            }
            _Mytoml_AppendToBuffer(buffer, size, "\n]");
            break;
        }
        case TOML_INLINETABLE:
        {
            _Mytoml_AppendToBuffer(buffer, size, "{\n");
            TomlKey *k = (TomlKey *)(v->data);
            int total = kh_size(k->subkeys);
            for (khiter_t ki = kh_begin(k->subkeys);
                 ki != kh_end(k->subkeys);
                 ++ki)
            {
                if (kh_exist(k->subkeys, ki))
                {
                    tomlKeyDumpBuffer(kh_value(k->subkeys, ki), buffer, size);
                    if (--total > 0)
                    {
                        _Mytoml_AppendToBuffer(buffer, size, ",\n");
                    }
                }
            }
            _Mytoml_AppendToBuffer(buffer, size, "\n}");
            break;
        }
        default:
            printf("\nprint_value: unknown value type\n");
            break;
        }
    }

    MYTOML_API void toml_json_dump(TomlKey *root)
    {
        printf("{\n");
        int total = kh_size(root->subkeys);
        for (khiter_t ki = kh_begin(root->subkeys);
             ki != kh_end(root->subkeys);
             ki++)
        {
            if (kh_exist(root->subkeys, ki))
            {
                // tomlKeyDumpBuffer(kh_value(root->subkeys, ki),buffer,size);
                if (--total > 0)
                {
                    printf(",\n");
                }
            }
        }
        printf("\n}\n");
    }

    MYTOML_API void tomlFree(TomlKey *toml)
    {
        _Mytoml_DeleteKey(toml);
    }

    MYTOML_API int *tomlGetInt(TomlKey *key)
    {
        if (!key)
            return NULL;
        if (!(key->value))
            return NULL;
        if (!(key->value->type == TOML_INT))
            return NULL;
        if (!(key->value->data))
            return NULL;
        return (int *)(key->value->data);
    }

    MYTOML_API bool *tomlGetBool(TomlKey *key)
    {
        if (!key)
            return NULL;
        if (!(key->value))
            return NULL;
        if (!(key->value->type == TOML_BOOL))
            return NULL;
        if (!(key->value->data))
            return NULL;
        return (bool *)(key->value->data);
    }

    MYTOML_API char *tomlGetString(TomlKey *key)
    {
        if (!key)
            return NULL;
        if (!(key->value))
            return NULL;
        if (!(key->value->type == TOML_STRING))
            return NULL;
        if (!(key->value->data))
            return NULL;
        return (char *)(key->value->data);
    }

    MYTOML_API double *tomlGetFloat(TomlKey *key)
    {
        if (!key)
            return NULL;
        if (!(key->value))
            return NULL;
        if (!(key->value->type == TOML_FLOAT))
            return NULL;
        if (!(key->value->data))
            return NULL;
        return (double *)(key->value->data);
    }

    MYTOML_API TomlValue *tomlGetArray(TomlKey *key)
    {
        if (!key)
            return NULL;
        if (!(key->value))
            return NULL;
        if (!(key->value->type == TOML_ARRAY))
            return NULL;
        return key->value;
    }

    MYTOML_API struct tm *tomlGetDatetime(TomlKey *key)
    {
        if (!key)
            return NULL;
        if (!(key->value))
            return NULL;
        if (!(key->value->type == TOML_DATETIME ||
              key->value->type == TOML_DATETIMELOCAL ||
              key->value->type == TOML_DATELOCAL ||
              key->value->type == TOML_TIMELOCAL))
            return NULL;
        if (!(key->value->data))
            return NULL;
        return (struct tm *)(key->value->data);
    }

    MYTOML_API TomlKey *tomlGetKey(TomlKey *key, const char *id)
    {
        if (key == NULL)
        {
            return NULL;
        }
        if (strcmp(key->id, id) == 0)
        {
            return key;
        }
        khiter_t k = kh_get(str, key->subkeys, id);
        if (k != kh_end(key->subkeys))
        {
            return kh_value(key->subkeys, k);
        }
        LOG_ERR("node %s does not exist in subkeys of node %s",
                id, key->id);
        return NULL;
    }

#ifdef __cplusplus
}
#endif // __cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Only Classes
//-----------------------------------------------------------------------------

#ifdef __cplusplus

TomlError::TomlError(TomlError_t error)
{
    m_Error = {.type = error.type, .message = strdup(error.message), .line = error.line, .column = error.column};
};

const char *TomlError::what() const noexcept
{
    return m_Error.message;
};

TomlErrorType TomlError::type() const noexcept
{
    return m_Error.type;
};

#endif //__cplusplus

#pragma endregion
