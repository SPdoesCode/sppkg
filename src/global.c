#include "include/global.h"
#include "include/parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static enum tokentype {
    IDENTIFIER,
    DOTDOT,
    VALUE,
    EOF_TOK,
} tokentype;

static struct token {
    enum tokentype type;
    int x, y;
    char* value;
} token;

// helper functions
static struct token* __tokenize(char* src);
static int __iswhitespace(char c);
static int __isnewline(char c);
static char __peek(int pos, char* src);
static int __howmanytokens(struct token* token);
static void __verify_gbl(global_t* gbl);

void add_global(char* filepath, package_t pkg, global_t *gbl) {
    gbl->packages = realloc(gbl->packages, (gbl->gpkg_num + 1) * sizeof(*gbl->packages));
    gbl->packages[gbl->gpkg_num].file = strdup(filepath);
    gbl->packages[gbl->gpkg_num].name = strdup(pkg.name);
    gbl->packages[gbl->gpkg_num].version = strdup(pkg.version);
    gbl->packages[gbl->gpkg_num].src = strdup(pkg.src);

    gbl->gpkg_num++;

    __verify_gbl(gbl);
}

void rm_global(package_t pkg, global_t *gbl) {
    for (int i = 0; i < gbl->gpkg_num; i++) {
        if (gbl->packages[i].name == pkg.name) {
            free(gbl->packages[i].file);
            free(gbl->packages[i].name);
            free(gbl->packages[i].version);
            free(gbl->packages[i].src);
            for (int j = i; i < gbl->gpkg_num - 1; i++) {
                gbl->packages[i] = gbl->packages[i + 1];
            }
            gbl->gpkg_num--;
            gbl->packages = realloc(gbl->packages, gbl->gpkg_num * sizeof(*gbl->packages));
            __verify_gbl(gbl);
            return;
        }
    }
}

char* write_global(global_t gbl) {
    __verify_gbl(&gbl);

    char* str = NULL;
    size_t len = 0;

    for (size_t i = 0; i < gbl.gpkg_num; i++) {
        size_t llen = snprintf(NULL, 0, "%s:\"%s\":\"%s\":\"%s\"\n",
                gbl.packages[i].name, gbl.packages[i].version, gbl.packages[i].file, gbl.packages[i].src);

        char* tmp = realloc(str, len + llen + 1);
        if (!tmp) {
            free(str);
            return NULL;
        }
        str = tmp;

        sprintf(str + len, "%s:\"%s\":\"%s\":\"%s\"\n",
                gbl.packages[i].name, gbl.packages[i].version, gbl.packages[i].file, gbl.packages[i].src);

        len += llen;
    }

    return str;
}

int in_global(global_t gbl, package_t pkg) {
    for (int i = 0; i < gbl.gpkg_num; i++) {
        if (gbl.packages[i].name == pkg.name) {
            return 0;
        }
    }
    return 1;
}

global_t parser_global(char *src) {
    global_t gbl;
    memset(&gbl, 0, sizeof(gbl));

    struct token* tokens = __tokenize(src);
    int tok_num = __howmanytokens(tokens);

    int j = 0;
    for(int i = 0; i < tok_num; i++) {
        if (tokens[i].type == IDENTIFIER) {
            if (i + 6 > tok_num) {
                printf("ERROR (GLOBAL): Expected a filled out line at line %d column %d", tokens[i].y, tokens[i].x);
                exit(1);
            }
            if (tokens[i+1].type == DOTDOT && tokens[i+2].type == VALUE && tokens[i+3].type == DOTDOT
                && tokens[i+4].type == VALUE && tokens[i+5].type == DOTDOT && tokens[i+6].type == VALUE) {
                    gbl.packages = realloc(gbl.packages, sizeof(*gbl.packages) * (j + 1));
                    gbl.packages[j].name = strdup(tokens[i].value);
                    gbl.packages[j].version = strdup(tokens[i+2].value);
                    gbl.packages[j].file = strdup(tokens[i+4].value);
                    gbl.packages[j].src = strdup(tokens[i+6].value);
                    j++;
                    i+=6;
            } else {
                printf("ERROR (GLOBAL): Incorrectly filled out line at line %d column %d", tokens[i].y, tokens[i].x);
                exit(1);
            }
        }
    }
    gbl.gpkg_num = j;

    __verify_gbl(&gbl);

    return gbl;
}

void free_global(global_t *gbl) {
    for (int i = 0; i < gbl->gpkg_num; i++) {
        free(gbl->packages[i].file);
        free(gbl->packages[i].name);
        free(gbl->packages[i].version);
        free(gbl->packages[i].src);
    }
    free(gbl->packages);
}

static int __iswhitespace(char c) {
    if (c == ' ' || c == '\t') {
        return 0;
    }
    return 1;
}

static int __isnewline(char c) {
    if (c == '\n') {
        return 0;
    }
    return 1;
}

static char __peek(int pos, char* src) {
    return src[pos];
}

static int __howmanytokens(struct token* tokens) {
    int i = 0;

    while (tokens[i].type != EOF_TOK) {
        i++;
    }

    return i;
}

static struct token* __tokenize(char* src) {
    struct token* tokens = malloc(sizeof(struct token) * 1028);
    int num = 0;
    int pos = 0;
    int cx = 1, cy = 1;
    while (__peek(pos, src) != '\0') {
        if (pos > strlen(src)) break;
        char c = __peek(pos, src);
        if (__iswhitespace(c) == 0) {
            pos++;
            cx++;
            continue;
        }
        if (__isnewline(c) == 0) {
            cy++;
            pos++;
            cx = 1;
            continue;
        }
        switch (c) {
            case '#': {
                while (__peek(pos, src) != '\n' && __peek(pos, src) != '\0') pos++;
                break;
            }
            case '"': {
                tokens[num].x = cx;
                tokens[num].y = cy;
                tokens[num].type = VALUE;
                pos++;
                int start = pos;
                while (__peek(pos, src) != '"' && __peek(pos, src) != '\0') {
                    if (__isnewline(__peek(pos, src)) == 0) {
                        printf("ERROR (TOKONIZER): Unexpected newline in value at line %d column %d", cy, cx);
                        exit(0);
                    }
                    pos++;
                    cx++;
                }
                size_t len = pos - start;
                tokens[num].value = malloc(len + 1);
                if (!tokens[num].value) exit(1);
                strncpy(tokens[num].value, &src[start], len);
                tokens[num].value[len] = '\0';
                if (__peek(pos, src) == '"') pos++;
                num++;
                break;
            }
            case ':': {
                tokens[num].type = DOTDOT;
                tokens[num].value = strdup(":");
                tokens[num].x = cx;
                tokens[num].y = cy;
                num++;
                pos++;
                cx++;
                break;
            }
            default: {
                tokens[num].x = cx;
                tokens[num].y = cy;
                tokens[num].type = IDENTIFIER;
                int start = pos;
                while (__iswhitespace(__peek(pos, src)) != 0 && __peek(pos, src) != '{' && __peek(pos, src) != '}' && __peek(pos, src) != '=' && __peek(pos, src) != '\0') {
                    pos++;
                    cx++;
                }
                size_t len = pos - start;
                tokens[num].value = malloc(len + 1);
                if (!tokens[num].value) exit(1);
                strncpy(tokens[num].value, &src[start], len);
                tokens[num].value[len] = '\0';
                num++;
                break;
            }
        }
    }
    tokens[num].type = EOF_TOK;
    tokens[num].value = strdup("EOF");
    tokens[num].x = cx;
    tokens[num].y = cy;
    return tokens;
}

static void __verify_gbl(global_t* gbl) {
    if (gbl->packages) {
    } else {
        for (int i = 0; i < gbl->gpkg_num; i++) {
            if (gbl->packages[i].name){
            } else {
                printf("WARN (GBL VERIFIER): Possible error with name in global package %d\n", i);
            }
            if (gbl->packages[i].file){
            } else {
                printf("WARN (GBL VERIFIER): Possible error with file in global package %d\n", i);
            }
            if (gbl->packages[i].src){
            } else {
                printf("WARN (GBL VERIFIER): Possible error with source in global package %d\n", i);
            }
            if (gbl->packages[i].version){
            } else {
                printf("WARN (GBL VERIFIER): Possible error with name in global package %d\n", i);
            }
        }
    }
}