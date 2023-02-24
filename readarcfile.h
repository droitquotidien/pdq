#ifndef PDQ_READARCFILE_H
#define PDQ_READARCFILE_H
#include <archive.h>
#include "buffer.h"
int read_archive_file(struct archive *a, struct write_buffer *wbuf);
#endif //PDQ_READARCFILE_H
