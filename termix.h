#ifndef TERMIX_H
#define TERMIX_H

#include <stddef.h>

void termix_config(void *(*mall)(size_t), void *(*reall)(void*, size_t), void (*fr)(void*));

#endif
