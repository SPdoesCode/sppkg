/*

        THIS IS A PLACEHOLDER TILL I USE LIBGIT2 OR FIND
        A BETTER SOLOTION

*/

#include "include/repo.h"
#include "include/parser.h"
#include "include/helper.h"
#include "../config.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

static void pull_repo(char *link, char* name) {
    size_t len_clone = snprintf(NULL, 0, "git clone %s %s/%s", link, REPO_DIR, name);
    char *cmd_clone = malloc(len_clone + 1);
    if (!cmd_clone) return;

    sprintf(cmd_clone, "git clone %s %s/%s", link, REPO_DIR, name);
    system(cmd_clone);
    free(cmd_clone);

    size_t len_rm = snprintf(NULL, 0, "rm -rf %s/%s/.git", REPO_DIR, name);
    char *cmd_rm = malloc(len_rm + 1);
    if (!cmd_rm) return;

    sprintf(cmd_rm, "rm -rf %s/%s/.git", REPO_DIR, name);
    system(cmd_rm);
    free(cmd_rm);
}

void update_repos() {

    config_t cfg = parse_config(read_file(CONFIG_FILE));

    size_t len_rm = snprintf(NULL, 0, "rm -rf %s", REPO_DIR);
    char *cmd_rm = malloc(len_rm + 1);
    if (!cmd_rm) return;

    sprintf(cmd_rm, "rm -rf %s", REPO_DIR);
    system(cmd_rm);
    free(cmd_rm);

    for (int i = 0; i < cfg.repos_num; i++) {
        printf("Pulling repo %s to %s/%s", cfg.repos[i].link, REPO_DIR, cfg.repos[i].name);
        pull_repo(cfg.repos[i].link, cfg.repos[i].name);
    }
}