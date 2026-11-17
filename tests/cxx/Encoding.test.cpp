#include <catch2/catch_test_macros.hpp>
#include <mytoml/mytoml.hpp>

#include <cstdint>
#include <string>
#include <vector>

TEST_CASE("Encoding detection recognises byte-order marks", "[encoding]")
{
    SECTION("insufficient data")
    {
        CHECK(mytoml::detail::determine_encoding(nullptr, 0) == mytoml::encoding::unspecified);
        const char data[] = {'x'};
        CHECK(mytoml::detail::determine_encoding(const_cast<char *>(data), 1) ==
              mytoml::encoding::unspecified);
    }

    SECTION("UTF-8")
    {
        char bom[] = {'\xEF', '\xBB', '\xBF'};
        CHECK(mytoml::detail::determine_encoding(bom, sizeof(bom)) == mytoml::encoding::utf8);

        char short_bom[] = {'\xEF', '\xBB'};
        CHECK(mytoml::detail::determine_encoding(short_bom, sizeof(short_bom)) ==
              mytoml::encoding::unspecified);
    }

    SECTION("UTF-16")
    {
        char little[] = {'\xFF', '\xFE'};
        CHECK(mytoml::detail::determine_encoding(little, sizeof(little)) == mytoml::encoding::utf16le);

        char big[] = {'\xFE', '\xFF'};
        CHECK(mytoml::detail::determine_encoding(big, sizeof(big)) == mytoml::encoding::utf16be);
    }

    SECTION("UTF-32")
    {
        char little[] = {'\xFF', '\xFE', '\x00', '\x00'};
        CHECK(mytoml::detail::determine_encoding(little, sizeof(little)) == mytoml::encoding::utf32le);

        char big[] = {'\x00', '\x00', '\xFE', '\xFF'};
        CHECK(mytoml::detail::determine_encoding(big, sizeof(big)) == mytoml::encoding::utf32be);
    }

    SECTION("plain text")
    {
        char text[] = "plain text";
        CHECK(mytoml::detail::determine_encoding(text, sizeof(text)) == mytoml::encoding::unspecified);
    }
}

TEST_CASE("UTF-8 decoding", "[encoding]")
{
    unsigned int codepoint = 0;

    SECTION("ASCII")
    {
        const char data[] = {'A'};
        CHECK(mytoml::detail::utf8::decode(data, sizeof(data), codepoint) == 1);
        CHECK(codepoint == 0x41);
    }

    SECTION("two-byte code point")
    {
        const char data[] = {'\xC2', '\xA9'};
        CHECK(mytoml::detail::utf8::decode(data, sizeof(data), codepoint) == 2);
        CHECK(codepoint == 0xA9);
    }

    SECTION("three-byte code point")
    {
        const char data[] = {'\xE2', '\x82', '\xAC'};
        CHECK(mytoml::detail::utf8::decode(data, sizeof(data), codepoint) == 3);
        CHECK(codepoint == 0x20AC);
    }

    SECTION("four-byte code point")
    {
        const char data[] = {'\xF0', '\x9F', '\x98', '\x80'};
        CHECK(mytoml::detail::utf8::decode(data, sizeof(data), codepoint) == 4);
        CHECK(codepoint == 0x1F600);
    }

    SECTION("invalid input")
    {
        const char invalid[] = {'\x80'};
        CHECK(mytoml::detail::utf8::decode(invalid, sizeof(invalid), codepoint) == -1);
        CHECK(codepoint == 0);

        const char truncated[] = {'\xE2', '\x82'};
        CHECK(mytoml::detail::utf8::decode(truncated, sizeof(truncated), codepoint) == -1);

        CHECK(mytoml::detail::utf8::decode(nullptr, 0, codepoint) == -1);
    }
}

TEST_CASE("UTF-8 encoding", "[encoding]")
{
    char output[5] = {};

    CHECK(mytoml::detail::utf8::encode(0x41, output, sizeof(output)) == 1);
    CHECK(output[0] == 'A');

    CHECK(mytoml::detail::utf8::encode(0xA9, output, sizeof(output)) == 2);
    CHECK(output[0] == '\xC2');
    CHECK(output[1] == '\xA9');

    CHECK(mytoml::detail::utf8::encode(0x20AC, output, sizeof(output)) == 3);
    CHECK(std::string(output, 3) == "\xE2\x82\xAC");

    CHECK(mytoml::detail::utf8::encode(0x1F600, output, sizeof(output)) == 4);
    CHECK(std::string(output, 4) == "\xF0\x9F\x98\x80");

    CHECK(mytoml::detail::utf8::encode(0x41, output, 0) == -1);
    CHECK(mytoml::detail::utf8::encode(0x20AC, output, 2) == -1);
    CHECK(mytoml::detail::utf8::encode(0x110000, output, sizeof(output)) == -1);
}

TEST_CASE("UTF-8 converts to UTF-16 and UTF-32", "[encoding]")
{
    SECTION("ASCII to UTF-16")
    {
        const auto bytes = mytoml::detail::utf8::to_utf16("A");
        REQUIRE(bytes.size() == 2);

        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf16::decode(reinterpret_cast<const char *>(bytes.data()),
                                            bytes.size(), codepoint) == 2);
        CHECK(codepoint == 0x41);
    }

    SECTION("supplementary code point to UTF-16")
    {
        const auto bytes = mytoml::detail::utf8::to_utf16("\xF0\x9F\x98\x80");
        REQUIRE(bytes.size() == 4);

        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf16::decode(reinterpret_cast<const char *>(bytes.data()),
                                            bytes.size(), codepoint) == 4);
        CHECK(codepoint == 0x1F600);
    }

    SECTION("ASCII to UTF-32")
    {
        const auto bytes = mytoml::detail::utf8::to_utf32("A");
        REQUIRE(bytes.size() == 4);

        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf32::decode(reinterpret_cast<const char *>(bytes.data()),
                                            bytes.size(), codepoint) == 4);
        CHECK(codepoint == 0x41);
    }

    SECTION("supplementary code point to UTF-32")
    {
        const auto bytes = mytoml::detail::utf8::to_utf32("\xF0\x9F\x98\x80");
        REQUIRE(bytes.size() == 4);

        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf32::decode(reinterpret_cast<const char *>(bytes.data()),
                                            bytes.size(), codepoint) == 4);
        CHECK(codepoint == 0x1F600);
    }
}

TEST_CASE("UTF-16 decoding and encoding", "[encoding]")
{
    SECTION("little-endian code unit")
    {
        const char data[] = {'\x41', '\x00'};
        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf16::decode(data, sizeof(data), codepoint,
                                            mytoml::detail::endian::little) == 2);
        CHECK(codepoint == 0x41);
    }

    SECTION("big-endian code unit")
    {
        const char data[] = {'\x00', '\x41'};
        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf16::decode(data, sizeof(data), codepoint,
                                            mytoml::detail::endian::big) == 2);
        CHECK(codepoint == 0x41);
    }

    SECTION("surrogate pair")
    {
        const char little[] = {'\x3D', '\xD8', '\x00', '\xDE'};
        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf16::decode(little, sizeof(little), codepoint,
                                            mytoml::detail::endian::little) == 4);
        CHECK(codepoint == 0x1F600);

        const char big[] = {'\xD8', '\x3D', '\xDE', '\x00'};
        codepoint = 0;
        CHECK(mytoml::detail::utf16::decode(big, sizeof(big), codepoint,
                                            mytoml::detail::endian::big) == 4);
        CHECK(codepoint == 0x1F600);
    }

    SECTION("invalid input")
    {
        unsigned int codepoint = 0;
        const char invalid_low[] = {'\x3D', '\xD8', '\x00', '\x00'};
        CHECK(mytoml::detail::utf16::decode(invalid_low, sizeof(invalid_low), codepoint,
                                            mytoml::detail::endian::little) == -1);

        const char short_input[] = {'\x41'};
        CHECK(mytoml::detail::utf16::decode(short_input, sizeof(short_input), codepoint,
                                            mytoml::detail::endian::little) == -1);
        CHECK(mytoml::detail::utf16::decode(nullptr, 2, codepoint, mytoml::detail::endian::little) == -1);
    }

    SECTION("encode BMP and supplementary code points")
    {
        mytoml::detail::utf16::char_t output[2] = {};

        CHECK(mytoml::detail::utf16::encode(0x41, output, 2,
                                            mytoml::detail::endian::little) > 0);
        const char *bytes = reinterpret_cast<const char *>(output);
        CHECK(bytes[0] == '\x41');
        CHECK(bytes[1] == '\x00');

        CHECK(mytoml::detail::utf16::encode(0x1F600, output, 4,
                                            mytoml::detail::endian::little) == 2);
        CHECK(bytes[0] == '\x3D');
        CHECK(bytes[1] == '\xD8');
        CHECK(bytes[2] == '\x00');
        CHECK(bytes[3] == '\xDE');
    }

    SECTION("encode rejects invalid arguments")
    {
        mytoml::detail::utf16::char_t output[2] = {};
        CHECK(mytoml::detail::utf16::encode(0x41, output, 1,
                                            mytoml::detail::endian::little) == -1);
        CHECK(mytoml::detail::utf16::encode(0x1F600, output, 2,
                                            mytoml::detail::endian::little) == -1);
        CHECK(mytoml::detail::utf16::encode(0x110000, output, 4,
                                            mytoml::detail::endian::little) == -1);
    }

    SECTION("to_utf8")
    {
        const std::vector<unsigned char> ascii = {'\x41', '\x00'};
        CHECK(mytoml::detail::utf16::to_utf8(ascii, mytoml::detail::endian::little) == "A");

        const std::vector<unsigned char> emoji = {0x3D, 0xD8, 0x00, 0xDE};
        CHECK(mytoml::detail::utf16::to_utf8(emoji, mytoml::detail::endian::little) ==
              "\xF0\x9F\x98\x80");
    }
}

TEST_CASE("UTF-32 decoding and encoding", "[encoding]")
{
    SECTION("little-endian code point")
    {
        const char data[] = {'\x00', '\xF6', '\x01', '\x00'};
        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf32::decode(data, sizeof(data), codepoint,
                                            mytoml::detail::endian::little) == 4);
        CHECK(codepoint == 0x1F600);
    }

    SECTION("big-endian code point")
    {
        const char data[] = {'\x00', '\x01', '\xF6', '\x00'};
        unsigned int codepoint = 0;
        CHECK(mytoml::detail::utf32::decode(data, sizeof(data), codepoint,
                                            mytoml::detail::endian::big) == 4);
        CHECK(codepoint == 0x1F600);
    }

    SECTION("invalid input")
    {
        unsigned int codepoint = 0;
        const char short_input[] = {'\x41'};
        CHECK(mytoml::detail::utf32::decode(short_input, sizeof(short_input), codepoint,
                                            mytoml::detail::endian::little) == -1);
        CHECK(mytoml::detail::utf32::decode(nullptr, 4, codepoint, mytoml::detail::endian::little) == -1);
    }

    SECTION("encode")
    {
        mytoml::detail::utf32::char_t output[1] = {};
        CHECK(mytoml::detail::utf32::encode(0x1F600, output, 4,
                                            mytoml::detail::endian::little) == 1);
        const char *bytes = reinterpret_cast<const char *>(output);
        CHECK(bytes[0] == '\x00');
        CHECK(bytes[1] == '\xF6');
        CHECK(bytes[2] == '\x01');
        CHECK(bytes[3] == '\x00');

        CHECK(mytoml::detail::utf32::encode(0x41, output, 0,
                                            mytoml::detail::endian::little) == -1);
    }

    SECTION("to_utf8")
    {
        const std::vector<unsigned char> bytes = {0x00, 0xF6, 0x01, 0x00};
        CHECK(mytoml::detail::utf32::to_utf8(bytes, mytoml::detail::endian::little) ==
              "\xF0\x9F\x98\x80");
    }
}
