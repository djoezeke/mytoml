#include <stdio.h>
#include <stdlib.h>

#include "../mytoml.h"

int main(int argc, char *argv[], char **envp)
{
    
    char *file = NULL;
    if (argc > 1)
        file = argv[1];
    TomlKey *toml = tomlLoad(file);
    if (toml == NULL)
        return 1;
    toml_json_dump(toml);
    tomlFree(toml);
    return 0;
}
