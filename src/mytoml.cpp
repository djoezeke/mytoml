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

        encoding_error::encoding_error(encoding encoding, const char *message, void *data, size_t size) noexcept
            : exception(generate(encoding, message, data, size)) {};

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

        bool lexer::scan_literal()
        {
            std::string string;
            if ((m_char >= 'a' && m_char <= 'z') || (m_char >= 'A' && m_char <= 'Z'))
            {
                // string.push_back(static_cast<char>(m_char));
                while (true)
                {
                    advance();
                    if (m_char == EOF)
                        break;
                    if ((m_char >= 'a' && m_char <= 'z') || (m_char >= 'A' && m_char <= 'Z'))
                    {
                        string.push_back(static_cast<char>(m_char));
                        continue;
                    }
                    reverse();
                    break;
                }
                if (string == "false")
                {
                    m_token.type = token_t::false_literal;
                    m_token.text = string;
                    return true;
                }
                if (string == "true")
                {
                    m_token.type = token_t::true_literal;
                    m_token.text = string;
                    return true;
                }
                if (string == "null")
                {
                    m_token.type = token_t::null_literal;
                    return true;
                }
            }
            return false;
        };

        bool lexer::scan_comment()
        {
            std::string string;
            switch (m_char)
            {
            case '/':
            {
                while (true)
                {
                    advance();
                    switch (m_char)
                    {
                    case '\n':
                    case '\r':
                    case '\0':
                        return true;
                    default:
                        break;
                    }
                }
            }

            case '*':
            {
                while (true)
                {
                    advance();
                    switch (m_char)
                    {
                    case '\0':
                    {
                        return false;
                    }

                    case '*':
                    {
                        advance();
                        switch (m_char)
                        {
                        case '/':
                            return true;
                        default:
                        {
                            reverse();
                            continue;
                        }
                        }
                    }
                    default:
                        continue;
                    }
                }
            }
            default:
            {
                return false;
            }
            }
        };

        bool lexer::scan_string()
        {
            std::string string;
            while (true)
            {
                advance();
                if (m_char == EOF)
                {
                    m_token.type = token_t::string_value;
                    return true;
                }
                if (m_char == '\\')
                {
                    advance();
                    if (m_char == EOF)
                        break;
                    string.push_back(static_cast<char>('\\'));
                    string.push_back(static_cast<char>(m_char));
                    continue;
                }
                if (m_char == '"')
                {
                    m_token.type = token_t::string_value;
                    m_token.text = string;
                    return true;
                }
                string.push_back(static_cast<char>(m_char));
            }
            return false;
        };

        bool lexer::scan_number()
        {
            std::string string;
            if ((m_char >= '0' && m_char <= '9') || m_char == '-')
            {
                string.push_back(static_cast<char>(m_char));
                while (true)
                {
                    advance();
                    if (m_char == EOF)
                        break;
                    if ((m_char >= '0' && m_char <= '9') || m_char == '+' || m_char == '-' || m_char == 'e' || m_char == 'E' || m_char == '.')
                    {
                        string.push_back(static_cast<char>(m_char));
                        continue;
                    }
                    reverse();
                    break;
                }
                m_token.type = token_t::number_value;
                m_token.text = string;
                return true;
            }
            return false;
        };

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

            int c = skip_ws();
            if (c == EOF)
            {
                m_token.type = token_t::end_of_input;
                return m_token;
            }

            c = get_char();

            switch (c)
            {
            case '{':
                m_token.type = token_t::object_start;
                return m_token;
            case '}':
                m_token.type = token_t::object_end;
                return m_token;
            case '[':
                m_token.type = token_t::array_start;
                return m_token;
            case ']':
                m_token.type = token_t::array_end;
                return m_token;
            case ',':
                m_token.type = token_t::value_separator;
                return m_token;
            case ':':
                m_token.type = token_t::name_separator;
                return m_token;
            case '"':
            {
                // parse string
                while (true)
                {
                    int ch = get_char();
                    if (ch == EOF)
                    {
                        m_token.type = token_t::string_value;
                        return m_token;
                    }
                    if (ch == '\\')
                    {
                        int esc = get_char();
                        if (esc == EOF)
                            break;
                        // handle simple escapes; for brevity we append raw
                        // sequences; a complete implementation would decode
                        // unicode escapes.
                        add_char('\\');
                        add_char(esc);
                        continue;
                    }
                    if (ch == '"')
                    {
                        m_token.type = token_t::string_value;
                        m_token.text = m_string;
                        return m_token;
                    }
                    add_char(ch);
                }
                break;
            }
            default:
                break;
            }

            // numbers, literals (true,false,null)
            if ((c >= '0' && c <= '9') || c == '-')
            {
                // number
                m_string.push_back(static_cast<char>(c));
                while (true)
                {
                    int ch = get_char();
                    if (ch == EOF)
                        break;
                    if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == 'e' || ch == 'E' || ch == '.')
                    {
                        m_string.push_back(static_cast<char>(ch));
                        continue;
                    }
                    unget_char();
                    break;
                }
                m_token.type = token_t::number_value;
                m_token.text = m_string;
                return m_token;
            }

            // true, false, null
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            {
                m_string.push_back(static_cast<char>(c));
                while (true)
                {
                    int ch = get_char();
                    if (ch == EOF)
                        break;
                    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
                    {
                        m_string.push_back(static_cast<char>(ch));
                        continue;
                    }
                    unget_char();
                    break;
                }

                if (m_string == "false")
                {
                    m_token.type = token_t::false_literal;
                    m_token.text = m_string;
                    return m_token;
                }

                if (m_string == "true")
                {
                    m_token.type = token_t::true_literal;
                    m_token.text = m_string;
                    return m_token;
                }

                if (m_string == "null")
                {
                    m_token.type = token_t::null_literal;
                    return m_token;
                }
            }

            // unknown single char
            m_token.type = token_t::unknown;
            return m_token;
        }

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

        const char *string(mark type) { return ""; };

        const char *string(event type) { return ""; };

        const char *string(token type) { return ""; };

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

    version::version(int major, int minor, int patch)
        : major(major), minor(minor), patch(patch)
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
        : m_value(nullptr)
    {
    }

    toml::toml(bool value) noexcept
        : m_value(value)
    {
    }

    toml::toml(int value) noexcept
        : m_value(static_cast<integer_t>(value))
    {
    }

    toml::toml(value_t value) noexcept
    {
        switch (value)
        {
        case value_t::null:
            m_value = nullptr;
            break;
        case value_t::boolean:
            m_value = false;
            break;
        case value_t::integer:
            m_value = static_cast<integer_t>(0);
            break;
        case value_t::number:
            m_value = static_cast<number_t>(0.0);
            break;
        case value_t::string:
            m_value = string_t();
            break;
        case value_t::array:
            m_value = array_t();
            break;
        case value_t::table:
            m_value = table_t();
            break;
        }
    }

    toml::toml(integer_t value) noexcept
        : m_value(value)
    {
    }

    toml::toml(number_t value) noexcept
        : m_value(value)
    {
    }

    toml::toml(const char *value)
        : m_value(string_t(value != nullptr ? value : ""))
    {
    }

    toml::toml(const string_t &value)
        : m_value(value)
    {
    }

    toml::toml(const string_t &&value)
        : m_value(std::move(value))
    {
    }

    toml::toml(const array_t &value)
        : m_value(value)
    {
    }

    toml::toml(const array_t &&value)
        : m_value(std::move(value))
    {
    }

    toml::toml(const table_t &value)
        : m_value(value)
    {
    }

    toml::toml(const table_t &&value)
        : m_value(std::move(value))
    {
    }

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
            m_value = std::move(object);
            return;
        }

        array_t array;
        array.reserve(init.size());
        for (const auto &element : init)
        {
            array.push_back(element);
        }
        m_value = std::move(array);
    }

    toml::toml(const toml &other) = default;
    toml::toml(toml &&other) noexcept = default;

    toml toml::table(initializer_list_t init)
    {
        return toml(init, false, value_t::table);
    }

    toml toml::array(initializer_list_t init)
    {
        return toml(init, false, value_t::array);
    }

    //========== Helper Methods ==========

    void toml::ensure_table()
    {
        if (!is_table())
            m_value = table_t();
    }

    void toml::ensure_array()
    {
        if (!is_array())
            m_value = array_t();
    }

    const toml::table_t &toml::get_table() const
    {
        if (!is_table())
            MYTOML_THROW(std::runtime_error("Cannot access as object"));
        return std::get<table_t>(m_value);
    }

    const toml::array_t &toml::get_array() const
    {
        if (!is_array())
            MYTOML_THROW(std::runtime_error("Cannot access as array"));
        return std::get<array_t>(m_value);
    }

    toml::table_t &toml::get_table()
    {
        if (!is_table())
            MYTOML_THROW(std::runtime_error("Cannot access as object"));
        return std::get<table_t>(m_value);
    }

    toml::array_t &toml::get_array()
    {
        if (!is_array())
            MYTOML_THROW(std::runtime_error("Cannot access as array"));
        return std::get<array_t>(m_value);
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

    //========== Assignment ==========

    toml &toml::operator=(const toml &other) = default;
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
        m_value = value;
        return *this;
    }

    toml &toml::operator=(const table_t &value)
    {
        m_value = value;
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
            return toml{table};
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
            return toml{arr};
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
            return toml{out};
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
                return toml{true};
            if (text == "false")
                return toml{false};

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
                    double d = std::stod(text);
                    return toml{d};
                }
                int64_t v = std::stoll(text);
                return toml{v};
            }
            catch (...)
            {
                // Fallback: bare string
                return toml{text};
            }
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

        std::istringstream in(text);
        std::string line;
        while (std::getline(in, line))
        {
            auto trimmed = trim(line);
            if (trimmed.empty())
                continue;
            if (trimmed[0] == '#')
                continue;

            if (trimmed.front() == '[' && trimmed.back() == ']')
            {
                // Table header
                auto body = trimmed.substr(1, trimmed.size() - 2);
                auto parts = split_top_level(body, '.');
                ensure_path(root, parts, parts.size(), &current);
                continue;
            }

            auto eq = trimmed.find('=');
            if (eq == std::string::npos)
                throw parse_error("expected '=' in assignment");

            auto key_part = trim(trimmed.substr(0, eq));
            auto value_part = trimmed.substr(eq + 1);
            auto keys = split_top_level(key_part, '.');

            table_t *target = nullptr;
            if (keys.size() > 1)
            {
                ensure_path(*current, keys, keys.size() - 1, &target);
            }
            else
            {
                target = current;
            }

            auto key = keys.back();
            (*target)[key] = parse_value(value_part);
        }

        return toml{root};
    }

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

    bool toml::is_table() const noexcept { return type() == node_t::table; }
    bool toml::is_array() const noexcept { return type() == node_t::array; }
    bool toml::is_string() const noexcept { return type() == node_t::string; }
    bool toml::is_integer() const noexcept { return type() == node_t::integer; }
    bool toml::is_floating() const noexcept { return type() == node_t::number; }
    bool toml::is_boolean() const noexcept { return type() == node_t::boolean; }

    toml::table_t &toml::as_table() { return std::get<table_t>(m_value); }
    const toml::table_t &toml::as_table() const { return std::get<table_t>(m_value); }
    toml::array_t &toml::as_array() { return std::get<array_t>(m_value); }
    const toml::array_t &toml::as_array() const { return std::get<array_t>(m_value); }
    std::string &toml::as_string() { return std::get<std::string>(m_value); }
    const std::string &toml::as_string() const { return std::get<std::string>(m_value); }
    int64_t toml::as_integer() const { return std::get<int64_t>(m_value); }
    double toml::as_floating() const
    {
        if (std::holds_alternative<double>(m_value))
            return std::get<double>(m_value);
        return static_cast<double>(std::get<int64_t>(m_value));
    }
    bool toml::as_boolean() const { return std::get<bool>(m_value); }

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
        return "";
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
        // node = toml::parse(stream);

        std::ostringstream buffer;
        buffer << stream.rdbuf();
        const_cast<toml &>(node) = toml::parse(buffer.str());
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