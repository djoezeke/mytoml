#if defined(__cpp_modules) && __cpp_modules >= 201907L
import mytoml;
#else
#include <mytoml/mytoml.hpp>
#endif

#include <iostream>

int main()
{
    auto value = mytoml::toml::parse("title = \"module-example\"");
    std::cout << value.as_table().at("title").as_string() << '\n';
    return 0;
}
