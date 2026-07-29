#include "termix.h"
#include "termix-def.h"

#include <stdlib.h>
#include <string.h>

size_t round_size(size_t size) {
    if(size == 0) return 1;

    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size |= size >> 32;

    return size + 1;
}

char *malloc_string(const char *str, size_t len) {
    char *dup = config_malloc(len + 1);
    if(!dup) return NULL;

    memcpy(dup, str, len);
    dup[len] = '\0';

    return dup;
}
