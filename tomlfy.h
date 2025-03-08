#ifndef DJOEZEKE_TOMLFY_H
#define DJOEZEKE_TOMLFY_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma region Tomlfy DEF
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

// Forward declaration of Toml
typedef struct Toml Toml;

// Define a TOML key-value pair for tables
typedef struct
{
    char *key;
    Toml *value;
} TomlKeyValuePair;

// Define a TOML value
struct Toml
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
            Toml **items;
            size_t size;
        } array_value;
        struct
        {
            TomlKeyValuePair **items;
            size_t size;
        } table_value;
    } data;
};

#pragma endregion // Tomlfy DEF

#pragma region Tomlfy IMP
// Function to create a TOML string value
Toml *tomlfy_create_string(const char *string_value)
{
    Toml *value = (Toml *)malloc(sizeof(Toml));
    value->type = TOML_STRING;
    value->data.string_value = strdup(string_value);
    return value;
}

// Function to create a TOML integer value
Toml *tomlfy_create_integer(int int_value)
{
    Toml *value = (Toml *)malloc(sizeof(Toml));
    value->type = TOML_INTEGER;
    value->data.int_value = int_value;
    return value;
}

// Function to create a TOML float value
Toml *tomlfy_create_float(double float_value)
{
    Toml *value = (Toml *)malloc(sizeof(Toml));
    value->type = TOML_FLOAT;
    value->data.float_value = float_value;
    return value;
}

// Function to create a TOML boolean value
Toml *tomlfy_create_boolean(int bool_value)
{
    Toml *value = (Toml *)malloc(sizeof(Toml));
    value->type = TOML_BOOLEAN;
    value->data.bool_value = bool_value;
    return value;
}

// Function to create a TOML array value
Toml *tomlfy_create_array()
{
    Toml *value = (Toml *)malloc(sizeof(Toml));
    value->type = TOML_ARRAY;
    value->data.array_value.items = NULL;
    value->data.array_value.size = 0;
    return value;
}

// Function to add an item to a TOML array
void tomlfy_array_add(Toml *array, Toml *item)
{
    if (array->type != TOML_ARRAY)
        return;
    array->data.array_value.items = (Toml **)realloc(array->data.array_value.items, (array->data.array_value.size + 1) * sizeof(Toml *));
    array->data.array_value.items[array->data.array_value.size] = item;
    array->data.array_value.size++;
}

// Function to create a TOML table value
Toml *tomlfy_create_table()
{
    Toml *value = (Toml *)malloc(sizeof(Toml));
    value->type = TOML_TABLE;
    value->data.table_value.items = NULL;
    value->data.table_value.size = 0;
    return value;
}

// Function to add a key-value pair to a TOML table
void tomlfy_table_add(Toml *table, const char *key, Toml *value)
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
void tomlfy_print(Toml *value, int indent)
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
void tomlfy_free(Toml *value)
{
    if (!value)
        return;
    switch (value->type)
    {
    case TOML_STRING:
        free(value->data.string_value);
        break;
    case TOML_ARRAY:
        for (size_t i = 0; i < value->data.array_value.size; i++)
        {
            tomlfy_free(value->data.array_value.items[i]);
        }
        free(value->data.array_value.items);
        break;
    case TOML_TABLE:
        for (size_t i = 0; i < value->data.table_value.size; i++)
        {
            free(value->data.table_value.items[i]->key);
            tomlfy_free(value->data.table_value.items[i]->value);
            free(value->data.table_value.items[i]);
        }
        free(value->data.table_value.items);
        break;
    default:
        break;
    }
    free(value);
}

#pragma endregion // Tomlfy IMP

#endif // DJOEZEKE_TOMLFY_H