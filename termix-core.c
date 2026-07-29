#include "termix.h"
#include "termix-def.h"

#include <stdlib.h>

static void *(*internal_malloc)(size_t) = malloc;
static void *(*internal_realloc)(void*, size_t) = realloc;
static void (*internal_free)(void*) = free;

void *config_malloc(size_t size) {
    return internal_malloc(size);
}

void *config_realloc(void *ptr, size_t size) {
    return internal_realloc(ptr, size);
}

void config_free(void *ptr) {
    internal_free(ptr);
}

void termix_config(void *(*mall)(size_t), void *(*reall)(void*, size_t), void (*fr)(void*)) {
    internal_malloc = mall;
    internal_realloc = reall;
    internal_free = fr;
}
