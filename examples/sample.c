#include "../tomlfy.h"

int main()

{
    // Create a TOML table
    Toml_t *root = tomlfy_create_table();
    tomlfy_table_add(root, "name", tomlfy_create_string("John Doe"));
    tomlfy_table_add(root, "age", tomlfy_create_integer(30));
    tomlfy_table_add(root, "height", tomlfy_create_float(5.9));
    tomlfy_table_add(root, "is_student", tomlfy_create_boolean(0));

    // Create a TOML array and add it to the table
    Toml_t *hobbies = tomlfy_create_array();
    tomlfy_array_add(hobbies, tomlfy_create_string("reading"));
    tomlfy_array_add(hobbies, tomlfy_create_string("swimming"));
    tomlfy_array_add(hobbies, tomlfy_create_string("coding"));
    tomlfy_table_add(root, "hobbies", hobbies);

    // Print the TOML value
    tomlfy_print(root, 0);

    // Free the TOML value
    tomlfy_free(root);

    return 0;
}