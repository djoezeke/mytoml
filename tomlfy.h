#ifndef DJOEZEKE_TOMLFY_H
#define DJOEZEKE_TOMLFY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define TOMLFY_VERSION_MAJOR 0
#define TOMLFY_VERSION_MINOR 1
#define TOMLFY_VERSION_PATCH 0

#define TOMLFY_VERSION v##TOMLFY_VERSION_MAJOR##.##TOMLFY_VERSION_MINOR##.##TOMLFY_VERSION_PATCH

#define TOMLFY_CONCAT(x, y) x##y
#define TOMLFY_UNIQUE_NAME(prefix) TOMLFY_CONCAT(prefix, __COUNTER__)

#define TOMLFY_DEBUG

#ifdef TOMLFY_DEBUG
#endif // TOMLFY_DEBUG

/**
 * @brief Macro to convert an error value to its string representation.
 *
 * This macro is used in a switch-case statement to return the string representation
 * of an error value.
 *
 * @param value The error value to convert to a string.
 */
#define CASEFY_ERROR(value) \
    case value:             \
        return #value;

typedef struct Toml_t Toml_t;

#pragma region STRUCTURES

enum TomlError
{
    UNKNOWN = -1,

    NO_SEPARATOR,
    MISSING_VALUE,

    EMPTY_FIELD,

    INVALID_FIELD, // if filed doesnt exits
    INVALID_ROW,

    INVALID_FILE, // if
    NULL_FILE,    // if pointer is null
    FILE_ERROR,

    VALUE_NULL,
    WRONG_CAST
};

// Define TOML value types
typedef enum
{
    TOML_STRING,
    TOML_INTEGER,
    TOML_FLOAT,
    TOML_BOOLEAN,
    TOML_ARRAY,
    TOML_TABLE
} TomlType;

// Forward declaration of Toml_t

// Define a TOML key-value pair for tables
typedef struct
{
    char *key;
    Toml_t *value;
} TomlKeyValuePair;

// Define a TOML value
struct Toml_t
{
    TomlType type;
    union
    {
        char *string_value;
        int int_value;
        double float_value;
        int bool_value;
        struct
        {
            Toml_t **items;
            size_t size;
        } array_value;
        struct
        {
            TomlKeyValuePair **items;
            size_t size;
        } table_value;
    } data;
};

#pragma endregion // STRUCTURES

#pragma region DECLARATIONS

TomlKeyValuePair tomlfy_create_keyvalue(char *key, Toml_t *value);
void tomlfy_free_keyvalue(TomlKeyValuePair *keyvalue);

// Function to create a TOML string value
Toml_t tomlfy_create_string(const char *string_value);
char *tomlfy_get_string(Toml_t *value);
void tomlfy_free_string(Toml_t *value);

// Function to create a TOML integer value
Toml_t tomlfy_create_integer(int int_value);
int tomlfy_get_integer(Toml_t *value);
void tomlfy_free_integer(Toml_t *value);

// Function to create a TOML float value
Toml_t tomlfy_create_float(double float_value);
double tomlfy_get_float(Toml_t *value);
void tomlfy_free_float(Toml_t *value);

// Function to create a TOML boolean value
Toml_t tomlfy_create_boolean(int bool_value);
int tomlfy_get_boolean(Toml_t *value);
void tomlfy_free_boolean(Toml_t *value);

// Function to create a TOML array value
Toml_t tomlfy_create_array();
void tomlfy_free_array(Toml_t *value);
void tomlfy_array_add(Toml_t *table, Toml_t *value);
void tomlfy_array_del(Toml_t *table, int index);
Toml_t tomlfy_array_get(Toml_t *table, int index);

// Function to create a TOML table value
Toml_t tomlfy_create_table();
void tomlfy_free_table(Toml_t *value);
void tomlfy_table_add(Toml_t *table, const char *key, Toml_t *value);
void tomlfy_table_del(Toml_t *table, const char *key);
Toml_t tomlfy_table_get(Toml_t *table, const char *key);

// Function to print a TOML value (for demonstration purposes)
void tomlfy_print(Toml_t *value, int indent);

Toml_t tomlfy_read_from_file(const char *filename);
bool tomlfy_write_to_file(const Toml_t *toml, const char *filename);

// Function to free a TOML value
void tomlfy_free(Toml_t *value);

void tomlfy_error(enum TomlError error, const char *format, ...);
const char *tomlfy_error_name(enum TomlError error);

#pragma endregion // DECLARATIONS

#pragma region DEFINATIONS

TomlKeyValuePair tomlfy_create_keyvalue(char *key, Toml_t *value)
{
    TomlKeyValuePair keyvalue;
    keyvalue.key = strdup(key);
    keyvalue.value = value;
    return keyvalue;
};

void tomlfy_free_keyvalue(TomlKeyValuePair *keyvalue)
{
    if (keyvalue == NULL)
    {
#ifdef TOMLFY_DEBUG
        tomlfy_error(VALUE_NULL, "Can't free null key value");
#endif // TOMLFY_DEBUG
    }
    else
    {
        free(keyvalue->key);
        tomlfy_free(keyvalue->value);
    }
    return;
};

// Function to create a TOML string value
Toml_t tomlfy_create_string(const char *string_value)
{
    Toml_t value;
    value.type = TOML_STRING;
    value.data.string_value = strdup(string_value);
    return value;
}

// Function to create a TOML integer value
Toml_t tomlfy_create_integer(int int_value)
{
    Toml_t value;
    value.type = TOML_INTEGER;
    value.data.int_value = int_value;
    return value;
}

// Function to create a TOML float value
Toml_t tomlfy_create_float(double float_value)
{
    Toml_t value;
    value.type = TOML_FLOAT;
    value.data.float_value = float_value;
    return value;
}

// Function to create a TOML boolean value
Toml_t tomlfy_create_boolean(int bool_value)
{
    Toml_t value;
    value.type = TOML_BOOLEAN;
    value.data.bool_value = bool_value;
    return value;
}

// Function to create a TOML array value
Toml_t tomlfy_create_array()
{
    Toml_t value;
    value.type = TOML_ARRAY;
    value.data.array_value.items = NULL;
    value.data.array_value.size = 0;
    return value;
}

// Function to add an item to a TOML array
void tomlfy_array_add(Toml_t *array, Toml_t *item)
{
    if (array->type != TOML_ARRAY)
        return;
    array->data.array_value.items = (Toml_t **)realloc(array->data.array_value.items, (array->data.array_value.size + 1) * sizeof(Toml_t *));
    array->data.array_value.items[array->data.array_value.size] = item;
    array->data.array_value.size++;
}

// Function to create a TOML table value
Toml_t tomlfy_create_table()
{
    Toml_t value;
    value.type = TOML_TABLE;
    value.data.table_value.items = NULL;
    value.data.table_value.size = 0;
    return value;
}

// Function to add a key-value pair to a TOML table
void tomlfy_table_add(Toml_t *table, const char *key, Toml_t *value)
{
    if (table->type != TOML_TABLE)
        return;
    TomlKeyValuePair *pair = (TomlKeyValuePair *)malloc(sizeof(TomlKeyValuePair));
    pair->key = strdup(key);
    pair->value = value;
    table->data.table_value.items = (TomlKeyValuePair **)realloc(table->data.table_value.items, (table->data.table_value.size + 1) * sizeof(TomlKeyValuePair *));
    table->data.table_value.items[table->data.table_value.size] = pair;
    table->data.table_value.size++;
}

// Function to print a TOML value (for demonstration purposes)
void tomlfy_print(Toml_t *value, int indent)
{
    for (int i = 0; i < indent; i++)
        printf("  ");
    switch (value->type)
    {
    case TOML_STRING:
        printf("\"%s\"\n", value->data.string_value);
        break;
    case TOML_INTEGER:
        printf("%d\n", value->data.int_value);
        break;
    case TOML_FLOAT:
        printf("%f\n", value->data.float_value);
        break;
    case TOML_BOOLEAN:
        printf(value->data.bool_value ? "true\n" : "false\n");
        break;
    case TOML_ARRAY:
        printf("[\n");
        for (size_t i = 0; i < value->data.array_value.size; i++)
        {
            tomlfy_print(value->data.array_value.items[i], indent + 1);
        }
        for (int i = 0; i < indent; i++)
            printf("  ");
        printf("]\n");
        break;
    case TOML_TABLE:
        printf("{\n");
        for (size_t i = 0; i < value->data.table_value.size; i++)
        {
            for (int j = 0; j < indent + 1; j++)
                printf("  ");
            printf("%s = ", value->data.table_value.items[i]->key);
            tomlfy_print(value->data.table_value.items[i]->value, 0);
        }
        for (int i = 0; i < indent; i++)
            printf("  ");
        printf("}\n");
        break;
    }
}

// Function to free a TOML value
void tomlfy_free(Toml_t *value)
{
    if (!value)
        return;

    switch (value->type)
    {
    case TOML_FLOAT:
        tomlfy_free_float(value);
        break;
    case TOML_INTEGER:
        tomlfy_free_integer(value);
        break;
    case TOML_BOOLEAN:
        tomlfy_free_boolean(value);
        break;
    case TOML_STRING:
        tomlfy_free_string(value);
        break;
    case TOML_ARRAY:
        tomlfy_free_array(value);
        break;
    case TOML_TABLE:
        tomlfy_free_table(value);
        break;
    default:
        break;
    }
    free(value);
}

void tomlfy_free_string(Toml_t *value)
{
    if (value == NULL)
    {
#ifdef TOMLFY_DEBUG
        tomlfy_error(VALUE_NULL, "Can't free null value");
#endif // TOMLFY_DEBUG
    }
    else
        free(value->data.string_value);

    return;
};

void tomlfy_free_integer(Toml_t *value)
{
#ifdef TOMLFY_DEBUG

    if (value == NULL)
    {
        tomlfy_error(VALUE_NULL, "Can't free null value");
    }
#endif // TOMLFY_DEBUG
    return;
};

void tomlfy_free_float(Toml_t *value)
{
#ifdef TOMLFY_DEBUG

    if (value == NULL)
    {
        tomlfy_error(VALUE_NULL, "Can't free null value");
    }
#endif // TOMLFY_DEBUG
    return;
};

void tomlfy_free_boolean(Toml_t *value)
{
#ifdef TOMLFY_DEBUG

    if (value == NULL)
    {
        tomlfy_error(VALUE_NULL, "Can't free null value");
    }
#endif // TOMLFY_DEBUG
    return;
};

void tomlfy_free_array(Toml_t *value)
{
    if (value == NULL)
    {
#ifdef TOMLFY_DEBUG
        tomlfy_error(VALUE_NULL, "Can't free null value");
#endif // TOMLFY_DEBUG
    }
    else
    {
        for (size_t i = 0; i < value->data.array_value.size; i++)
        {
            free(value->data.array_value.items[i]);
        }
        free(value->data.array_value.items);
    }
    return;
};

void tomlfy_free_table(Toml_t *value)
{
    if (value == NULL)
    {
#ifdef TOMLFY_DEBUG
        tomlfy_error(VALUE_NULL, "Can't free null value");
#endif // TOMLFY_DEBUG
    }
    else
    {
        for (size_t i = 0; i < value->data.table_value.size; i++)
        {
            free(value->data.table_value.items[i]->key);
            tomlfy_free(value->data.table_value.items[i]->value);
            free(value->data.table_value.items[i]);
        }
        free(value->data.table_value.items);
    }
    return;
};

void tomlfy_error(enum TomlError error, const char *format, ...)
{

    if (format == NULL)
        fprintf(stderr, "TOMLFY [%i] : %s \n", error, tomlfy_error_name(error));
    else
    {
        va_list args;

        va_start(args, format);

        fprintf(stderr, "TOMLFY [%i] :", error);
        vprintf(format, args);
        // Clean up the argument list
        va_end(args);
    }
};

const char *tomlfy_error_name(enum TomlError error)
{
    switch (error)
    {
        CASEFY_ERROR(UNKNOWN);

        CASEFY_ERROR(NO_SEPARATOR);
        CASEFY_ERROR(MISSING_VALUE);

        CASEFY_ERROR(EMPTY_FIELD);

        CASEFY_ERROR(INVALID_FIELD);
        CASEFY_ERROR(INVALID_ROW);

        CASEFY_ERROR(INVALID_FILE);
        CASEFY_ERROR(NULL_FILE);
        CASEFY_ERROR(FILE_ERROR);

        CASEFY_ERROR(VALUE_NULL);

        CASEFY_ERROR(WRONG_CAST);
    }
};
#pragma endregion // DEFINATIONS

#pragma region // CPLUSPLUS

#ifdef __cplusplus

// Exeptions if C++

#include <fstream>
#include <iostream>
class Toml
{
private:
    Toml_t m_Toml_t;

public:
    Toml(char seperator) {
    };

    Toml() {
    };

    void loadFile(std::fstream file) {};
    void loadFileStr(std::string file_path) {};
    void loadString(std::string string) {};

    std::string Stringify()
    {
        return std::string("");
    };

    ~Toml()
    {
        tomlfy_free(&m_Toml_t);
    };
};

#endif // __cplusplus

#pragma endregion // CPLUSPLUS

#endif // DJOEZEKE_TOMLFY_H