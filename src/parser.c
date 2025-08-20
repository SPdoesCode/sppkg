#include "include/parser.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static enum tokentype {
    IDENTIFIER,
    O_BRACE,
    C_BRACE,
    EQUALS,
    VALUE,
    COMMA,
    EOF_TOK,
} tokentype;

static struct token {
    enum tokentype type;
    char* value;
    int x, y;
} token;

// Helper functions
static struct token* __tokenize(char *src);
static int __iswhitespace(char c);
static int __isnewline(char c);
static char __peek(int pos, char* src);
static int __howmanytokens(struct token* token);
static void __validate_pkg(package_t* pkg);
static void __validate_cfg(config_t* cfg);

void free_pkg(package_t* pkg) {
    if (!pkg) return;

    free(pkg->name);
    free(pkg->version);
    free(pkg->author);
    free(pkg->src);

    for (size_t i = 0; i < pkg->dep_num; i++) {
        free(pkg->deps[i].dep);
        free(pkg->deps[i].ver);
    }
    free(pkg->deps);

    for (size_t i = 0; i < pkg->build_num; i++) {
        free(pkg->builds[i].step);
    }
    free(pkg->builds);
}

void free_cfg(config_t* cfg) {
    if (!cfg) return;
    for (size_t i = 0; i < cfg->repos_num; i++) {
        free(cfg->repos[i].name);
        free(cfg->repos[i].link);
    }
    free(cfg->repos);
}


package_t parse_package(char *src) {
    package_t pkg;
    memset(&pkg, 0, sizeof(pkg));

    struct token* tokens = __tokenize(src);
    int tok_num = __howmanytokens(tokens);

    for (int i = 0; i < tok_num; i++) {
        if (tokens[i].type == IDENTIFIER) {
            if (i + 1 >= tok_num) {
                printf("ERROR: Identifier %s declared but nothing comes after it at line %d column %d", tokens[i].value, tokens[i].y, tokens[i].x);
                exit(1);
            }
            if (tokens[i + 1].type == EQUALS) {
                if (i + 2 >= tok_num || tokens[i+2].type != VALUE) {
                    printf("ERROR (PACKAGE): Identifier %s declared but no legit value comes after the = at line %d column %d", tokens[i].value, tokens[i + 1].y, tokens[i + 1].x);
                    exit(1);
                }

                if (strcmp(tokens[i].value, "name") == 0) {
                    pkg.name = strdup(tokens[i+2].value);
                } else if (strcmp(tokens[i].value, "version") == 0) {
                    pkg.version = strdup(tokens[i+2].value);
                } else if (strcmp(tokens[i].value, "author") == 0) {
                    pkg.author = strdup(tokens[i+2].value);
                } else if (strcmp(tokens[i].value, "src") == 0) {
                    pkg.src = strdup(tokens[i+2].value);
                } else {
                    printf("ERROR (PACKAGE): Identifier %s does not match any allowed identifiers line %d column %d", tokens[i].value, tokens[i].y, tokens[i].x);
                    exit(1);
                }
                i += 2;

            } else if (tokens[i+1].type == O_BRACE) {
                if (strcmp(tokens[i].value, "deps") == 0) {
                    size_t l = 0;
                    for (size_t j = i + 2; j < tok_num; j++) {
                        if (tokens[j].type == C_BRACE) { pkg.dep_num = l; i = j; break; }
                        if (tokens[j].type == VALUE) {
                            if (tokens[j + 1].type != EQUALS || tokens[j + 2].type != VALUE) {
                                printf("ERROR (PACKAGE): Malformed dep at line %d column %d", tokens[j].y, tokens[j].x);
                                exit(1);
                            }
                            pkg.deps = realloc(pkg.deps, sizeof(*pkg.deps) * (l + 1));
                            pkg.deps[l].dep = strdup(tokens[j].value);
                            pkg.deps[l].ver = strdup(tokens[j+2].value);
                            l++;
                            j += 2;
                        }
                    }
                }
                if (strcmp(tokens[i].value, "build") == 0) {
                    size_t l = 0;
                    for (size_t j = i + 2; j < tok_num; j++) {
                        if (tokens[j].type == C_BRACE) { pkg.build_num = l; i = j; break; }
                        if (tokens[j].type == VALUE) {
                            pkg.builds = realloc(pkg.builds, sizeof(*pkg.builds) * (l + 1));
                            pkg.builds[l].step = strdup(tokens[j].value);
                            l++;
                        } else {
                            printf("ERROR (PACKAGE): Expected a value got other with value of %s at line %d column %d",tokens[j].value, tokens[j].y, tokens[j].x);
                            exit(1);
                        }
                    }
                }
            }
        }
    }
    __validate_pkg(&pkg);
    return pkg;
}

config_t parse_config(char *src) {
    config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    struct token* tokens = __tokenize(src);
    int tok_num = __howmanytokens(tokens);

    for (int i = 0; i < tok_num; i++) {
        if (tokens[i].type == IDENTIFIER) {
            if (i + 1 >= tok_num) {
                printf("ERROR: Identifier %s declared but nothing comes after it at line %d column %d", tokens[i].value, tokens[i].y, tokens[i].x);
                exit(1);
            }
            if (tokens[i+1].type == O_BRACE) {
                if (strcmp(tokens[i].value, "repos") == 0) {
                    size_t l = 0;
                    for (size_t j = i + 2; j < tok_num; j++) {
                        if (tokens[j].type == C_BRACE) { cfg.repos_num = l; i = j; break; }
                        if (tokens[j].type == VALUE) {
                            if (tokens[j + 1].type != EQUALS || tokens[j + 2].type != VALUE) {
                                printf("ERROR (PACKAGE): Malformed repo at line %d column %d", tokens[j].y, tokens[j].x);
                                exit(1);
                            }
                            cfg.repos = realloc(cfg.repos, sizeof(*cfg.repos) * (l + 1));
                            cfg.repos[l].name = strdup(tokens[j].value);
                            cfg.repos[l].link = strdup(tokens[j+2].value);
                            l++;
                            j += 2;
                        }
                    }
                }
            }
        }
    }
    __validate_cfg(&cfg);
    return cfg;
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
            case '=': {
                tokens[num].type = EQUALS;
                tokens[num].value = strdup("=");
                tokens[num].x = cx;
                tokens[num].y = cy;
                num++;
                pos++;
                cx++;
                break;
            }
            case ',': {
                tokens[num].type = COMMA;
                tokens[num].value = strdup(",");
                tokens[num].x = cx;
                tokens[num].y = cy;
                num++;
                pos++;
                cx++;
                break;
            }
            case '{': {
                tokens[num].type = O_BRACE;
                tokens[num].value = strdup("{");
                tokens[num].x = cx;
                tokens[num].y = cy;
                num++;
                pos++;
                cx++;
                break;
            }
            case '}': {
                tokens[num].type = C_BRACE;
                tokens[num].value = strdup("}");
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

static void __validate_pkg(package_t* pkg) {
    if (pkg->name) {
    } else {
        pkg->name = realloc(pkg->name, sizeof("none"));
        pkg->name = strdup("none");
    }
    if (pkg->author) {
    } else {
        pkg->author = realloc(pkg->author, sizeof("none"));
        pkg->author = strdup("none");
    }
    if (pkg->src) {
    } else {
        pkg->src = realloc(pkg->src, sizeof("none"));
        pkg->src = strdup("none");
    }
    if (pkg->version) {
    } else {
        pkg->version = realloc(pkg->version, sizeof("none"));
        pkg->version = strdup("none");
    }
    if (pkg->builds) {
    } else {
        for (int i = 0; i < pkg->build_num; i++) {
            if (pkg->builds[i].step) {
            } else {
                printf("WARN (PKG VALIDATOR): Possibly error with build step %d\n", i);
            }
        }
    }
    if (pkg->deps) {
    } else {
        for (int i = 0; i < pkg->dep_num; i++) {
            if (pkg->deps[i].dep) {
            } else {
                printf("WARN (PKG VALIDATOR): Possibly error with dep %d in the first value\n", i);
            }
            if (pkg->deps[i].ver) {
            } else {
                printf("WARN (PKG VALIDATOR): Possibly error with dep %d in the 2nd value\n", i);
            }
        }
    }
}
static void __validate_cfg(config_t* cfg) {
    if (cfg->repos) {
        for (int i = 0; i < cfg->repos_num; i++) {
            if (cfg->repos[i].link) {
            } else {
                printf("WARN (CONFIG VALIDATOR): Error with link repo %d in config\n", i);
            }
            if (cfg->repos[i].name) {
            } else {
                printf("WARN (CONFIG VALIDATOR): Error with name repo %d in config\n", i);
            }
        }
    } else {
        printf("WARN (CONFIG VALIDATOR): Error with repos in config\n");
    }
}