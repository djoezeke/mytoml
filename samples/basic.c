#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../mytoml.h"

int main(int argc, char *argv[])
{

    char *file = NULL;
    if (argc > 1)
        file = argv[1];
    TomlKey *toml = tomlLoad(file);
    if (toml == NULL)
        return 1;
    // char *buffer = "";
    // size_t size = 0;
    // toml_key_dump(toml, &buffer, &size);
    // printf(buffer);
    tomlKeyDumpFile(toml, argv[2]);
    tomlFree(toml);
    return 0;
}
