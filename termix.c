#include "ecli.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *name;
    char *value;
} xmlattr;

typedef struct xmltag {
    char *name;
    xmlattr *attrs;
    struct xmltag *tags;
    char *content;
} xmltag;

static inline void xml_skip_space(char **xml) {
    while(isspace(**xml)) *xml += 1;
}

static inline int xml_is_tag_char(int c) {
    return (
        isalpha(c) ||
        c == '-'   ||
        c == '_'   ||
        c == '.'
    );
}

static inline int xml_is_tag_name_invalid(char *name, size_t len) {
    if(len == 0) return 1;

    if(len < 3) return 0;

    if(
        tolower(name[0]) == 'x' &&
        tolower(name[1]) == 'm' &&
        tolower(name[2]) == 'l'
    ) return 1;

    return 0;
}

static xmltag *xml_next_tag(char **xml) {
    xml_skip_space(xml);

    if(**xml != '<') return NULL;
    *xml += 1;

    char *name = *xml;

    if(!isalpha(**xml) && **xml != '_') return NULL;
    *xml += 1;

    while(xml_is_tag_char(**xml)) *xml += 1;

    char *endname = *xml;

    if(xml_is_tag_name_invalid(name, endname - name - 1)) return NULL;

    xml_skip_space(xml);

    while(1) {
        
    }

    if(**xml != '>') return NULL;



    *xml = '\0';
    if(xml_is_tag_name_invalid(name)) {
        *xml = '>';
        return NULL;
    }
    *xml += 1;



}

static inline xmltag *xml_generate_DOM(char *xml) {
    return xml_next_tag(&xml);
}
