#include "termix.h"
#include "termix-def.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>

const xml_error XML_EMPTY_TEXT = "empty text";
const xml_error XML_MISSING_OPENING_TAG = "missing opening tag";
const xml_error XML_MISSING_TAG_NAME = "missing tag name";
const xml_error XML_INVALID_TAG_NAME = "invalid tag name";
const xml_error XML_OUT_OF_MEMORY = "out of memory";
const xml_error XML_INVALID_TOKEN = "invalid token";
const xml_error XML_MISSING_ATTRIBUTE_VALUE = "missing attibute value";
const xml_error XML_INVALID_ATTRIBUTE_NAME = "invalid attribute name";
const xml_error XML_UNCLOSED_QUOTES = "unclosed quotes";
const xml_error XML_MISSING_CLOSING_TAG = "missing closing tag";

static inline xml_node *xml_new_node(void) {
    xml_node *node = config_malloc(sizeof(xml_node));
    if(!node) return NULL;

    node->name = NULL;
    node->text = NULL;

    node->attrs = NULL;
    node->attrs_used = 0;
    node->attrs_size = 0;

    node->childs = NULL;
    node->childs_used = 0;
    node->childs_size = 0;

    return node;
}

static void xml_free_node(xml_node *node) {
    if(node->name) config_free(node->name);
    if(node->text) config_free(node->text);

    if(node->attrs) {
        for(size_t n = 0; n < node->attrs_used; n++) {
            config_free(node->attrs[n].name);
            config_free(node->attrs[n].value);
        }
        config_free(node->attrs);
    }

    if(node->childs) {
        for(size_t n = 0; n < node->childs_used; n++) {
            xml_free_node(node->childs[n]);
        }
        config_free(node->childs);
    }

    config_free(node);
}

static xml_error xml_add_child(xml_node *parent, xml_node *child) {
    if(parent->childs_used >= parent->childs_size) {
        size_t new_size = round_size(parent->childs_used + 1);

        xml_node **new_childs = config_realloc(parent->childs, new_size * sizeof(xml_node*));
        if(!new_childs) return XML_OUT_OF_MEMORY;

        parent->childs = new_childs;
        parent->childs_size = new_size;
    }

    parent->childs[parent->childs_used++] = child;

    return NULL;
}

static xml_error xml_add_attr(
    xml_node *node, const char *name, size_t len, const char *value, size_t _len
) {
    if(node->attrs_used >= node->attrs_size) {
        size_t new_size = round_size(node->attrs_used + 1);

        xml_attr *new_attrs = config_realloc(node->attrs, new_size * sizeof(xml_attr));
        if(!new_attrs) return XML_OUT_OF_MEMORY;

        node->attrs = new_attrs;
        node->attrs_size = new_size;
    }

    xml_attr *attr = &node->attrs[node->attrs_used];
    attr->name = malloc_string(name, len);
    attr->value = malloc_string(value, _len);

    if(!attr->name || !attr->value) {
        if(attr->name) config_free(attr->name);
        if(attr->value) config_free(attr->value);
        return XML_OUT_OF_MEMORY;
    }

    node->attrs_used++;

    return NULL;
}

static inline const char *xml_skip_spaces(const char *ptr) {
    while(*ptr && isspace((unsigned char)*ptr)) ptr++;
    return ptr;
}

static inline int xml_is_char1(int c) {
    return isalpha(c) || c == '_';
}

static inline int xml_is_char_other(int c) {
    return  isalnum(c) || c == '_' || c == '-' || c == '.' || c == ':';
}

static xml_error xml_parse_attrs(xml_node *node, const char **xml) {
    while(**xml) {
        *xml = xml_skip_spaces(*xml);
        if(**xml == '>' || **xml == '/' || !**xml) return NULL;

        if(!xml_is_char1((unsigned char)(**xml))) return XML_INVALID_ATTRIBUTE_NAME;

        const char *name = *xml;
        while(xml_is_char_other((unsigned char)(**xml))) {
            *xml += 1;
        }

        size_t name_len = (size_t)(*xml - name);
        if(name_len == 0) return XML_INVALID_TOKEN;

        *xml = xml_skip_spaces(*xml);
        if(**xml != '=') return XML_MISSING_ATTRIBUTE_VALUE;
        *xml += 1;

        *xml = xml_skip_spaces(*xml);

        char quote = **xml;
        if(quote != '\"' && quote != '\'') return XML_MISSING_ATTRIBUTE_VALUE;
        *xml += 1;

        const char *value = *xml;
        while(**xml && **xml != quote) *xml += 1;
        if(**xml != quote) return XML_UNCLOSED_QUOTES;

        size_t value_len = *xml - value;
        *xml += 1;

        xml_error error = xml_add_attr(node, name, name_len, value, value_len);
        if(error) return error;
    }

    return NULL;
}

static xml_error xml_parse_childs(xml_node *node, const char **xml) {
    while(**xml) {
        *xml = xml_skip_spaces(*xml);
        if(!**xml) return NULL;

        const char *tmp = *xml;

        if(tmp[0] == '<' && tmp[1] == '/') {

            *xml += 2;
            const char *name = *xml;

            while(**xml && **xml != '>') *xml += 1;
            size_t len = *xml - name;

            if(**xml != '>' || strncmp(name, node->name, len) != 0) return XML_MISSING_CLOSING_TAG;
            *xml += 1;

            break;
        }

        xml_error error = NULL;
        xml_node *child = xml_parse(xml, &error);
        if(!child) return error;

        error = xml_add_child(node, child);
        if(error) return error;
    }

    return NULL;
}

xml_node *xml_parse(const char **xml, xml_error *error) {
    *xml = xml_skip_spaces(*xml);
    if(!*xml) {
        if(error) *error = XML_EMPTY_TEXT;
        return NULL;
    }

    if(**xml == '<') {
        *xml += 1;

        if(**xml == '/') {
            if(error) *error = XML_MISSING_OPENING_TAG;
            return NULL;
        }

        if(!xml_is_char1((unsigned char)(**xml))) {
            if(error) *error = XML_INVALID_TAG_NAME;
            return NULL;
        }

        const char *name = *xml;
        while(xml_is_char_other((unsigned char)(**xml))) *xml += 1;

        size_t len = *xml - name;
        if(len == 0) {
            if(error) *error = XML_MISSING_TAG_NAME;
            return NULL;
        }

        xml_node *node = xml_new_node();
        if(!node) {
            if(error) *error = XML_OUT_OF_MEMORY;
            return NULL;
        }

        node->name = malloc_string(name, len);
        if(!node->name) {
            if(error) *error = XML_OUT_OF_MEMORY;
            xml_free_node(node);
            return NULL;
        }

        xml_error attr_error = xml_parse_attrs(node, xml);
        if(attr_error) {
            if(error) *error = attr_error;
            xml_free_node(node);
            return NULL;
        }

        *xml = xml_skip_spaces(*xml);
        if(**xml == '/') {
            *xml += 1;
            if(**xml != '>') {
                if(error) *error = XML_INVALID_TOKEN;
                xml_free_node(node);
                return NULL;
            }

            *xml += 1;
            return node;
        }

        if(**xml != '>') {
            if(error) *error = XML_INVALID_TOKEN;
            xml_free_node(node);
            return NULL;
        }

        *xml += 1;

        xml_error childs_error = xml_parse_childs(node, xml);
        if(childs_error) {
            if(error) *error = childs_error;
            xml_free_node(node);
            return NULL;
        }

        return node;
    }

    const char *text = *xml;

    while(**xml && **xml != '<') *xml += 1;

    const char *end = *xml - 1;
    while(end > text && isspace((unsigned char)(*end))) end--;

    size_t len = (size_t)(end - text + 1);

    if(len == 0) {
        if(error) *error = XML_INVALID_TOKEN;
        return NULL;
    }

    xml_node *node = xml_new_node();

    node->text = malloc_string(text, len);
    if(!node->text) {
        if(error) *error = XML_OUT_OF_MEMORY;
        xml_free_node(node);
        return NULL;
    }

    return node;
}

void xml_print(const xml_node *node, int depth) {
    for(size_t n = 0; n < depth; n++) printf("   ");

    if(node->text) {
        printf("%s\n", node->text);
    } else {
        printf("<%s", node->name);
        for(size_t n = 0; n < node->attrs_used; n++) {
            printf(" %s=\"%s\"", node->attrs[n].name, node->attrs[n].value);
        }
        printf(">\n");

        for(size_t n = 0; n < node->childs_used; n++) {
            xml_print(node->childs[n], depth + 1);
        }

        for(size_t n = 0; n < depth; n++) printf("   ");
        printf("</%s>\n", node->name);
    }
}
