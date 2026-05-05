/**
 * @file mytoml.cpp
 * @brief TOML For Modern C/C++.
 * @details All public API, types, macros, and configuration.
 * @author Sackey Ezekiel Etrue (djoezeke)
 * @version 0.1.0
 * @see https://www.github.com/djoezeke/mytoml
 * @copyright Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Developed by Sackey Ezekiel Etrue and every direct or indirect contributors to the GitHub.
 * See LICENSE for copyright and licensing details (standard MIT License).
 *
 * SECTIONS: Index of this file
 *
 *      Details: Details Namespace
 *
 *          [SECTION] Details : Encoding
 *          [SECTION] Details : Input
 *          [SECTION] Details : Output
 *
 *      Mytoml: Mytoml Namespace
 *
 *          [SECTION] Mytoml : Classes
 *          [SECTION] Mytoml : Functions
 *          [SECTION] Mytoml : Exceptions
 *
 *      Literals: Literals Namespace
 *
 *          [SECTION] Literals : Toml
 *
 * CONTRIBUTORS:
 *
 * TODO:
 *
 * HELP:
 *    - See links below.
 *    - Read top of mytoml.hpp for more details and comments.
 *
 *  Has only had a few tests run, may have issues.
 *
 *  If having issues compiling/linking/running raise an issue (https://github.com/djoezeke/mytoml/issues).
 *  Please post data https://github.com/djoezeke/mytoml/discussions if you cannot find a solution data resources above.
 *
 * RESOURCES:
 * - Homepage ................... https://github.com/djoezeke/mytoml
 * - Releases & changelog ....... https://github.com/djoezeke/mytoml/releases
 * - Issues & support ........... https://github.com/djoezeke/mytoml/issues
 *
 */

// NOLINTBEGIN

// clang-format off

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
#endif

// clang-format on

//-------------------------------------------------------------------------
// [SECTION] INCLUDES
//-------------------------------------------------------------------------

#define MYTOML_IMPLEMENTATION 1

#include <cctype>
#include <mytoml/mytoml.hpp>
#include <sstream>

#ifdef MYTOML_IMPLEMENTATION

// #include <MYTOML_ASSERT.h>
// #include <ctype.h>
// #include <limits.h>
#include <stdarg.h>
// #include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>  //
#include <stdlib.h> //
#include <string.h> //

//-------------------------------------------------------------------------
// [SECTION] Defines
//-----------------------------------------------------------------------------

// clang-format off

/** Byte order marks. */

#ifndef MYTOML_BOM_UTF8
    #define MYTOML_BOM_UTF8 "\xef\xbb\xbf"
#endif // MYTOML_BOM_UTF8

#ifndef MYTOML_BOM_UTF16LE
    #define MYTOML_BOM_UTF16LE "\xff\xfe"
#endif // MYTOML_BOM_UTF16LE

#ifndef MYTOML_BOM_UTF16BE
    #define MYTOML_BOM_UTF16BE "\xfe\xff"
#endif // MYTOML_BOM_UTF16BE

#ifndef MYTOML_BOM_UTF32LE
    #define MYTOML_BOM_UTF32LE "\xff\xfe\x00\x00"
#endif // MYTOML_BOM_UTF32LE

#ifndef MYTOML_BOM_UTF32BE
    #define MYTOML_BOM_UTF32BE "\x00\x00\xfe\xff"
#endif // MYTOML_BOM_UTF32BE

// clang-format on

#pragma region Detail

namespace mytoml
{
    MYTOML_VERSION_NAMESPACE_BEGIN

    namespace detail
    {
#ifndef MYTOML_NO_EXCEPTIONS

        //-----------------------------------------------------------------------------
        // [SECTION] Mytoml : Exceptions
        //-----------------------------------------------------------------------------
        // - exception()
        // - what()
        // - parse_error()
        // - generate()
        // - encoding_error()
        // - generate()
        //-----------------------------------------------------------------------------

        exception::exception(const char *message) noexcept
        {
            if (message != nullptr)
            {
                m_Message.append(message);
            }
        };

        const char *exception::what() const noexcept { return m_Message.c_str(); };

        parse_error::parse_error(const char *message) noexcept
            : exception(generate(message, {})) {};

        parse_error::parse_error(const char *message, detail::mark mark) noexcept
            : exception(generate(message, mark)) {};

        const char *parse_error::generate(const char *message, detail::mark mark) noexcept
        {
            // Use a thread_local buffer so the returned pointer remains valid
            // until the next call on the same thread. Callers should copy the
            // string if they need to keep it longer.
            thread_local std::string buffer;
            buffer.clear();

            if (message != nullptr)
            {
                buffer.append(message);
            }

            // Append positional information if available
            char tmp[128];
            int n = snprintf(tmp, sizeof(tmp), " (line: %zu, column: %zu, index: %zu)", mark.line, mark.column, mark.index);
            if (n > 0)
            {
                buffer.append(tmp, static_cast<size_t>(n));
            }

            // Ensure null-termination and return pointer
            return buffer.c_str();
        };

        encoding_error::encoding_error(const char *message) noexcept
            : exception(generate(encoding::unspecified, message, nullptr, 0)) {};

        encoding_error::encoding_error(encoding encoding_type, const char *message, void *data, size_t size) noexcept
            : exception(generate(encoding_type, message, data, size)) {};

        const char *encoding_error::generate(encoding enc, const char *message, void *data, size_t size) noexcept
        {
            thread_local std::string buffer;
            buffer.clear();

            if (message != nullptr)
            {
                buffer.append(message);
            }

            // Append encoding name
            const char *enc_name = "unspecified";
            switch (enc)
            {
            case encoding::utf8:
                enc_name = "utf-8";
                break;
            case encoding::utf16:
            case encoding::utf16le:
                enc_name = "utf-16-le";
                break;
            case encoding::utf16be:
                enc_name = "utf-16-be";
                break;
            case encoding::utf32:
            case encoding::utf32le:
                enc_name = "utf-32-le";
                break;
            case encoding::utf32be:
                enc_name = "utf-32-be";
                break;
            default:
                break;
            }

            char tmp[128];
            int n = snprintf(tmp, sizeof(tmp), " [encoding: %s]", enc_name);
            if (n > 0)
            {
                buffer.append(tmp, static_cast<size_t>(n));
            }

            // If data is provided, append a short hex preview (up to 8 bytes)
            if (data != nullptr && size > 0)
            {
                const unsigned char *b = reinterpret_cast<const unsigned char *>(data);
                size_t preview = (size < 8) ? size : 8; // limit
                buffer.append(" [data: 0x");
                for (size_t i = 0; i < preview; ++i)
                {
                    int written = snprintf(tmp, sizeof(tmp), "%02x", b[i]);
                    if (written > 0)
                    {
                        buffer.append(tmp, static_cast<size_t>(written));
                    }
                }
                if (size > preview)
                {
                    buffer.append("...");
                }
                buffer.append("]");
            }

            return buffer.c_str();
        };

#endif // MYTOML_NO_EXCEPTIONS

        //-------------------------------------------------------------------------
        // [SECTION] Details : Encoding
        //-------------------------------------------------------------------------

        encoding determine_encoding(void *data, size_t size)
        {
            if (size >= 4 && !memcmp(data, MYTOML_BOM_UTF32BE, 4))
            {
                return encoding::utf32be;
            }
            if (size >= 4 && !memcmp(data, MYTOML_BOM_UTF32LE, 4))
            {
                return encoding::utf32le;
            }
            if (size >= 3 && !memcmp(data, MYTOML_BOM_UTF8, 3))
            {
                return encoding::utf8;
            }
            if (size >= 2 && !memcmp(data, MYTOML_BOM_UTF16BE, 2))
            {
                return encoding::utf16be;
            }
            if (size >= 2 && !memcmp(data, MYTOML_BOM_UTF16LE, 2))
            {
                return encoding::utf16le;
            }
            return encoding::unspecified;
        };

        /*
         * Decode a UTF-8 character.  Check RFC 3629
         * (https://datatracker.ietf.org/doc/html/rfc3629) for more details.
         *
         * The following table (taken from the RFC) is used for
         * decoding.
         *
         * The table below summarizes the format of these different octet types.
         * The letter x indicates bits available for encoding bits of the
         * character number.

         *    Char. number range |        UTF-8 octet sequence
         *      (hexadecimal)    |              (binary)
         *   --------------------+------------------------------------
         *   0000 0000-0000 007F | 0xxxxxxx
         *   0000 0080-0000 07FF | 110xxxxx 10xxxxxx
         *   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
         *   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
         *
         * The definition of UTF-8 prohibits encoding character numbers between
         * U+D800 and U+DFFF, which are reserved for use with the UTF-16
         * encoding form (as surrogate pairs) and do not directly represent
         * characters. When encoding in UTF-8 from UTF-16 data, it is necessary
         * to first decode the UTF-16 data to obtain character numbers, which
         * are then encoded in UTF-8 as described above.
         *
         * (https://en.wikipedia.org/wiki/UTF-8)
         */

        int utf8::decode(const char *data, size_t size, unsigned int &value)
        {
            if (size == 0 || data == nullptr)
            {
                value = 0;
                return -1;
            }

            unsigned char byte = static_cast<unsigned char>(data[0]);
            if (byte < 0x80)
            {
                value = byte;
                return 1;
            }
            if ((byte & 0xE0) == 0xC0)
            {
                if (size < 2)
                {
                    value = 0;
                    return -1;
                }
                value = ((byte & 0x1F) << 6) | (static_cast<unsigned char>(data[1]) & 0x3F);
                return 2;
            }
            if ((byte & 0xF0) == 0xE0)
            {
                if (size < 3)
                {
                    value = 0;
                    return -1;
                }
                value = ((byte & 0x0F) << 12) | ((static_cast<unsigned char>(data[1]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(data[2]) & 0x3F);
                return 3;
            }
            if ((byte & 0xF8) == 0xF0)
            {
                if (size < 4)
                {
                    value = 0;
                    return -1;
                }
                value = ((byte & 0x07) << 18) | ((static_cast<unsigned char>(data[1]) & 0x3F) << 12) |
                        ((static_cast<unsigned char>(data[2]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(data[3]) & 0x3F);
                return 4;
            }
            value = 0;
            return -1;
        };

        int utf8::encode(unsigned int codepoint, utf8::char_t *output, size_t size)
        {
            if (size < 1)
            {
                return -1;
            }

            if (codepoint < 0x80)
            {
                output[0] = static_cast<char>(codepoint);
                return 1;
            }

            if (codepoint < 0x800)
            {
                if (size < 2)
                {
                    return -1;
                }
                output[0] = static_cast<char>((codepoint >> 6) | 0xC0);
                output[1] = static_cast<char>((codepoint & 0x3F) | 0x80);
                return 2;
            }

            if (codepoint < 0x10000)
            {
                if (size < 3)
                {
                    return -1;
                }
                output[0] = static_cast<char>((codepoint >> 12) | 0xE0);
                output[1] = static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
                output[2] = static_cast<char>((codepoint & 0x3F) | 0x80);
                return 3;
            }

            if (codepoint <= 0x10FFFF)
            {
                if (size < 4)
                {
                    return -1;
                }
                output[0] = static_cast<char>((codepoint >> 18) | 0xF0);
                output[1] = static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80);
                output[2] = static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
                output[3] = static_cast<char>((codepoint & 0x3F) | 0x80);
                return 4;
            }

            return -1;
        };

        std::vector<unsigned char> utf8::to_utf16(const std::string &string, mytoml_endian_value_t order)
        {
            std::vector<unsigned char> out;
            size_t idx = 0;
            while (idx < string.size())
            {
                unsigned int cp = 0;
                int dec = utf8::decode(string.data() + idx, string.size() - idx, cp);
                MYTOML_ASSERT(dec > 0);
                char tmp[4];
                int enc = utf16::encode(cp, reinterpret_cast<utf16::char_t *>(tmp), sizeof(tmp), order);
                MYTOML_ASSERT(enc > 0);
                for (int i = 0; i < enc * 2; ++i)
                    out.push_back(static_cast<unsigned char>(tmp[i]));
                idx += static_cast<size_t>(dec);
            }
            return out;
        };

        std::vector<unsigned char> utf8::to_utf32(const std::string &string, mytoml_endian_value_t order)
        {
            std::vector<unsigned char> out;
            size_t idx = 0;
            while (idx < string.size())
            {
                unsigned int cp = 0;
                int dec = utf8::decode(string.data() + idx, string.size() - idx, cp);
                MYTOML_ASSERT(dec > 0);
                char tmp[4];
                int enc = utf32::encode(cp, reinterpret_cast<utf32::char_t *>(tmp), sizeof(tmp), order);
                MYTOML_ASSERT(enc > 0);
                (void)enc;
                for (int i = 0; i < 4; ++i)
                    out.push_back(static_cast<unsigned char>(tmp[i]));
                idx += static_cast<size_t>(dec);
            }
            return out;
        };

        /*
         * Decode a UTF-16 character.  Check RFC 2781
         * (https://datatracker.ietf.org/doc/html/rfc2781).
         *
         * Normally, two subsequent bytes describe a Unicode
         * character.  However a special technique (called a
         * surrogate pair) is used for specifying character
         * values larger than 0xFFFF.
         *
         * A surrogate pair consists of two pseudo-characters:
         *      high surrogate area (0xD800-0xDBFF)
         *      low surrogate area (0xDC00-0xDFFF)
         *
         * The following formulas are used for decoding
         * and encoding characters using surrogate pairs:
         *
         *  U  = U' + 0x10000   (0x01 00 00 <= U <= 0x10 FF FF)
         *  U' = yyyyyyyyyyxxxxxxxxxx   (0 <= U' <= 0x0F FF FF)
         *  W1 = 110110yyyyyyyyyy
         *  W2 = 110111xxxxxxxxxx
         *
         * where U is the character value, W1 is the high surrogate
         * area, W2 is the low surrogate area.
         *
         * (https://en.wikipedia.org/wiki/UTF-16)
         *
         */

        int utf16::decode(const char *data, size_t size, unsigned int &value, mytoml_endian_value_t order)
        {
            if (size < 2 || data == nullptr)
            {
                value = 0;
                return -1;
            }

            const unsigned char *b = reinterpret_cast<const unsigned char *>(data);

            auto read_u16 = [&](size_t offset) -> uint16_t
            {
                if (order == mytoml_endian_t::native)
                {
                    uint16_t tmp = 0;
                    memcpy(&tmp, b + offset, 2);
                    return tmp;
                }
                else if (order == mytoml_endian_t::little)
                {
                    return static_cast<uint16_t>(b[offset] | (b[offset + 1] << 8));
                }
                else // big-endian
                {
                    return static_cast<uint16_t>((b[offset] << 8) | b[offset + 1]);
                }
            };

            uint16_t w1 = read_u16(0);

            // Check for surrogate pair (high surrogate 0xD800-0xDBFF)
            if (w1 >= 0xD800 && w1 <= 0xDBFF)
            {
                if (size < 4)
                {
                    value = 0;
                    return -1;
                }
                uint16_t w2 = read_u16(2);
                if (!(w2 >= 0xDC00 && w2 <= 0xDFFF))
                {
                    value = 0;
                    return -1;
                }
                unsigned int high = static_cast<unsigned int>(w1);
                unsigned int low = static_cast<unsigned int>(w2);
                value = ((high - 0xD800) << 10) + (low - 0xDC00) + 0x10000;
                return 4;
            }

            value = static_cast<unsigned int>(w1);
            return 2;
        };

        int utf16::encode(unsigned int codepoint, utf16::char_t *output, size_t size, mytoml_endian_value_t order)
        {
            if (size < 2 || output == nullptr)
            {
                return -1;
            }

            unsigned char *out = reinterpret_cast<unsigned char *>(output);

            auto write_u16 = [&](size_t offset, uint16_t val)
            {
                if (order == mytoml_endian_t::native)
                {
                    memcpy(out + offset, &val, 2);
                }
                else if (order == mytoml_endian_t::little)
                {
                    out[offset] = static_cast<unsigned char>(val & 0xFF);
                    out[offset + 1] = static_cast<unsigned char>((val >> 8) & 0xFF);
                }
                else // big-endian
                {
                    out[offset] = static_cast<unsigned char>((val >> 8) & 0xFF);
                    out[offset + 1] = static_cast<unsigned char>(val & 0xFF);
                }
            };

            if (codepoint < 0x10000)
            {
                uint16_t val = static_cast<uint16_t>(codepoint);
                write_u16(0, val);
                return 1;
            }

            if (codepoint > 0x10FFFF)
            {
                return -1;
            }

            if (size < 4)
            {
                return -1;
            }

            unsigned int cp = codepoint - 0x10000;
            uint16_t w1 = static_cast<uint16_t>((cp >> 10) + 0xD800);
            uint16_t w2 = static_cast<uint16_t>((cp & 0x3FF) + 0xDC00);
            write_u16(0, w1);
            write_u16(2, w2);
            return 2;
        };

        std::string utf16::to_utf8(const std::vector<unsigned char> &bytes, mytoml_endian_value_t order)
        {
            std::string out;
            size_t idx = 0;
            while (idx < bytes.size())
            {
                unsigned int cp = 0;
                int dec = utf16::decode(reinterpret_cast<const char *>(bytes.data() + idx), bytes.size() - idx, cp, order);
                MYTOML_ASSERT(dec > 0);
                char tmp[4];
                int enc = utf8::encode(cp, reinterpret_cast<utf8::char_t *>(tmp), sizeof(tmp));
                MYTOML_ASSERT(enc > 0);
                out.append(tmp, tmp + enc);
                idx += static_cast<size_t>(dec);
            }
            return out;
        };

        int utf32::decode(const char *data, size_t size, unsigned int &value, mytoml_endian_value_t order)
        {
            if (size < 4 || data == nullptr)
            {
                value = 0;
                return -1;
            }

            const unsigned char *b = reinterpret_cast<const unsigned char *>(data);
            uint32_t tmp = 0;

            if (order == mytoml_endian_t::native)
            {
                memcpy(&tmp, b, 4);
            }
            else if (order == mytoml_endian_t::little)
            {
                tmp = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
            }
            else // big-endian
            {
                tmp = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | (uint32_t)b[3];
            }

            value = static_cast<unsigned int>(tmp);
            return 4;
        };

        int utf32::encode(unsigned int codepoint, utf32::char_t *output, size_t size, mytoml_endian_value_t order)
        {
            if (size < 4 || output == nullptr)
            {
                return -1;
            }

            uint32_t cp = static_cast<uint32_t>(codepoint);
            unsigned char *out = reinterpret_cast<unsigned char *>(output);

            if (order == mytoml_endian_t::native)
            {
                memcpy(out, &cp, 4);
            }
            else if (order == mytoml_endian_t::little)
            {
                out[0] = static_cast<unsigned char>(cp & 0xFF);
                out[1] = static_cast<unsigned char>((cp >> 8) & 0xFF);
                out[2] = static_cast<unsigned char>((cp >> 16) & 0xFF);
                out[3] = static_cast<unsigned char>((cp >> 24) & 0xFF);
            }
            else // big-endian
            {
                out[0] = static_cast<unsigned char>((cp >> 24) & 0xFF);
                out[1] = static_cast<unsigned char>((cp >> 16) & 0xFF);
                out[2] = static_cast<unsigned char>((cp >> 8) & 0xFF);
                out[3] = static_cast<unsigned char>(cp & 0xFF);
            }

            return 1;
        };

        std::string utf32::to_utf8(const std::vector<unsigned char> &bytes, mytoml_endian_value_t order)
        {
            std::string out;
            size_t idx = 0;
            while (idx + 4 <= bytes.size())
            {
                unsigned int cp = 0;
                int dec = utf32::decode(reinterpret_cast<const char *>(bytes.data() + idx), bytes.size() - idx, cp, order);
                MYTOML_ASSERT(dec == 4);
                (void)dec;
                char tmp[4];
                int enc = utf8::encode(cp, reinterpret_cast<utf8::char_t *>(tmp), sizeof(tmp));
                MYTOML_ASSERT(enc > 0);
                out.append(tmp, tmp + enc);
                idx += 4;
            }
            return out;
        };

        //-------------------------------------------------------------------------
        // [SECTION] Details : Iterators
        //-------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // [Class] iterator
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------

        template <class node_type>
        iterator<node_type>::iterator(pointer toml) noexcept
            : m_table(toml)
        {
            if (m_table != nullptr && m_table->is_array())
            {
                m_type = iterator_t::array;
            }
            else
            {
                m_type = iterator_t::table;
            }
        }

        template <class node_type>
        template <typename T, typename std::enable_if<std::is_const<T>::value, int>::type>
        iterator<node_type>::iterator(const other_iterator &other) noexcept
            : m_table(other.m_table)
        {
            if (other.type() == other_iterator::iterator_t::array)
            {
                m_type = iterator_t::array;
                m_holder.array_iter = other.m_holder.array_iter;
            }
            else
            {
                m_type = iterator_t::table;
                m_holder.table_iter = other.m_holder.table_iter;
            }
        }

        template <class node_type>
        iterator<node_type>::iterator(const table_iterator &itr) noexcept
            : m_type(iterator_t::table)
        {
            m_holder.table_iter = itr;
        }

        template <class node_type>
        iterator<node_type>::iterator(const array_iterator &itr) noexcept
            : m_type(iterator_t::array)
        {
            m_holder.array_iter = itr;
        }

        template <class node_type>
        typename iterator<node_type>::iterator_t iterator<node_type>::type() const noexcept
        {
            return m_type;
        }

        template <class node_type>
        const std::string &iterator<node_type>::key() const
        {
            if (m_type != iterator_t::table)
            {
                MYTOML_THROW(std::runtime_error("Cannot retrieve key from non-table iterator"));
            }
            return m_holder.table_iter->first;
        }

        template <class node_type>
        typename iterator<node_type>::reference iterator<node_type>::value() const noexcept
        {
            return operator*();
        }

        template <class node_type>
        bool iterator<node_type>::operator==(const mytoml::detail::iterator<node_type> &rhs) const
        {
            if (m_type != rhs.m_type)
            {
                return false;
            }

            if (m_type == iterator_t::array)
            {
                return m_holder.array_iter == rhs.m_holder.array_iter;
            }

            return m_holder.table_iter == rhs.m_holder.table_iter;
        }

        template <class node_type>
        bool iterator<node_type>::operator==(const typename iterator<node_type>::other_iterator &rhs) const
        {
            if (static_cast<unsigned int>(m_type) != static_cast<unsigned int>(rhs.type()))
            {
                return false;
            }

            if (m_type == iterator_t::array)
            {
                return m_holder.array_iter == rhs.m_holder.array_iter;
            }

            return m_holder.table_iter == rhs.m_holder.table_iter;
        }

        template <class node_type>
        bool iterator<node_type>::operator!=(const mytoml::detail::iterator<node_type> &rhs) const
        {
            return !(*this == rhs);
        }

        template <class node_type>
        bool iterator<node_type>::operator!=(const typename iterator<node_type>::other_iterator &rhs) const
        {
            return !(*this == rhs);
        }

        template <class node_type>
        bool iterator<node_type>::operator<(const mytoml::detail::iterator<node_type> &rhs) const
        {
            if (m_type != rhs.m_type)
            {
                return static_cast<unsigned int>(m_type) < static_cast<unsigned int>(rhs.m_type);
            }

            if (m_type == iterator_t::array)
            {
                return m_holder.array_iter < rhs.m_holder.array_iter;
            }

            return false;
        }

        template <class node_type>
        bool iterator<node_type>::operator<(const typename iterator<node_type>::other_iterator &rhs) const
        {
            const auto lhs_type = static_cast<unsigned int>(m_type);
            const auto rhs_type = static_cast<unsigned int>(rhs.type());
            if (lhs_type != rhs_type)
            {
                return lhs_type < rhs_type;
            }

            if (m_type == iterator_t::array)
            {
                return m_holder.array_iter < rhs.m_holder.array_iter;
            }

            return false;
        }

        template <class node_type>
        bool iterator<node_type>::operator<=(const mytoml::detail::iterator<node_type> &rhs) const
        {
            return (*this < rhs) || (*this == rhs);
        }

        template <class node_type>
        bool iterator<node_type>::operator<=(const typename iterator<node_type>::other_iterator &rhs) const
        {
            return (*this < rhs) || (*this == rhs);
        }

        template <class node_type>
        bool iterator<node_type>::operator>(const mytoml::detail::iterator<node_type> &rhs) const
        {
            return rhs < *this;
        }

        template <class node_type>
        bool iterator<node_type>::operator>(const typename iterator<node_type>::other_iterator &rhs) const
        {
            return rhs < *this;
        }

        template <class node_type>
        bool iterator<node_type>::operator>=(const mytoml::detail::iterator<node_type> &rhs) const
        {
            return !(*this < rhs);
        }

        template <class node_type>
        bool iterator<node_type>::operator>=(const typename iterator<node_type>::other_iterator &rhs) const
        {
            return !(*this < rhs);
        }

        template <class node_type>
        typename iterator<node_type>::pointer iterator<node_type>::operator->() noexcept
        {
            if (m_type == iterator_t::array)
            {
                return &(*m_holder.array_iter);
            }
            return &m_holder.table_iter->second;
        }

        template <class node_type>
        typename iterator<node_type>::reference iterator<node_type>::operator*() const noexcept
        {
            if (m_type == iterator_t::array)
            {
                return *m_holder.array_iter;
            }
            return m_holder.table_iter->second;
        }

        template <class node_type>
        iterator<node_type> iterator<node_type>::operator+(difference_type i) const noexcept
        {
            iterator result(*this);
            result += i;
            return result;
        }

        template <class node_type>
        iterator<node_type> &iterator<node_type>::operator+=(difference_type i) noexcept
        {
            if (i >= 0)
            {
                while (i-- > 0)
                {
                    ++(*this);
                }
            }
            else
            {
                while (i++ < 0)
                {
                    --(*this);
                }
            }
            return *this;
        }

        template <class node_type>
        iterator<node_type> &iterator<node_type>::operator++() noexcept
        {
            if (m_type == iterator_t::array)
            {
                ++m_holder.array_iter;
            }
            else
            {
                ++m_holder.table_iter;
            }
            return *this;
        }

        template <class node_type>
        iterator<node_type> iterator<node_type>::operator++(int) & noexcept
        {
            iterator result(*this);
            ++(*this);
            return result;
        }

        template <class node_type>
        iterator<node_type> iterator<node_type>::operator-(difference_type i) const noexcept
        {
            iterator result(*this);
            result -= i;
            return result;
        }

        template <class node_type>
        iterator<node_type> &iterator<node_type>::operator-=(difference_type i) noexcept
        {
            return (*this += -i);
        }

        template <class node_type>
        iterator<node_type> &iterator<node_type>::operator--() noexcept
        {
            if (m_type == iterator_t::array)
            {
                --m_holder.array_iter;
            }
            else
            {
                --m_holder.table_iter;
            }
            return *this;
        }

        template <class node_type>
        iterator<node_type> iterator<node_type>::operator--(int) & noexcept
        {
            iterator result(*this);
            --(*this);
            return result;
        }

        //-----------------------------------------------------------------------------
        // [Class] reverse_iterator
        //-----------------------------------------------------------------------------
        //-----------------------------------------------------------------------------

        template <class iterator_type>
        reverse_iterator<iterator_type>::reverse_iterator(const iterator_type &iter) noexcept
            : base_iterator(iter)
        {
        }

        template <class iterator_type>
        reverse_iterator<iterator_type>::reverse_iterator(const base_iterator &iter) noexcept
            : base_iterator(iter)
        {
        }

        template <class iterator_type>
        const std::string &reverse_iterator<iterator_type>::key() const
        {
            auto it = this->base();
            --it;
            return it.key();
        }

        template <class iterator_type>
        typename reverse_iterator<iterator_type>::reference reverse_iterator<iterator_type>::value() const noexcept
        {
            auto it = this->base();
            --it;
            return *it;
        }

        template <class iterator_type>
        typename reverse_iterator<iterator_type>::reference reverse_iterator<iterator_type>::operator[](difference_type i) const
        {
            return *(*this + i);
        }

        template <class iterator_type>
        reverse_iterator<iterator_type> reverse_iterator<iterator_type>::operator+(difference_type i) const noexcept
        {
            reverse_iterator result(*this);
            result += i;
            return result;
        }

        template <class iterator_type>
        reverse_iterator<iterator_type> &reverse_iterator<iterator_type>::operator+=(difference_type i) noexcept
        {
            if (i >= 0)
            {
                while (i-- > 0)
                {
                    ++(*this);
                }
            }
            else
            {
                while (i++ < 0)
                {
                    --(*this);
                }
            }
            return *this;
        }

        template <class iterator_type>
        reverse_iterator<iterator_type> &reverse_iterator<iterator_type>::operator++() noexcept
        {
            base_iterator::operator++();
            return *this;
        }

        template <class iterator_type>
        reverse_iterator<iterator_type> reverse_iterator<iterator_type>::operator++(int) & noexcept
        {
            reverse_iterator result(*this);
            ++(*this);
            return result;
        }

        template <class iterator_type>
        reverse_iterator<iterator_type> reverse_iterator<iterator_type>::operator-(difference_type i) const noexcept
        {
            reverse_iterator result(*this);
            result -= i;
            return result;
        }

        template <class iterator_type>
        typename reverse_iterator<iterator_type>::difference_type reverse_iterator<iterator_type>::operator-(const reverse_iterator &other) const
        {
            difference_type distance = 0;
            auto it = other;

            while (it != *this)
            {
                ++it;
                ++distance;
            }

            return distance;
        }

        template <class iterator_type>
        reverse_iterator<iterator_type> &reverse_iterator<iterator_type>::operator-=(difference_type i) noexcept
        {
            return (*this += -i);
        }

        template <class iterator_type>
        reverse_iterator<iterator_type> &reverse_iterator<iterator_type>::operator--() noexcept
        {
            base_iterator::operator--();
            return *this;
        }

        template <class iterator_type>
        reverse_iterator<iterator_type> reverse_iterator<iterator_type>::operator--(int) & noexcept
        {
            reverse_iterator result(*this);
            --(*this);
            return result;
        }

        template class iterator<toml>;
        template class iterator<const toml>;
        template class reverse_iterator<iterator<toml>>;
        template class reverse_iterator<iterator<const toml>>;

        //-------------------------------------------------------------------------
        // [SECTION] Details : Input
        //-------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // [Class] file_iadapter
        //-----------------------------------------------------------------------------
        // - file_iadapter()
        // - read()
        //-----------------------------------------------------------------------------

        file_iadapter::file_iadapter(FILE *file)
            : m_file(file) {};

        size_t file_iadapter::read(void *data, size_t size)
        {
            if (m_file == nullptr || data == nullptr || size == 0)
            {
                return 0;
            }
            // fread returns number of elements read; using 1-byte elements.
            const size_t nsize = fread(data, 1, size, m_file);
            return nsize;
        };

#ifndef MYTOML_NO_STL

        //-----------------------------------------------------------------------------
        // [Class] stream_iadapter
        //-----------------------------------------------------------------------------
        // - stream_iadapter()
        // - read()
        //-----------------------------------------------------------------------------

        stream_iadapter::stream_iadapter(std::istream &stream)
            : m_stream(&stream) {};

        size_t stream_iadapter::read(void *data, size_t size)
        {
            if ((m_stream == nullptr) || (data == nullptr) || size == 0)
            {
                return 0;
            }
            // Read using istream::readsome when available to avoid blocking, but
            // fall back to read() which will block until requested size or EOF.
            std::istream &stream = *m_stream;
            stream.read(static_cast<char *>(data), static_cast<std::streamsize>(size));
            const std::streamsize got = stream.gcount();
            if (got < 0)
            {
                return 0;
            }
            return static_cast<size_t>(got);
        };

#endif // MYTOML_NO_STL

        //-----------------------------------------------------------------------------
        // [Class] memory_iadapter
        //-----------------------------------------------------------------------------
        // - memory_iadapter()
        // - read()
        //-----------------------------------------------------------------------------

        memory_iadapter::memory_iadapter(void *data, size_t size)
            : m_pos(0), m_size(size), m_data(data) {};

        size_t memory_iadapter::read(void *data, size_t size)
        {
            if ((data == nullptr) || size == 0 || m_data == nullptr)
            {
                return 0;
            }
            // remaining bytes
            if (m_pos >= m_size)
            {
                return 0;
            }
            const size_t remaining = m_size - m_pos;
            const size_t to_copy = (size < remaining) ? size : remaining;
            memcpy(data, static_cast<char *>(m_data) + m_pos, to_copy);
            m_pos += to_copy;
            return to_copy;
        };

#if 0
        //-----------------------------------------------------------------------------
        // [Class] lexer
        //-----------------------------------------------------------------------------
        // - lexer()
        //-----------------------------------------------------------------------------

        // Lexer constructor: read the whole input from the adapter, detect
        // encoding (using BOM when present), strip the BOM and convert the
        // payload into UTF-8 bytes stored in m_input. This approach simplifies
        // tokenization: subsequent get_char() returns UTF-8 bytes only.
        lexer::lexer(iadapter *adapter)
            : m_adapter(adapter), m_encoding(mytoml::encoding::unspecified), m_string(), m_input{}, m_input_pos(0), m_putback(-1), m_position{}, m_token{}, m_char(EOF)
        {
            if (!m_adapter)
            {
                m_token.type = token_t::end_of_input;
                return;
            }

            // Read a small header to probe encoding (BOM detection)
            unsigned char header[4] = {0};
            size_t got_header = m_adapter->read(header, sizeof(header));
            if (got_header > 0)
            {
                m_encoding = determine_encoding(header, got_header);
            }

            // Read the rest of the adapter into a raw buffer. We already
            // consumed header bytes from the adapter by reading them above,
            // so append the remainder as well.
            std::vector<unsigned char> raw;
            // start with header bytes we read (they came from adapter)
            for (size_t i = 0; i < got_header; ++i)
                raw.push_back(header[i]);

            // Read until adapter returns 0
            while (true)
            {
                char buf[4096];
                size_t n = m_adapter->read(buf, sizeof(buf));
                if (n == 0)
                    break;
                raw.insert(raw.end(), reinterpret_cast<unsigned char *>(buf), reinterpret_cast<unsigned char *>(buf) + n);
            }

            // Strip BOM bytes if present and convert to UTF-8 when needed.
            // If encoding is unspecified, prefer UTF-8 semantics (no conversion).
            size_t skip = 0;
            switch (m_encoding)
            {
            case encoding::utf8:
                if (raw.size() >= 3 && !memcmp(raw.data(), MYTOML_BOM_UTF8, 3))
                    skip = 3;
                break;
            case encoding::utf16le:
                if (raw.size() >= 2 && !memcmp(raw.data(), MYTOML_BOM_UTF16LE, 2))
                    skip = 2;
                break;
            case encoding::utf16be:
                if (raw.size() >= 2 && !memcmp(raw.data(), MYTOML_BOM_UTF16BE, 2))
                    skip = 2;
                break;
            case encoding::utf32le:
                if (raw.size() >= 4 && !memcmp(raw.data(), MYTOML_BOM_UTF32LE, 4))
                    skip = 4;
                break;
            case encoding::utf32be:
                if (raw.size() >= 4 && !memcmp(raw.data(), MYTOML_BOM_UTF32BE, 4))
                    skip = 4;
                break;
            default:
                m_encoding = encoding::utf8;
                break;
            }

            if (skip > raw.size())
                skip = 0;

            const std::vector<unsigned char> payload(raw.begin() + static_cast<std::ptrdiff_t>(skip), raw.end());

            if (m_encoding == encoding::utf16le)
            {
                m_input = utf16::to_utf8(payload, endian::little);
            }
            else if (m_encoding == encoding::utf16be)
            {
                m_input = utf16::to_utf8(payload, endian::big);
            }
            else if (m_encoding == encoding::utf32le)
            {
                m_input = utf32::to_utf8(payload, endian::little);
            }
            else if (m_encoding == encoding::utf32be)
            {
                m_input = utf32::to_utf8(payload, endian::big);
            }
            else
            {
                m_input.assign(reinterpret_cast<const char *>(payload.data()), payload.size());
            }

            m_input_pos = 0;
            m_position = mark{};
            m_token = token{};
            m_token.type = token_t::unknown;
            m_char = m_input.empty() ? EOF : static_cast<unsigned char>(m_input[0]);
        };

        mark lexer::position() const
        {
            return m_position;
        };

        void lexer::advance(size_t amount)
        {
            size_t times = 1;
            do
            {
                m_position.index += 1;
                m_position.column += 1;
                if (m_char == '\n')
                {
                    m_position.line += 1;
                    m_position.column = 0;
                }

                if (m_position.index < m_input.size())
                {
                    m_char = m_input[m_position.index];
                }

            } while (times != amount);
        };

        void lexer::reverse(size_t amount)
        {
            size_t times = 1;
            do
            {
                m_position.index -= 1;
                m_position.column -= 1;
                if (m_char == '\n')
                {
                    m_position.line -= 1;
                    m_position.column = 0;
                }

                if (m_position.index > 0)
                {
                    m_char = m_input[m_position.index];
                }

            } while (times != amount);
        };

        // Helper to scan different TOML value types (numbers, booleans, datetimes, bare keys)
        bool lexer::scan_value()
        {
            std::string current_text;
            current_text.push_back(static_cast<char>(m_char));

            while (true)
            {
                int next_c = get_char();
                // TOML delimiters and terminators for bare keys, numbers, literals, etc.
                if (next_c == EOF || next_c == ' ' || next_c == '\t' || next_c == '\n' || next_c == '\r' || next_c == '=' || next_c == ',' || next_c == '[' || next_c == ']' || next_c == '{' || next_c == '}' || next_c == '#')
                {
                    unget_char(); // Put back the character that ended the scan
                    break;
                }
                current_text.push_back(static_cast<char>(next_c));
            }

            // Attempt to identify the type in order of specificity
            if (scan_literal(current_text))
            {                                         // Handles true, false, null
                m_token.type = token_t::true_literal; // Default to true literal, scan_literal will correct it
                m_token.text = current_text;
                return true;
            }
            if (scan_datetime_literal(current_text))
            { // Handles dates, times, datetimes
                // token_type is set inside scan_datetime_literal
                m_token.text = current_text;
                return true;
            }
            if (scan_number(current_text))
            { // Handles integers and floats
                m_token.type = token_t::number_value;
                m_token.text = current_text;
                return true;
            }

            // If it's not a recognized literal, number, or datetime, it's likely a bare key.
            // Bare keys in TOML can contain letters, numbers, underscores, and dashes.
            // This simple check assumes it's a valid bare key if it passes the delimiter checks.
            m_token.type = token_t::string_value; // Treat as string (bare key) for now. Parser will interpret.
            m_token.text = current_text;
            return true;
        };

        // TOML specific string scanning: basic strings '...'
        bool lexer::scan_basic_string()
        {
            std::string str_val;
            while (true)
            {
                int c = get_char();
                if (c == EOF)
                { // Unterminated string
                    MYTOML_THROW(parse_error("Unterminated basic string"));
                }
                if (c == '\'')
                { // End of basic string
                    m_token.type = token_t::basic_string;
                    m_token.text = str_val;
                    return true;
                }
                if (c == '\\')
                { // Escape sequence
                    int esc = get_char();
                    if (esc == EOF)
                        MYTOML_THROW(parse_error("Unterminated escape sequence"));
                    switch (esc)
                    {
                    case '\'':
                        str_val += '\'';
                        break;
                    case '\\':
                        str_val += '\\';
                        break;
                    case 'b':
                        str_val += '\b';
                        break;
                    case 'f':
                        str_val += '\f';
                        break;
                    case 'n':
                        str_val += '\n';
                        break;
                    case 'r':
                        str_val += '\r';
                        break;
                    case 't':
                        str_val += '\t';
                        break;
                    // TOML supports \uXXXX and \UXXXXXXXX unicode escapes
                    case 'u':            // \uXXXX
                    case 'U':            // \UXXXXXXXX
                        str_val += '\\'; // Keep escape char
                        str_val += static_cast<char>(esc);
                        for (int i = 0; i < (esc == 'u' ? 4 : 8); ++i)
                        {
                            int hex_c = get_char();
                            if (hex_c == EOF)
                                MYTOML_THROW(parse_error("Incomplete unicode escape sequence"));
                            str_val += static_cast<char>(hex_c);
                        }
                        break;
                    default:
                        MYTOML_THROW(parse_error("Invalid escape sequence"));
                    }
                }
                else
                {
                    // Basic strings do NOT allow newline characters
                    if (c == '\n' || c == '\r')
                    {
                        MYTOML_THROW(parse_error("Newline character found in basic string"));
                    }
                    str_val += static_cast<char>(c);
                }
            }
            return false; // Should not reach here if successful
        }

        // TOML literal strings '''...''' (no escapes)
        bool lexer::scan_literal_string()
        {
            std::string str_val;
            // Consume opening '''
            for (int i = 0; i < 3; ++i)
                get_char();

            while (true)
            {
                int c = get_char();
                if (c == EOF)
                    MYTOML_THROW(parse_error("Unterminated literal string"));
                if (c == '\'')
                {
                    // Check for closing '''
                    if (get_char() == '\'')
                    {
                        if (get_char() == '\'')
                        {
                            m_token.type = token_t::literal_string;
                            m_token.text = str_val;
                            return true;
                        }
                        unget_char();    // Only two ' found, continue
                        str_val += '\''; // Append first '
                        str_val += '\''; // Append second '
                    }
                    else
                    {
                        unget_char();    // Only one ' found, continue
                        str_val += '\''; // Append the '
                    }
                }
                else
                {
                    str_val += static_cast<char>(c);
                }
            }
            return false; // Should not reach here
        };

        // TOML multiline basic strings """..."""
        bool lexer::scan_multiline_basic_string()
        {
            std::string str_val;
            // Consume opening """
            for (int i = 0; i < 3; ++i)
                get_char();

            while (true)
            {
                int c = get_char();
                if (c == EOF)
                    MYTOML_THROW(parse_error("Unterminated multiline basic string"));
                if (c == '"')
                {
                    if (get_char() == '"')
                    {
                        if (get_char() == '"')
                        {
                            m_token.type = token_t::multiline_basic_string;
                            m_token.text = str_val;
                            return true;
                        }
                        unget_char(); // Only two " found
                        str_val += '"';
                        str_val += '"';
                    }
                    else
                    {
                        unget_char(); // Only one " found
                        str_val += '"';
                    }
                }
                else
                {
                    str_val += static_cast<char>(c);
                }
            }
            return false; // Should not reach here
        };

        // TOML multiline literal strings ''''...''''
        bool lexer::scan_multiline_literal_string()
        {
            std::string str_val;
            // Consume opening '''
            for (int i = 0; i < 3; ++i)
                get_char();

            while (true)
            {
                int c = get_char();
                if (c == EOF)
                    MYTOML_THROW(parse_error("Unterminated multiline literal string"));
                if (c == '\'')
                {
                    if (get_char() == '\'')
                    {
                        if (get_char() == '\'')
                        {
                            m_token.type = token_t::multiline_literal_string;
                            m_token.text = str_val;
                            return true;
                        }
                        unget_char(); // Only two ' found
                        str_val += '\'';
                        str_val += '\'';
                    }
                    else
                    {
                        unget_char(); // Only one ' found
                        str_val += '\'';
                    }
                }
                else
                {
                    str_val += static_cast<char>(c);
                }
            }
            return false; // Should not reach here
        }

        // Regular string scanning (quoted strings) "..."
        bool lexer::scan_string()
        {
            std::string str_val;
            while (true)
            {
                int c = get_char();
                if (c == EOF)
                { // Unterminated string
                    MYTOML_THROW(parse_error("Unterminated string"));
                }
                if (c == '"')
                { // End of string
                    m_token.type = token_t::string_value;
                    m_token.text = str_val;
                    return true;
                }
                if (c == '\\')
                { // Escape sequence
                    int esc = get_char();
                    if (esc == EOF)
                        MYTOML_THROW(parse_error("Unterminated escape sequence"));
                    switch (esc)
                    {
                    case '"':
                        str_val += '"';
                        break;
                    case '\\':
                        str_val += '\\';
                        break;
                    case 'b':
                        str_val += '\b';
                        break;
                    case 'f':
                        str_val += '\f';
                        break;
                    case 'n':
                        str_val += '\n';
                        break;
                    case 'r':
                        str_val += '\r';
                        break;
                    case 't':
                        str_val += '\t';
                        break;
                        // TOML allows \uXXXX and \UXXXXXXXX unicode escapes in quoted strings too.
                    case 'u':            // \uXXXX
                    case 'U':            // \UXXXXXXXX
                        str_val += '\''; // Keep escape char
                        str_val += static_cast<char>(esc);
                        for (int i = 0; i < (esc == 'u' ? 4 : 8); ++i)
                        {
                            int hex_c = get_char();
                            if (hex_c == EOF)
                                MYTOML_THROW(parse_error("Incomplete unicode escape sequence"));
                            str_val += static_cast<char>(hex_c);
                        }
                        break;
                    default:
                        MYTOML_THROW(parse_error("Invalid escape sequence"));
                    }
                }
                else
                {
                    str_val += static_cast<char>(c);
                }
            }
            return false; // Should not reach here
        }

        // Handles comments starting with '#'
        void lexer::scan_comment()
        {
            while (true)
            {
                int c = get_char();
                if (c == EOF || c == '\n' || c == '\r')
                {
                    m_position.line++;
                    m_position.column = 0;
                    m_position.index++;
                    unget_char(); // Put back newline/CR to be processed by next_token
                    break;
                }
                // Consume characters until newline or EOF
            }
        }

        // Scans for TOML literals: true, false, null
        bool lexer::scan_literal(const std::string &current_text)
        {
            if (current_text == "true")
            {
                m_token.type = token_t::true_literal;
                return true;
            }
            if (current_text == "false")
            {
                m_token.type = token_t::false_literal;
                return true;
            }
            if (current_text == "null")
            {
                m_token.type = token_t::null_literal;
                return true;
            }
            return false;
        }

        // Scans for TOML numbers (integers, floats, scientific notation, hex, octal, binary)
        bool lexer::scan_number(const std::string &current_text)
        {
            try
            {
                // Check for potential float/scientific notation or signs
                bool is_float_like = false;
                if (current_text.find('.') != std::string::npos || current_text.find('e') != std::string::npos || current_text.find('E') != std::string::npos)
                {
                    is_float_like = true;
                }
                else if (!current_text.empty() && (current_text.back() == '+' || current_text.back() == '-'))
                {
                    is_float_like = true; // Trailing sign might indicate scientific notation
                }

                // Check for hex, octal, binary prefixes
                if (current_text.length() > 2 && current_text[0] == '0')
                {
                    char prefix = current_text[1];
                    if (prefix == 'x' || prefix == 'o' || prefix == 'b')
                    {
                        // It's a hex, octal, or binary number.
                        // The parser will handle validation.
                    }
                    if (prefix == '.')
                    { // e.g., 0.123 is a float
                        is_float_like = true;
                    }
                }

                // Minimal check for general number format
                bool starts_with_digit_or_sign = !current_text.empty() && (isdigit(current_text[0]) || current_text[0] == '+' || current_text[0] == '-');
                if (!starts_with_digit_or_sign)
                    return false;

                for (size_t i = 1; i < current_text.length(); ++i)
                {
                    char c = current_text[i];
                    // Allow digits, '.', 'e', 'E', '+', '-', '_'
                    if (!isdigit(c) && c != '.' && c != 'e' && c != 'E' && c != '+' && c != '-' && c != '_')
                    {
                        return false;
                    }
                }

                // If it passed basic checks, consider it a number token.
                return true;
            }
            catch (const std::exception &)
            {
                // If standard parsing fails, it's not a number in that form.
                return false; // Treat as not a number token.
            }
        }

        // Scans for TOML datetime literals (date, time, datetime)
        bool lexer::scan_datetime_literal(const std::string &current_text)
        {
            // TOML datetimes follow RFC 3339 / ISO 8601.
            bool has_digits = false;
            bool has_separators = false;
            for (char c : current_text)
            {
                if (isdigit(c))
                    has_digits = true;
                else if (c == '-' || c == ':' || c == 'T' || c == '.' || c == 'Z' || c == '+' || c == ' ')
                    has_separators = true;
                else if (!(isalpha(c)))
                    return false; // Allow letters only in datetime specifiers like 'Z'
            }

            if (has_digits && has_separators)
            {
                // Determine the specific token type based on format.
                if (current_text.find('T') != std::string::npos || current_text.find('+') != std::string::npos || current_text.find('Z') != std::string::npos || current_text.find(' ') != std::string::npos)
                { // Includes offset or 'Z' or space separator
                    m_token.type = token_t::datetime_literal;
                }
                else if (current_text.find(':') != std::string::npos)
                { // Contains time separators
                    m_token.type = token_t::time_literal;
                }
                else if (current_text.find('-') != std::string::npos)
                { // Contains date separators
                    m_token.type = token_t::date_literal;
                }
                else
                {
                    m_token.type = token_t::datetime_literal; // Default if ambiguous
                }
                return true;
            }
            return false;
        }

        [[nodiscard]] int lexer::get_char()
        {
            // return putback if available
            if (m_putback != -1)
            {
                int c = m_putback;
                m_putback = -1;
                return c;
            }

            // ensure input buffer has data; if empty, read more from adapter
            if (m_input_pos >= m_input.size())
            {
                if (!m_adapter)
                    return EOF;
                char buf[512];
                size_t n = m_adapter->read(buf, sizeof(buf));
                if (n == 0)
                    return EOF;
                m_input.append(buf, buf + n);
            }

            unsigned char c = static_cast<unsigned char>(m_input[m_input_pos++]);
            return static_cast<int>(c);
        }

        void lexer::unget_char()
        {
            // allow single-char putback
            if (m_input_pos > 0)
            {
                --m_input_pos;
            }
        }

        void lexer::add_char(int character)
        {
            m_string.push_back(static_cast<char>(character));
        }

        token lexer::get_token()
        {
            return m_token;
        }

        token_t lexer::get_type() const
        {
            return m_token.type;
        }

        // Helper: skip whitespace and return first non-space character (but
        // leave it buffered via putback semantics)
        int lexer::skip_ws()
        {
            int c;
            do
            {
                c = get_char();
                if (c == EOF)
                    return EOF;
            } while (c == ' ' || c == '\t' || c == '\n' || c == '\r');
            // put it back for caller
            unget_char();
            return c;
        }

        token lexer::next_token()
        {
            m_string.clear();
            m_token.text.clear();
            m_token.type = token_t::unknown;

            int c = get_char();
            while (c != EOF && (c == ' ' || c == '\t' || c == '\n' || c == '\r'))
            {
                if (c == '\n')
                { // Handle newline for position tracking
                    m_position.line++;
                    m_position.column = 0;
                }
                else
                {
                    m_position.column++;
                }
                m_position.index++;
                c = get_char();
            }

            if (c == EOF)
            {
                m_token.type = token_t::end_of_input;
                return m_token;
            }
            m_char = c; // Store the first non-whitespace character

            m_token.start = m_position; // Mark the start of the token

            switch (m_char)
            {
            case '=':
                m_token.type = token_t::name_separator;
                break;
            case ',':
                m_token.type = token_t::value_separator;
                break;
            case '[':
                // Peek ahead for '[[', indicating array of tables
                if (get_char() == '[')
                {
                    m_token.type = token_t::array_start; // Use existing array_start for simplicity, parser needs to distinguish
                    m_token.text = "[[";                 // Store for parser context
                    advance();                           // Consume the second '['
                }
                else
                {
                    unget_char(); // Put back the char if it wasn't '[['
                    m_token.type = token_t::array_start;
                    m_token.text = "[";
                }
                break;
            case ']':
                m_token.type = token_t::array_end;
                break;
            case '{':
                m_token.type = token_t::object_start;
                break; // Used for inline tables
            case '}':
                m_token.type = token_t::object_end;
                break; // Used for inline tables
            case '"':  // Standard quoted string
                if (scan_string())
                    return m_token;
                break;
            case '\'': // Basic string '...'
                if (scan_basic_string())
                    return m_token;
                break;
            case '#':                // Comment
                scan_comment();      // Consume the rest of the line
                return next_token(); // Return the next token after comment
            default:
                if (scan_value())
                    return m_token; // Handles numbers, bools, datetimes, and bare keys
                break;
            }

            m_token.end = m_position; // Mark the end of the token (even if unknown)
            return m_token;
        };

        //-----------------------------------------------------------------------------
        // [Class] Parser
        //-----------------------------------------------------------------------------

        // Helper for parsing unicode escape sequences
        namespace
        {
            static bool is_hex_digit(char ch) { return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'); }
            static unsigned int hex_value(char ch)
            {
                if (ch >= '0' && ch <= '9')
                    return static_cast<unsigned int>(ch - '0');
                if (ch >= 'a' && ch <= 'f')
                    return static_cast<unsigned int>(10 + (ch - 'a'));
                return static_cast<unsigned int>(10 + (ch - 'A'));
            }

            // Helper to parse UTF-16 escape sequences (\uXXXX)
            static unsigned int parse_u16_hex(const std::string &text, size_t index)
            {
                if (index + 4 > text.size())
                    MYTOML_THROW(parse_error("Invalid unicode escape sequence"));
                unsigned int value = 0;
                for (size_t i = 0; i < 4; ++i)
                {
                    const char ch = text[index + i];
                    if (!is_hex_digit(ch))
                        MYTOML_THROW(parse_error("Invalid unicode escape sequence"));
                    value = (value << 4U) | hex_value(ch);
                }
                return value;
            }

            // Helper to parse UTF-32 escape sequences (\UXXXXXXXX)
            static unsigned int parse_u32_hex(const std::string &text, size_t index)
            {
                if (index + 8 > text.size())
                    MYTOML_THROW(parse_error("Invalid unicode escape sequence"));
                unsigned int value = 0;
                for (size_t i = 0; i < 8; ++i)
                {
                    const char ch = text[index + i];
                    if (!is_hex_digit(ch))
                        MYTOML_THROW(parse_error("Invalid unicode escape sequence"));
                    value = (value << 4U) | hex_value(ch);
                }
                return value;
            }

            // Helper to append a decoded UTF-8 codepoint to a string
            static void append_utf8_codepoint(std::string &out, unsigned int cp)
            {
                char tmp[4] = {0, 0, 0, 0};
                const int encoded = utf8::encode(cp, reinterpret_cast<utf8::char_t *>(tmp), sizeof(tmp));
                if (encoded <= 0)
                {
                    MYTOML_THROW(parse_error("Invalid unicode codepoint"));
                }
                out.append(tmp, tmp + encoded);
            }
        } // namespace

        parser::parser(lexer *lexer)
            : m_lexer(lexer), m_current{}
        {
            if (m_lexer == nullptr)
            {
                MYTOML_THROW(parse_error("Invalid parser state"));
            }
            advance(); // Read the first token
        }

        void parser::advance() { m_current = m_lexer->next_token(); }

        // Parses TOML quoted strings (handling escapes)
        std::string parser::parse_string(const std::string &text)
        {
            std::string result;
            result.reserve(text.size()); // Pre-allocate memory

            for (size_t i = 0; i < text.size(); ++i)
            {
                const char ch = text[i];
                if (ch != '\\')
                {
                    result.push_back(ch);
                    continue;
                }

                if (i + 1 >= text.size())
                {
                    MYTOML_THROW(parse_error("Invalid escape sequence"));
                }

                const char esc = text[++i];
                switch (esc)
                {
                case '"':
                    result.push_back('"');
                    break;
                case '\\':
                    result.push_back('\\');
                    break;
                case '/':
                    result.push_back('/');
                    break; // Allowed escape in TOML
                case 'b':
                    result.push_back('\b');
                    break;
                case 'f':
                    result.push_back('\f');
                    break;
                case 'n':
                    result.push_back('\n');
                    break;
                case 'r':
                    result.push_back('\r');
                    break;
                case 't':
                    result.push_back('\t');
                    break;
                case 'u':
                { // \uXXXX
                    unsigned int codepoint = parse_u16_hex(text, i + 1);
                    i += 4; // Consume the 4 hex digits
                    // Handle surrogate pairs for characters > 0xFFFF
                    if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
                    { // High surrogate
                        if (i + 6 >= text.size() || text[i + 1] != '' || text[i + 2] != 'u')
                        {
                            MYTOML_THROW(parse_error("Invalid unicode surrogate pair"));
                        }
                        unsigned int low = parse_u16_hex(text, i + 3);
                        if (low < 0xDC00 || low > 0xDFFF)
                        { // Must be a low surrogate
                            MYTOML_THROW(parse_error("Invalid unicode surrogate pair"));
                        }
                        // Combine surrogates into a single codepoint
                        codepoint = 0x10000 + (((codepoint - 0xD800) << 10U) | (low - 0xDC00));
                        i += 6; // Consume \uXXXX for the low surrogate
                    }
                    append_utf8_codepoint(result, codepoint);
                    break;
                }
                case 'U':
                { // \UXXXXXXXX
                    unsigned int codepoint = parse_u32_hex(text, i + 1);
                    i += 8; // Consume the 8 hex digits
                    // Check if codepoint is valid (within Unicode range)
                    if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF))
                    {
                        MYTOML_THROW(parse_error("Invalid unicode codepoint"));
                    }
                    append_utf8_codepoint(result, codepoint);
                    break;
                }
                default:
                    MYTOML_THROW(parse_error("Invalid escape sequence"));
                }
            }
            return result;
        }

        // Parses TOML numbers (integers and floats)
        toml parser::parse_number(const std::string &text)
        {
            try
            {
                // TOML numbers can be integers, floats, scientific notation, hex, octal, binary.
                // This is a simplified check; a full implementation would parse according to TOML spec.

                // Check for potential float/scientific notation or signs
                bool is_float_like = false;
                if (text.find('.') != std::string::npos || text.find('e') != std::string::npos || text.find('E') != std::string::npos)
                {
                    is_float_like = true;
                }
                else if (!text.empty() && (text.back() == '+' || text.back() == '-'))
                {
                    is_float_like = true; // Trailing sign might indicate scientific notation
                }

                // Check for hex, octal, binary prefixes
                if (text.length() > 2 && text[0] == '0')
                {
                    char prefix = text[1];
                    if (prefix == 'x' || prefix == 'o' || prefix == 'b')
                    {
                        // It's a hex, octal, or binary number.
                        // The parser will handle validation.
                    }
                    if (prefix == '.')
                    { // e.g., 0.123 is a float
                        is_float_like = true;
                    }
                }

                // Minimal check for general number format
                bool starts_with_digit_or_sign = !text.empty() && (isdigit(text[0]) || text[0] == '+' || text[0] == '-');
                if (!starts_with_digit_or_sign)
                    return false;

                for (size_t i = 1; i < text.length(); ++i)
                {
                    char c = text[i];
                    // Allow digits, '.', 'e', 'E', '+', '-', '_'
                    if (!isdigit(c) && c != '.' && c != 'e' && c != 'E' && c != '+' && c != '-' && c != '_')
                    {
                        return false;
                    }
                }

                // If it passed basic checks, consider it a number token.
                return true;
            }
            catch (const std::exception &)
            {
                // If standard parsing fails, it's not a number in that form.
                return false; // Treat as not a number token.
            }
        }

        // Parses TOML datetime, date, and time literals
        toml parser::parse_datetime(const std::string &text)
        {
            return toml(text); // Store as string for now
        }
        toml parser::parse_date(const std::string &text) { return toml(text); }
        toml parser::parse_time(const std::string &text) { return toml(text); }

        // Parses TOML arrays (e.g., [1, 2, 3])
        toml parser::parse_array()
        {
            toml array_val = toml::array();

            advance(); // Consume '['
            if (m_current.type == token_t::array_end)
            { // Empty array
                advance();
                return array_val;
            }

            while (true)
            {
                array_val.push_back(parse_value());

                if (m_current.type == token_t::value_separator)
                { // Comma found, expect more elements
                    advance();
                    continue;
                }

                if (m_current.type == token_t::array_end)
                { // End of array
                    advance();
                    break;
                }

                MYTOML_THROW(parse_error("Expected ',' or ']' in array"));
            }
            return array_val;
        }

        // Parses TOML tables (e.g., { key = value } or [table.name])
        toml parser::parse_object()
        { // Renamed to parse_table for TOML context
            toml table_val = toml::table();

            // Handle inline tables { key = value }
            if (m_current.type == token_t::object_start)
            {
                advance(); // Consume '{'
                if (m_current.type == token_t::object_end)
                { // Empty inline table
                    advance();
                    return table_val;
                }

                while (true)
                {
                    // Key can be a quoted or basic string token
                    if (m_current.type != token_t::string_value && m_current.type != token_t::basic_string)
                    {
                        MYTOML_THROW(parse_error("Expected table key (string) in inline table"));
                    }

                    std::string key = m_current.text; // Key is the token's text
                    advance();                        // Consume key

                    if (m_current.type != token_t::name_separator)
                    { // Expect '='
                        MYTOML_THROW(parse_error("Expected '=' after inline table key"));
                    }
                    advance(); // Consume '='

                    table_val[key] = parse_value();

                    if (m_current.type == token_t::value_separator)
                    { // Comma found, expect more key-value pairs
                        advance();
                        continue;
                    }

                    if (m_current.type == token_t::object_end)
                    { // End of inline table
                        advance();
                        break;
                    }
                    MYTOML_THROW(parse_error("Expected ',' or '}' in inline table"));
                }
            }
            else
            {
                // This function is specifically for inline tables. Top-level table headers are handled elsewhere.
                MYTOML_THROW(parse_error("Cannot parse non-inline table structure here"));
            }

            return table_val;
        }

        // Parses a single TOML value (string, number, boolean, null, array, table, datetime)
        toml parser::parse_value()
        {
            switch (m_current.type)
            {
            case token_t::null_literal:
                advance();
                return toml(nullptr);
            case token_t::true_literal:
                advance();
                return toml(true);
            case token_t::false_literal:
                advance();
                return toml(false);
            case token_t::number_value:
            {
                const std::string value = m_current.text;
                advance();
                return parse_number(value);
            }
            case token_t::string_value:
            { // Standard quoted string "..."
                const std::string value = parse_string(m_current.text);
                advance();
                return toml(value);
            }
            case token_t::basic_string:
            {                                                  // Basic string '...'
                const std::string value = scan_basic_string(); // Assumes scan_basic_string sets m_token.text
                advance();
                return toml(value);
            }
            case token_t::literal_string:
            {                                                    // Literal string '''...'''
                const std::string value = scan_literal_string(); // Assumes scan_literal_string updates m_token.text
                advance();
                return toml(value);
            }
            case token_t::multiline_basic_string:
            {                                                            // Multiline basic """..."""
                const std::string value = scan_multiline_basic_string(); // Assumes scan_multiline_basic_string updates m_token.text
                advance();
                return toml(value);
            }
            case token_t::multiline_literal_string:
            {                                                              // Multiline literal ''''...''''
                const std::string value = scan_multiline_literal_string(); // Assumes scan_multiline_literal_string updates m_token.text
                advance();
                return toml(value);
            }
            case token_t::datetime_literal:
            {
                const std::string value = m_current.text;
                advance();
                return parse_datetime(value);
            }
            case token_t::date_literal:
            {
                const std::string value = m_current.text;
                advance();
                return parse_date(value);
            }
            case token_t::time_literal:
            {
                const std::string value = m_current.text;
                advance();
                return parse_time(value);
            }
            case token_t::array_start:
                return parse_array();
            case token_t::object_start: // For inline tables
                return parse_object();
            default:
                MYTOML_THROW(parse_error("Unexpected token while parsing value"));
            }
        }

        // Main parse function for a TOML document.
        // This needs to handle [table] headers, [[array.of.tables]] headers,
        // and key-value pairs at different levels.
        toml parser::parse()
        {
            toml root_table = toml::table(); // TOML documents are essentially tables

            while (m_current.type != token_t::end_of_input)
            {
                if (m_current.type == token_t::comment)
                {
                    advance(); // Skip comments
                    continue;
                }

                // Handle table headers: [table.name] and [[array.of.tables]]
                if (m_current.type == token_t::name_separator && m_current.text == "[")
                { // Check for '[' token
                    // This needs careful parsing to distinguish [table] from [[array]]
                    // and to parse the full dotted key. This is a complex part of TOML parsing.
                    MYTOML_THROW(parse_error("Table header parsing not yet implemented"));
                }

                // If it's not a header or comment, it must be a key-value pair or an inline table assignment
                if (m_current.type == token_t::string_value || m_current.type == token_t::basic_string /* add other key types if needed */)
                {
                    std::string key = m_current.text; // Key is the token's text
                    advance();                        // Consume key

                    if (m_current.type != token_t::name_separator)
                    { // Expect '='
                        MYTOML_THROW(parse_error("Expected '=' after key"));
                    }
                    advance(); // Consume '='

                    root_table[key] = parse_value(); // Parse the value
                    continue;
                }

                if (m_current.type == token_t::object_start)
                { // Inline table assignment: key = { ... }
                    // This case is handled within parse_value now if '{' is encountered after '='
                    // If '{' is encountered at the top level without a preceding key, it's an error.
                    MYTOML_THROW(parse_error("Unexpected inline table start at top level"));
                }

                MYTOML_THROW(parse_error("Unexpected token at top level"));
            }

            return root_table;
        }

#endif

        //-------------------------------------------------------------------------
        // [SECTION] Details : Output
        //-------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        // [Class] file_oadapter
        //-----------------------------------------------------------------------------
        // - file_oadapter()
        // - write()
        //-----------------------------------------------------------------------------

        file_oadapter::file_oadapter(FILE *file)
            : m_file(file) {};

        size_t file_oadapter::write(const void *data, size_t size)
        {
            if ((m_file == nullptr) || (data == nullptr) || size == 0)
            {
                return 0;
            }
            const size_t nsize = fwrite(data, 1, size, m_file);
            return nsize;
        };

#ifndef MYTOML_NO_STL

        //-----------------------------------------------------------------------------
        // [Class] stream_oadapter
        //-----------------------------------------------------------------------------
        // - stream_oadapter()
        // - write()
        //-----------------------------------------------------------------------------

        stream_oadapter::stream_oadapter(std::ostream &stream)
            : m_stream(&stream) {};

        size_t stream_oadapter::write(const void *data, size_t size)
        {
            if ((m_stream == nullptr) || (data == nullptr) || size == 0)
            {
                return 0;
            }
            std::ostream &stream = *m_stream;
            stream.write(static_cast<const char *>(data), static_cast<std::streamsize>(size));
            if (!stream.good())
            {
                return 0;
            }
            return size;
        };

#endif // MYTOML_NO_STL

        //-----------------------------------------------------------------------------
        // [Class] memory_oadapter
        //-----------------------------------------------------------------------------
        // - memory_oadapter()
        // - write()
        //-----------------------------------------------------------------------------

        memory_oadapter::memory_oadapter(void *data, size_t size)
            : m_pos(0), m_size(size), m_data(data) {};

        size_t memory_oadapter::write(const void *data, size_t size)
        {
            if ((data == nullptr) || size == 0 || m_data == nullptr)
            {
                return 0;
            }
            // remaining space
            if (m_pos >= m_size)
            {
                return 0;
            }
            const size_t remaining = m_size - m_pos;
            const size_t to_copy = (size < remaining) ? size : remaining;
            memcpy(static_cast<char *>(m_data) + m_pos, data, to_copy);
            m_pos += to_copy;
            return to_copy;
        };

        /** @} group output */

        //-----------------------------------------------------------------------------
        // [SECTION] Functions Definitions : Detail
        //-----------------------------------------------------------------------------

        const char *string(token_t type)
        {
            switch (type)
            {
            default:
                return "unknown";
            }
        };

        const char *string(error_t type)
        {
            switch (type)
            {
            default:
                return "unknown";
            }
        };

        const char *string(event_t type)
        {
            switch (type)
            {
            default:
                return "unknown";
            }
        };

        const char *string(value_t type)
        {
            switch (type)
            {
            default:
                return "unknown";
            }
        };

        const char *string(break_t type)
        {
            switch (type)
            {
            case break_t::cr:
                return "cr";
            case break_t::ln:
                return "ln";
            case break_t::crln:
                return "crln";
            default:
                return "any";
            }
        };

        const char *string(mark type)
        {
            (void)type;
            return "mark";
        };

        const char *string(event type)
        {
            (void)type;
            return "event";
        };

        const char *string(token type)
        {
            (void)type;
            return "token";
        };

#ifndef MYTOML_NO_STL

        std::ostream &operator<<(std::ostream &ostream, const token_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const error_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const event_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const value_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const break_t &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const mark &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const event &type)
        {
            ostream << string(type);
            return ostream;
        };

        std::ostream &operator<<(std::ostream &ostream, const token &type)
        {
            ostream << string(type);
            return ostream;
        };

#endif // MYTOML_NO_STL

    }; // namespace detail

    MYTOML_VERSION_NAMESPACE_END
}; // namespace mytoml

#pragma endregion // Detail

#pragma region Mytoml

namespace mytoml
{
    MYTOML_VERSION_NAMESPACE_BEGIN

    //-----------------------------------------------------------------------------
    // [SECTION] Mytoml : Classes
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // [Class] version
    //-----------------------------------------------------------------------------
    // Semver versioning
    //-----------------------------------------------------------------------------
    // - version()
    // - operator<()
    // - operator>()
    // - operator==()
    // - operator!=()
    // - operator<=()
    // - operator>=()
    // - operator<<()
    //-----------------------------------------------------------------------------

    version::version(int major_value, int minor_value, int patch_value)
        : major(major_value), minor(minor_value), patch(patch_value)
    {
    }

    bool operator<(const version &lhs, const version &rhs) noexcept
    {
        return lhs.major < rhs.major || (lhs.major == rhs.major && lhs.minor < rhs.minor) ||
               (lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch < rhs.patch);
    };

    bool operator>(const version &lhs, const version &rhs) noexcept { return rhs < lhs; };

    bool operator==(const version &lhs, const version &rhs) noexcept
    {
        return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch == rhs.patch;
    };

    bool operator!=(const version &lhs, const version &rhs) noexcept { return !(lhs == rhs); };

    bool operator<=(const version &lhs, const version &rhs) noexcept { return !(rhs < lhs); };

    bool operator>=(const version &lhs, const version &rhs) noexcept { return !(lhs < rhs); };

#ifndef MYTOML_NO_STL

    std::ostream &operator<<(std::ostream &ostream, const version &version)
    {
        ostream << version.major << '.' << version.minor << '.' << version.patch;
        return ostream;
    };

#endif // MYTOML_NO_STL

    //-----------------------------------------------------------------------------
    // [Class] toml
    //-----------------------------------------------------------------------------

    //========== Constructors ==========

    toml::toml() noexcept
        : m_value(nullptr)
    {
    }

    toml::toml(std::nullptr_t) noexcept
        : m_value(nullptr) {
          };

    toml::toml(bool value) noexcept
        : m_value(value) {
          };

    toml::toml(int value) noexcept
        : m_value(static_cast<integer_t>(value)) {
          };

    toml::toml(value_t value) noexcept
    {
        switch (value)
        {
        case value_t::unknown:
            m_value = nullptr;
            break;
        case value_t::null:
            m_value = nullptr;
            break;
        case value_t::boolean:
            m_value = false;
            break;
        case value_t::integer:
            m_value = static_cast<integer_t>(0);
            break;
        case value_t::floating:
        case value_t::number:
            m_value = static_cast<number_t>(0.0);
            break;
        case value_t::empty:
        case value_t::string:
            m_value = string_t();
            break;
        case value_t::array:
            m_value = std::make_shared<array_t>();
            break;
        case value_t::table:
            m_value = std::make_shared<table_t>();
            break;
        case value_t::offset_datetime:
        case value_t::local_datetime:
        case value_t::local_date:
        case value_t::local_time:
            m_value = string_t();
            break;
        }
    };

    toml::toml(integer_t value) noexcept
        : m_value(value) {
          };

    toml::toml(number_t value) noexcept
        : m_value(value) {
          };

    toml::toml(const char *value)
        : m_value(string_t(value != nullptr ? value : "")) {
          };

    toml::toml(const string_t &value)
        : m_value(value) {
          };

    toml::toml(const string_t &&value)
        : m_value(std::move(value)) {
          };

    toml::toml(const array_t &value)
        : m_value(std::make_shared<array_t>(value)) {
          };

    toml::toml(const array_t &&value)
        : m_value(std::make_shared<array_t>(std::move(value))) {
          };

    toml::toml(const table_t &value)
        : m_value(std::make_shared<table_t>(value)) {
          };

    toml::toml(const table_t &&value)
        : m_value(std::make_shared<table_t>(std::move(value))) {
          };

    toml::toml(toml::initializer_list_t init, bool type_deduction, toml::value_t manual_type)
    {
        bool as_table = (manual_type == value_t::table);
        if (manual_type != value_t::array && manual_type != value_t::table)
        {
            as_table = false;
        }

        if (type_deduction && !as_table)
        {
            as_table = true;
            for (const auto &element : init)
            {
                if (!element.is_array() || element.size() != 2 || !element[static_cast<size_t>(0)].is_string())
                {
                    as_table = false;
                    break;
                }
            }
        }

        if (as_table)
        {
            table_t object;
            for (const auto &element : init)
            {
                object[element[static_cast<size_t>(0)].as_string()] = element[static_cast<size_t>(1)];
            }
            m_value = std::make_shared<table_t>(std::move(object));
            return;
        }

        array_t array;
        array.reserve(init.size());
        for (const auto &element : init)
        {
            array.push_back(element);
        }
        m_value = std::make_shared<array_t>(std::move(array));
    };

    toml::toml(const toml &other)
    {
        switch (other.type())
        {
        case value_t::null:
        case value_t::unknown:
            m_value = nullptr;
            break;
        case value_t::boolean:
            m_value = other.as_boolean();
            break;
        case value_t::integer:
            m_value = other.as_integer();
            break;
        case value_t::number:
        case value_t::floating:
            m_value = other.as_floating();
            break;
        case value_t::string:
        case value_t::offset_datetime:
        case value_t::local_datetime:
        case value_t::local_date:
        case value_t::local_time:
            m_value = other.as_string();
            break;
        case value_t::array:
            m_value = std::make_shared<array_t>(other.as_array());
            break;
        case value_t::table:
            m_value = std::make_shared<table_t>(other.as_table());
            break;
        case value_t::empty:
            m_value = string_t();
            break;
        }
    };

    toml::toml(toml &&other) noexcept = default;

    toml toml::table(initializer_list_t init)
    {
        return toml(init, false, value_t::table);
    };

    toml toml::array(initializer_list_t init)
    {
        return toml(init, false, value_t::array);
    };

    //========== Helper Methods ==========

    void toml::ensure_table()
    {
        if (!is_table())
            m_value = std::make_shared<table_t>();
    }

    void toml::ensure_array()
    {
        if (!is_array())
            m_value = std::make_shared<array_t>();
    }

    const toml::table_t &toml::get_table() const
    {
        if (!is_table())
            MYTOML_THROW(std::runtime_error("Cannot access as object"));
        return *std::get<std::shared_ptr<table_t>>(m_value);
    }

    const toml::array_t &toml::get_array() const
    {
        if (!is_array())
            MYTOML_THROW(std::runtime_error("Cannot access as array"));
        return *std::get<std::shared_ptr<array_t>>(m_value);
    }

    toml::table_t &toml::get_table()
    {
        if (!is_table())
            MYTOML_THROW(std::runtime_error("Cannot access as object"));
        return *std::get<std::shared_ptr<table_t>>(m_value);
    }

    toml::array_t &toml::get_array()
    {
        if (!is_array())
            MYTOML_THROW(std::runtime_error("Cannot access as array"));
        return *std::get<std::shared_ptr<array_t>>(m_value);
    }

    //========== Container Access (Objects) ==========

    toml &toml::at(const std::string &key) { return get_table().at(key); }
    const toml &toml::at(const std::string &key) const { return get_table().at(key); }

    toml &toml::operator[](const std::string &key)
    {
        ensure_table();
        return get_table()[key];
    }

    toml toml::operator[](const std::string &key) const
    {
        if (!is_table())
            return toml();
        auto &obj = get_table();
        auto it = obj.find(key);
        return it != obj.end() ? it->second : toml();
    }

    toml &toml::operator[](const char *key) { return operator[](std::string(key)); }
    toml toml::operator[](const char *key) const { return operator[](std::string(key)); }

    bool toml::contains(const std::string &key) const noexcept
    {
        if (!is_table())
            return false;
        return get_table().find(key) != get_table().end();
    }

    size_t toml::count(const std::string &key) const noexcept
    {
        if (!is_table())
            return 0;
        return get_table().count(key);
    }

    size_t toml::erase(const std::string &key) noexcept
    {
        if (!is_table())
            return 0;
        return get_table().erase(key);
    }

    //========== Container Access (Arrays) ==========

    toml &toml::at(size_t index) { return get_array().at(index); }
    const toml &toml::at(size_t index) const { return get_array().at(index); }

    toml &toml::operator[](size_t index)
    {
        ensure_array();
        auto &arr = get_array();
        if (index >= arr.size())
            arr.resize(index + 1);
        return arr[index];
    }

    const toml &toml::operator[](size_t index) const { return get_array().at(index); }

    toml &toml::front() { return get_array().front(); }
    const toml &toml::front() const { return get_array().front(); }
    toml &toml::back() { return get_array().back(); }
    const toml &toml::back() const { return get_array().back(); }

    void toml::push_back(const toml &value)
    {
        ensure_array();
        get_array().push_back(value);
    }

    void toml::push_back(toml &&value)
    {
        ensure_array();
        get_array().push_back(std::move(value));
    }

    void toml::push_front(const toml &value)
    {
        ensure_array();
        get_array().insert(get_array().begin(), value);
    }

    toml::iterator toml::insert(const const_iterator &pos, const toml &value)
    {
        ensure_array();
        const auto index = static_cast<size_type>(std::distance(cbegin(), pos));
        auto it = get_array().insert(get_array().cbegin() + static_cast<difference_type>(index), value);
        return iterator(it);
    }

    toml::iterator toml::insert(const const_iterator &pos, toml &&value)
    {
        ensure_array();
        const auto index = static_cast<size_type>(std::distance(cbegin(), pos));
        auto it = get_array().insert(get_array().cbegin() + static_cast<difference_type>(index), std::move(value));
        return iterator(it);
    }

    toml::iterator toml::erase(const_iterator pos)
    {
        if (!is_array())
        {
            return end();
        }

        const auto index = static_cast<size_type>(std::distance(cbegin(), pos));
        auto it = get_array().erase(get_array().cbegin() + static_cast<difference_type>(index));
        return iterator(it);
    }

    toml::iterator toml::erase(const_iterator first, const_iterator last)
    {
        if (!is_array())
        {
            return end();
        }

        const auto begin_index = static_cast<size_type>(std::distance(cbegin(), first));
        const auto end_index = static_cast<size_type>(std::distance(cbegin(), last));

        auto it = get_array().erase(
            get_array().cbegin() + static_cast<difference_type>(begin_index),
            get_array().cbegin() + static_cast<difference_type>(end_index));
        return iterator(it);
    }

    //========== Size and Capacity ==========

    size_t toml::size() const noexcept
    {
        if (is_table())
            return get_table().size();
        if (is_array())
            return get_array().size();
        return 0;
    }

    bool toml::empty() const noexcept
    {
        if (is_table())
            return get_table().empty();
        if (is_array())
            return get_array().empty();
        return size() == 0;
    }

    void toml::clear() noexcept
    {
        if (is_table())
            get_table().clear();
        else if (is_array())
            get_array().clear();
    }

    //========== Iteration ==========

    toml::iterator toml::begin()
    {
        if (is_table())
        {
            return iterator(get_table().begin());
        }
        if (is_array())
        {
            return iterator(get_array().begin());
        }
        return iterator(typename table_t::iterator{});
    }

    toml::const_iterator toml::begin() const { return cbegin(); }

    toml::const_iterator toml::cbegin() const
    {
        if (is_table())
        {
            return const_iterator(get_table().cbegin());
        }
        if (is_array())
        {
            return const_iterator(get_array().cbegin());
        }
        return const_iterator(typename table_t::const_iterator{});
    }

    toml::iterator toml::end()
    {
        if (is_table())
        {
            return iterator(get_table().end());
        }
        if (is_array())
        {
            return iterator(get_array().end());
        }
        return iterator(typename table_t::iterator{});
    }

    toml::const_iterator toml::end() const { return cend(); }
    toml::const_iterator toml::cend() const
    {
        if (is_table())
        {
            return const_iterator(get_table().cend());
        }
        if (is_array())
        {
            return const_iterator(get_array().cend());
        }
        return const_iterator(typename table_t::const_iterator{});
    }

    toml::reverse_iterator toml::rbegin() { return reverse_iterator(end()); }
    toml::const_reverse_iterator toml::rbegin() const { return crbegin(); };
    toml::const_reverse_iterator toml::crbegin() const { return const_reverse_iterator(cend()); }
    toml::reverse_iterator toml::rend() { return reverse_iterator(begin()); }
    toml::const_reverse_iterator toml::rend() const { return crend(); }
    toml::const_reverse_iterator toml::crend() const { return const_reverse_iterator(cbegin()); }

    //========== Comparison ==========

    bool toml::operator==(const toml &other) const noexcept
    {
        if (type() != other.type())
            return false;

        switch (type())
        {
        case value_t::null:
        case value_t::unknown:
            return true;
        case value_t::boolean:
            return as_boolean() == other.as_boolean();
        case value_t::integer:
            return as_integer() == other.as_integer();
        case value_t::number:
        case value_t::floating:
            return as_floating() == other.as_floating();
        case value_t::string:
        case value_t::offset_datetime:
        case value_t::local_datetime:
        case value_t::local_date:
        case value_t::local_time:
            return as_string() == other.as_string();
        case value_t::array:
            return get_array() == other.get_array();
        case value_t::table:
            return get_table() == other.get_table();
        case value_t::empty:
            return true;
        }

        return false;
    }
    bool toml::operator!=(const toml &other) const noexcept { return !(*this == other); }
    bool toml::operator<(const toml &other) const noexcept
    {
        if (type() != other.type())
            return static_cast<uint8_t>(type()) < static_cast<uint8_t>(other.type());

        switch (type())
        {
        case value_t::null:
            return false;
        case value_t::boolean:
            return as_boolean() < other.as_boolean();
        case value_t::integer:
            return as_integer() < other.as_integer();
        case value_t::number:
        case value_t::floating:
            return as_floating() < other.as_floating();
        case value_t::string:
        case value_t::offset_datetime:
        case value_t::local_datetime:
        case value_t::local_date:
        case value_t::local_time:
            return as_string() < other.as_string();
        case value_t::array:
            return get_array() < other.get_array();
        case value_t::table:
            return get_table() < other.get_table();
        default:
            return false;
        }
    }
    bool toml::operator<=(const toml &other) const noexcept { return (*this < other) || (*this == other); }
    bool toml::operator>(const toml &other) const noexcept { return other < *this; }
    bool toml::operator>=(const toml &other) const noexcept { return !(*this < other); }

    //========== Assignment ==========

    toml &toml::operator=(const toml &other)
    {
        if (this != &other)
        {
            *this = toml(other);
        }
        return *this;
    }
    toml &toml::operator=(toml &&other) noexcept = default;

    toml &toml::operator=(std::nullptr_t) noexcept
    {
        m_value = nullptr;
        return *this;
    }

    toml &toml::operator=(bool value) noexcept
    {
        m_value = value;
        return *this;
    }

    toml &toml::operator=(int value) noexcept
    {
        m_value = static_cast<integer_t>(value);
        return *this;
    }

    toml &toml::operator=(integer_t value) noexcept
    {
        m_value = value;
        return *this;
    }

    toml &toml::operator=(number_t value) noexcept
    {
        m_value = value;
        return *this;
    }

    toml &toml::operator=(const string_t &value)
    {
        m_value = value;
        return *this;
    }

    toml &toml::operator=(const char *value)
    {
        m_value = string_t(value != nullptr ? value : "");
        return *this;
    }

    toml &toml::operator=(const array_t &value)
    {
        m_value = std::make_shared<array_t>(value);
        return *this;
    }

    toml &toml::operator=(const table_t &value)
    {
        m_value = std::make_shared<table_t>(value);
        return *this;
    }

    toml &toml::operator=(initializer_list_t init)
    {
        *this = toml(init);
        return *this;
    }

    //========== Type Information ==========

    toml::value_t toml::type() const noexcept
    {
        switch (m_value.index())
        {
        case 0:
            return value_t::null;
        case 1:
            return value_t::boolean;
        case 2:
            return value_t::integer;
        case 3:
            return value_t::number;
        case 4:
            return value_t::string;
        case 5:
            return value_t::array;
        case 6:
            return value_t::table;
        default:
            return value_t::null;
        }
    }

    namespace
    {
        using namespace mytoml::detail;

        inline std::string ltrim(std::string s)
        {
            size_t i = 0;
            while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
                ++i;
            s.erase(0, i);
            return s;
        }

        inline std::string rtrim(std::string s)
        {
            size_t i = s.size();
            while (i > 0 && std::isspace(static_cast<unsigned char>(s[i - 1])))
                --i;
            s.erase(i);
            return s;
        }

        inline std::string trim(const std::string &s) { return rtrim(ltrim(s)); }

        toml parse_value(const std::string &text);

        std::vector<std::string> split_top_level(const std::string &text, char delim)
        {
            std::vector<std::string> parts;
            std::string current;
            int depth_brace = 0;
            int depth_bracket = 0;
            bool in_string = false;
            for (size_t i = 0; i < text.size(); ++i)
            {
                char c = text[i];
                if (c == '"' && (i == 0 || text[i - 1] != '\\'))
                    in_string = !in_string;
                if (!in_string)
                {
                    if (c == '{')
                        ++depth_brace;
                    else if (c == '}')
                        --depth_brace;
                    else if (c == '[')
                        ++depth_bracket;
                    else if (c == ']')
                        --depth_bracket;
                    else if (c == delim && depth_brace == 0 && depth_bracket == 0)
                    {
                        parts.push_back(trim(current));
                        current.clear();
                        continue;
                    }
                }
                current.push_back(c);
            }
            if (!current.empty())
                parts.push_back(trim(current));
            return parts;
        }

        toml parse_inline_table(const std::string &body)
        {
            toml::table_t table;
            auto parts = split_top_level(body, ',');
            for (const auto &entry : parts)
            {
                if (entry.empty())
                    continue;
                auto eq = entry.find('=');
                if (eq == std::string::npos)
                    throw parse_error("invalid inline table entry");
                auto key = trim(entry.substr(0, eq));
                auto value_text = trim(entry.substr(eq + 1));
                table[key] = parse_value(value_text);
            }
            return toml(table);
        }

        toml parse_array(const std::string &body)
        {
            toml::array_t arr;
            auto parts = split_top_level(body, ',');
            for (const auto &entry : parts)
            {
                if (entry.empty())
                    continue;
                arr.push_back(parse_value(entry));
            }
            return toml(arr);
        }

        toml parse_string(const std::string &text)
        {
            if (text.size() < 2 || text.front() != '"' || text.back() != '"')
                throw parse_error("invalid string literal");
            std::string out;
            for (size_t i = 1; i + 1 < text.size(); ++i)
            {
                char c = text[i];
                if (c == '\\' && i + 1 < text.size() - 1)
                {
                    char n = text[++i];
                    switch (n)
                    {
                    case 'n':
                        out.push_back('\n');
                        break;
                    case 't':
                        out.push_back('\t');
                        break;
                    case '"':
                        out.push_back('"');
                        break;
                    case '\\':
                        out.push_back('\\');
                        break;
                    default:
                        out.push_back(n);
                        break;
                    }
                }
                else
                {
                    out.push_back(c);
                }
            }
            return toml(out);
        }

        toml parse_value(const std::string &raw)
        {
            auto text = trim(raw);
            if (text.empty())
                throw parse_error("empty value");

            // String
            if (text.front() == '"')
                return parse_string(text);

            // Inline table
            if (text.front() == '{')
            {
                if (text.back() != '}')
                    throw parse_error("unterminated inline table");
                return parse_inline_table(text.substr(1, text.size() - 2));
            }

            // Array
            if (text.front() == '[')
            {
                if (text.back() != ']')
                    throw parse_error("unterminated array");
                return parse_array(text.substr(1, text.size() - 2));
            }

            // Boolean
            if (text == "true")
                return toml(true);
            if (text == "false")
                return toml(false);

            // Number detection
            bool has_dot = false;
            bool has_exp = false;
            size_t start = (text[0] == '+' || text[0] == '-') ? 1 : 0;
            for (size_t i = start; i < text.size(); ++i)
            {
                if (text[i] == '.')
                    has_dot = true;
                if (text[i] == 'e' || text[i] == 'E')
                    has_exp = true;
            }
            try
            {
                if (has_dot || has_exp)
                {
                    size_t consumed = 0;
                    double d = std::stod(text, &consumed);
                    if (consumed == text.size())
                        return toml(d);
                }
                else
                {
                    size_t consumed = 0;
                    int64_t v = std::stoll(text, &consumed);
                    if (consumed == text.size())
                        return toml(v);
                }
            }
            catch (...)
            {
                // Fallback: bare string
                return toml(text);
            }

            return toml(text);
        }

        void ensure_path(toml::table_t &root, const std::vector<std::string> &keys, size_t upto, toml::table_t **out)
        {
            toml::table_t *cur = &root;
            for (size_t i = 0; i < upto; ++i)
            {
                auto &slot = (*cur)[keys[i]];
                if (!slot.is_table())
                    slot = toml::table_t{};
                cur = &slot.as_table();
            }
            *out = cur;
        }
    }

    toml toml::parse(const std::string &text)
    {
        table_t root;
        table_t *current = &root;

        auto strip_comment = [](const std::string &line)
        {
            bool in_string = false;
            bool escaped = false;
            for (size_t i = 0; i < line.size(); ++i)
            {
                const char c = line[i];
                if (in_string)
                {
                    if (escaped)
                    {
                        escaped = false;
                    }
                    else if (c == '\\')
                    {
                        escaped = true;
                    }
                    else if (c == '"')
                    {
                        in_string = false;
                    }
                    continue;
                }

                if (c == '"')
                {
                    in_string = true;
                    continue;
                }

                if (c == '#')
                {
                    return line.substr(0, i);
                }
            }
            return line;
        };

        auto find_top_level_equals = [](const std::string &line) -> std::string::size_type
        {
            bool in_string = false;
            bool escaped = false;
            int depth_brace = 0;
            int depth_bracket = 0;
            for (std::string::size_type i = 0; i < line.size(); ++i)
            {
                const char c = line[i];
                if (in_string)
                {
                    if (escaped)
                    {
                        escaped = false;
                    }
                    else if (c == '\\')
                    {
                        escaped = true;
                    }
                    else if (c == '"')
                    {
                        in_string = false;
                    }
                    continue;
                }

                if (c == '"')
                {
                    in_string = true;
                    continue;
                }
                if (c == '{')
                {
                    ++depth_brace;
                    continue;
                }
                if (c == '}')
                {
                    --depth_brace;
                    continue;
                }
                if (c == '[')
                {
                    ++depth_bracket;
                    continue;
                }
                if (c == ']')
                {
                    --depth_bracket;
                    continue;
                }
                if (c == '=' && depth_brace == 0 && depth_bracket == 0)
                {
                    return i;
                }
            }
            return std::string::npos;
        };

        std::istringstream input(text);
        std::string line;
        while (std::getline(input, line))
        {
            const std::string cleaned = trim(strip_comment(line));
            if (cleaned.empty())
                continue;

            if (cleaned.size() >= 4 && cleaned.front() == '[' && cleaned[1] == '[' && cleaned[cleaned.size() - 2] == ']' && cleaned.back() == ']')
            {
                const std::string body = cleaned.substr(2, cleaned.size() - 4);
                const auto parts = split_top_level(body, '.');
                if (parts.empty())
                    throw parse_error("invalid array-of-tables header");

                table_t *target = nullptr;
                ensure_path(root, parts, parts.size() - 1, &target);
                toml &slot = (*target)[parts.back()];
                slot.ensure_array();
                slot.as_array().emplace_back(table_t{});
                current = &slot.as_array().back().as_table();
                continue;
            }

            if (cleaned.front() == '[' && cleaned.back() == ']')
            {
                const std::string body = cleaned.substr(1, cleaned.size() - 2);
                const auto parts = split_top_level(body, '.');
                if (parts.empty())
                    throw parse_error("invalid table header");

                ensure_path(root, parts, parts.size(), &current);
                continue;
            }

            const auto eq = find_top_level_equals(cleaned);
            if (eq == std::string::npos)
                throw parse_error("expected '=' in assignment");

            const std::string key_part = trim(cleaned.substr(0, eq));
            const std::string value_part = trim(cleaned.substr(eq + 1));
            const auto keys = split_top_level(key_part, '.');
            if (keys.empty())
                throw parse_error("invalid key");

            table_t *target = nullptr;
            if (keys.size() > 1)
            {
                ensure_path(*current, keys, keys.size() - 1, &target);
            }
            else
            {
                target = current;
            }

            (*target)[keys.back()] = parse_value(value_part);
        }

        return toml(root);
    }

    toml toml::parse(FILE *file)
    {
        if (file == nullptr)
            MYTOML_THROW(parse_error("Null file pointer passed to parse"));

        std::ostringstream buffer;
        char chunk[4096];
        while (true)
        {
            const size_t n = std::fread(chunk, 1, sizeof(chunk), file);
            if (n > 0)
                buffer.write(chunk, static_cast<std::streamsize>(n));
            if (n < sizeof(chunk))
            {
                if (std::feof(file) != 0)
                    break;
                if (std::ferror(file) != 0)
                    MYTOML_THROW(parse_error("Failed reading file in parse"));
            }
        }
        return parse(buffer.str());
    }

    toml toml::parse(const char *str)
    {
        if (str == nullptr)
            MYTOML_THROW(parse_error("Null pointer passed to parse"));

        return parse(string_t(str));
    }

#ifndef MYTOML_NO_STL
    toml toml::parse(std::istream &stream)
    {
        std::ostringstream buffer;
        buffer << stream.rdbuf();
        return parse(buffer.str());
    }
#endif // MYTOML_NO_STL

    toml toml::parse(detail::iadapter &adapter)
    {
        std::string input;
        char chunk[4096];
        while (true)
        {
            const size_t n = adapter.read(chunk, sizeof(chunk));
            if (n == 0)
                break;
            input.append(chunk, n);
            if (n < sizeof(chunk))
                break;
        }
        return parse(input);
    }

    bool toml::is_table() const noexcept { return type() == node_t::table; }
    bool toml::is_array() const noexcept { return type() == node_t::array; }
    bool toml::is_string() const noexcept { return type() == node_t::string; }
    bool toml::is_integer() const noexcept { return type() == node_t::integer; }
    bool toml::is_floating() const noexcept { return type() == node_t::number; }
    bool toml::is_boolean() const noexcept { return type() == node_t::boolean; }
    bool toml::is_local_time() const noexcept { return false; }
    bool toml::is_local_date() const noexcept { return false; }
    bool toml::is_local_datetime() const noexcept { return false; }
    bool toml::is_offset_datetime() const noexcept { return false; }

    toml::table_t &toml::as_table() { return *std::get<std::shared_ptr<table_t>>(m_value); }
    const toml::table_t &toml::as_table() const { return *std::get<std::shared_ptr<table_t>>(m_value); }
    toml::array_t &toml::as_array() { return *std::get<std::shared_ptr<array_t>>(m_value); }
    const toml::array_t &toml::as_array() const { return *std::get<std::shared_ptr<array_t>>(m_value); }
    std::string &toml::as_string() { return std::get<std::string>(m_value); }
    const std::string &toml::as_string() const { return std::get<std::string>(m_value); }
    toml::integer_type &toml::as_integer() { return std::get<integer_t>(m_value); }
    const toml::integer_type &toml::as_integer() const { return std::get<integer_t>(m_value); }
    toml::boolean_type &toml::as_boolean() { return std::get<boolean_t>(m_value); }
    const toml::boolean_type &toml::as_boolean() const { return std::get<boolean_t>(m_value); }
    toml::floating_type &toml::as_floating() { return std::get<number_t>(m_value); }
    const toml::floating_type &toml::as_floating() const
    {
        return std::get<number_t>(m_value);
    }
    toml::local_time_type &toml::as_local_time() { MYTOML_THROW(parse_error("local_time is not stored in this build")); }
    const toml::local_time_type &toml::as_local_time() const { MYTOML_THROW(parse_error("local_time is not stored in this build")); }
    toml::local_date_type &toml::as_local_date() { MYTOML_THROW(parse_error("local_date is not stored in this build")); }
    const toml::local_date_type &toml::as_local_date() const { MYTOML_THROW(parse_error("local_date is not stored in this build")); }
    toml::local_datetime_type &toml::as_local_datetime() { MYTOML_THROW(parse_error("local_datetime is not stored in this build")); }
    const toml::local_datetime_type &toml::as_local_datetime() const { MYTOML_THROW(parse_error("local_datetime is not stored in this build")); }
    toml::offset_datetime_type &toml::as_offset_datetime() { MYTOML_THROW(parse_error("offset_datetime is not stored in this build")); }
    const toml::offset_datetime_type &toml::as_offset_datetime() const { MYTOML_THROW(parse_error("offset_datetime is not stored in this build")); }

    //========== Serialization ==========

    namespace
    {
        void dump_value(const toml &node, std::ostream &out);

        void dump_table(const toml::table_t &table, const std::string &prefix, std::ostream &out)
        {
            if (!prefix.empty())
            {
                out << "[" << prefix << "]\n";
            }

            // Scalars first
            for (const auto &kv : table)
            {
                if (kv.second.is_table())
                    continue;
                out << kv.first << " = ";
                dump_value(kv.second, out);
                out << "\n";
            }
            // Nested tables
            for (const auto &kv : table)
            {
                if (!kv.second.is_table())
                    continue;
                std::string header = prefix.empty() ? kv.first : prefix + "." + kv.first;
                dump_table(kv.second.as_table(), header, out);
            }
        }

        void dump_value(const toml &node, std::ostream &out)
        {
            switch (node.type())
            {
            case value_t::string:
                out << '"' << node.as_string() << '"';
                break;
            case value_t::integer:
                out << node.as_integer();
                break;
            case value_t::number:
                out << node.as_floating();
                break;
            case value_t::boolean:
                out << (node.as_boolean() ? "true" : "false");
                break;
            case value_t::array:
            {
                out << "[";
                const auto &arr = node.as_array();
                for (size_t i = 0; i < arr.size(); ++i)
                {
                    if (i)
                        out << ", ";
                    dump_value(arr[i], out);
                }
                out << "]";
                break;
            }
            case value_t::table:
            {
                out << "{ ";
                const auto &tbl = node.as_table();
                size_t idx = 0;
                for (const auto &kv : tbl)
                {
                    if (idx++)
                        out << ", ";
                    out << kv.first << " = ";
                    dump_value(kv.second, out);
                }
                out << " }";
                break;
            }
            default:
                out << "null";
                break;
            }
        }
    };

    std::string toml::dump(int /*indent*/) const
    {
        std::ostringstream out;
        if (is_table())
        {
            dump_table(as_table(), "", out);
        }
        else
        {
            dump_value(*this, out);
        }
        return out.str();
    };

    void toml::dump(FILE *file)
    {
        if (file == nullptr)
            MYTOML_THROW(parse_error("Null file pointer passed to dump"));

        const auto text = dump();
        const auto written = std::fwrite(text.data(), 1, text.size(), file);
        if (written != text.size())
            MYTOML_THROW(parse_error("Failed writing file in dump"));
    };

    void toml::dump(const char *str)
    {
        if (str == nullptr)
            MYTOML_THROW(parse_error("Null path passed to dump"));

        FILE *file = std::fopen(str, "wb");
        if (file == nullptr)
            MYTOML_THROW(parse_error("Failed opening output file in dump"));

        MYTOML_TRY
        {
            dump(file);
            std::fclose(file);
        }
        MYTOML_CATCH(...)
        {
            std::fclose(file);
            MYTOML_THROW(parse_error("Failed writing output file in dump"));
        }
    };

    void toml::dump(const string_t &str)
    {
        dump(str.c_str());
    };

#ifndef MYTOML_NO_STL
    void toml::dump(std::ostream &stream)
    {
        stream << dump();
    };
#endif // MYTOML_NO_STL

    void toml::dump(detail::oadapter &adapter)
    {
        const auto text = dump();
        adapter.write(text.data(), text.size());
    };

    toml::~toml() noexcept = default;

    //-----------------------------------------------------------------------------
    // [SECTION] Mytoml : Functions
    //-----------------------------------------------------------------------------
    // - operator<<()
    // - operator>>()
    //-----------------------------------------------------------------------------

#ifndef MYTOML_NO_STL

    const char *string(encoding type)
    {
        switch (type)
        {
        case encoding::utf8:
            return "utf8";
        case encoding::utf16:
            return "utf16";
        case encoding::utf16le:
            return "utf16le";
        case encoding::utf16be:
            return "utf16be";
        case encoding::utf32:
            return "utf32";
        case encoding::utf32le:
            return "utf32le";
        case encoding::utf32be:
            return "utf32be";
        default:
            return "unspecified";
        }
    };

    const char *string(node_t type)
    {
        switch (type)
        {
        case node_t::unknown:
            return "unknown";
        case node_t::table:
            return "table";
        case node_t::array:
            return "array";
        case node_t::string:
            return "string";
        case node_t::integer:
            return "integer";
        case node_t::floating:
            return "floating";
        case node_t::boolean:
            return "boolean";
        default:
            return "unknown";
        }
    };

    std::ostream &operator<<(std::ostream &stream, const encoding &type)
    {
        stream << string(type);
        return stream;
    };

    std::ostream &operator<<(std::ostream &stream, const node_t &type)
    {
        stream << string(type);
        return stream;
    };

    std::ostream &operator<<(std::ostream &stream, const toml &node)
    {
        // detail::stream_oadapter adapter(ostream);
        // detail::serializer output(&adapter);
        // output.serialize(node, -1);

        stream << node.dump();
        return stream;
    };

    std::istream &operator>>(std::istream &stream, toml &node)
    {
        node = toml::parse(stream);
        return stream;
    };

#endif // MYTOML_NO_STL

    MYTOML_VERSION_NAMESPACE_END

}; // namespace mytoml

#pragma endregion // Mytoml

#pragma region Literal

namespace mytoml
{

    MYTOML_VERSION_NAMESPACE_BEGIN

    namespace literals
    {

        inline namespace toml_literals
        {

            //-----------------------------------------------------------------------------
            // [SECTION] Literals : Toml
            //-----------------------------------------------------------------------------

            toml MYTOML_QUOTE_OPERATOR(const char *string, size_t size)
            {
                return toml::parse(std::string(string, size));
            };

#if MYTOML_HAS_CHAR8_T

            toml MYTOML_QUOTE_OPERATOR(const char8_t *string, size_t size)
            {
                return toml::parse(std::string(reinterpret_cast<const char *>(string), size));
            };

#endif // MYTOML_HAS_CHAR8_T

            toml MYTOML_QUOTE_OPERATOR(const char16_t *string, size_t size)
            {
                std::u16string u16(string, size);
                std::string utf8;
                utf8.reserve(u16.size());
                for (char16_t c : u16)
                    utf8.push_back(static_cast<char>(c));
                return toml::parse(utf8);
            };

            toml MYTOML_QUOTE_OPERATOR(const char32_t *string, size_t size)
            {
                std::u32string u32(string, size);
                std::string utf8;
                utf8.reserve(u32.size());
                for (char32_t c : u32)
                    utf8.push_back(static_cast<char>(c));
                return toml::parse(utf8);
            };

        }; // namespace toml_literals
    }; // namespace literals

    MYTOML_VERSION_NAMESPACE_END

}; // namespace mytoml

#pragma endregion // Literal

#endif // MYTOML_IMPLEMENTATION

// NOLINTEND

/**
 * CHANGELOG: Version history
 */

/**
 * LICENSE: MIT License
 *
 * Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * data the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included data
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */