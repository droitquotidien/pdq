#include "parse.h"
#include "buffer.h"

#define MAX_PATH_BUF 1024

struct fs_backend {
    char *rootdir;
    char pathbuf[MAX_PATH_BUF];
};

int create_file(const char *fname);
ssize_t write_fs(struct fs_backend *fs, struct parsed_data *pdata, struct write_buffer *wbuf, char buf);
