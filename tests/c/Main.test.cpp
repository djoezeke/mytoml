#include <catch2/catch_test_macros.hpp>
#include <mytoml/mytoml.h>

#include <cstring>

TEST_CASE("C API exposes version macros", "[c]")
{
    CHECK(MYTOMLC_VERSION_MAJOR == 0);
    CHECK(MYTOMLC_VERSION_MINOR == 1);
    CHECK(MYTOMLC_VERSION_PATCH == 0);
    CHECK(std::strcmp(MYTOMLC_VERSION, "0.1.0") == 0);
}

TEST_CASE("C API exposes an encodings enumeration", "[c]")
{
    CHECK(static_cast<int>(mtUnspecifiedEncoding) == 0);
    CHECK(static_cast<int>(mtUTF8Encoding) == 1);
    CHECK(static_cast<int>(mtUTF16Encoding) == 2);
    CHECK(static_cast<int>(mtUTF16LEEncoding) == 3);
    CHECK(static_cast<int>(mtUTF16BEEncoding) == 4);
    CHECK(static_cast<int>(mtUTF32Encoding) == 5);
    CHECK(static_cast<int>(mtUTF32LEEncoding) == 6);
    CHECK(static_cast<int>(mtUTF32BEEncoding) == 7);
}

TEST_CASE("C API exposes a version structure", "[c]")
{
    mtVersion version{1, 2, 3};
    CHECK(version.major == 1);
    CHECK(version.minor == 2);
    CHECK(version.patch == 3);
}
