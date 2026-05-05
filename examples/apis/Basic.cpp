#include <exception>
#include <iostream>
#include <mytoml/mytoml.hpp>
#include <string>

int main()
{
    const std::string toml_text = R"(
[owner]
name = "Tom Preston-Werner"
dob = 1979-05-27T07:32:00Z

[database]
server = "192.168.1.1"
ports = [8001, 8001, 8002]
enabled = true
)";

    try
    {
        const mytoml::toml data = mytoml::toml::parse(toml_text);

        std::cout << "Owner Name: " << data["owner"]["name"].as_string() << '\n';
        std::cout << "Database Server: " << data["database"]["server"].as_string() << '\n';
        std::cout << "Database Enabled: " << (data["database"]["enabled"].as_bool() ? "true" : "false") << '\n';
        std::cout << "Serialized TOML:\n"
                  << data.serialize() << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
