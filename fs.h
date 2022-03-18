#include "parse.h"

#define MAX_PATH_BUF 1024

struct fs_backend {
    char *rootdir;
    char pathbuf[MAX_PATH_BUF];
};

int write_fs(struct fs_backend *fs, struct parsed_data *pdata);
