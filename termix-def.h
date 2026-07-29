#ifndef TERMIX_DEF_H
#define TERMIX_DEF_H

#include <stddef.h>

/** --- termix-helper --- */
size_t round_size(size_t size);
char *malloc_string(const char *str, size_t len);

/** --- termix-xml --- */
typedef const char *xml_error;

typedef struct {
    char *name;
    char *value;
} xml_attr;

typedef struct xml_node {
    char *name;
    char *text;

    xml_attr *attrs;
    size_t attrs_used;
    size_t attrs_size;

    struct xml_node **childs;
    size_t childs_used;
    size_t childs_size;
} xml_node;

extern const xml_error XML_EMPTY_TEXT;
extern const xml_error XML_MISSING_OPENING_TAG;
extern const xml_error XML_MISSING_TAG_NAME;
extern const xml_error XML_INVALID_TAG_NAME;
extern const xml_error XML_OUT_OF_MEMORY;
extern const xml_error XML_INVALID_TOKEN;
extern const xml_error XML_MISSING_ATTRIBUTE_VALUE;
extern const xml_error XML_INVALID_ATTRIBUTE_NAME;
extern const xml_error XML_UNCLOSED_QUOTES;
extern const xml_error XML_MISSING_CLOSING_TAG;

xml_node *xml_parse(const char **xml, xml_error *error);
void xml_print(const xml_node *node, int depth);

/** --- termix-ansi */
#define ANSI_BLACK 0
#define ANSI_RED 1
#define ANSI_GREEN 2
#define ANSI_YELLOW 3
#define ANSI_BLUE 4
#define ANSI_MAGENTA 5
#define ANSI_CIANO 6
#define ANSI_BIANCO 7

void ansi_bold(void);
void ansi_underlined(void);
void ansi_color(int color);
void ansi_background(int color);
void ansi_reset(void);

/** --- termix-core --- */
void *config_malloc(size_t size);
void *config_realloc(void *ptr, size_t size);
void config_free(void *ptr);

#endif
