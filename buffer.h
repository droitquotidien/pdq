#ifndef PDQ_BUFFER_H
#define PDQ_BUFFER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#define CHECK_WRITE(fildes, buf, len, wbuf) \
          r = buffer_copy(fildes, buf, len, wbuf);\
          if (r < 0) {\
                return -1;\
          }\
          rt += r;

#define MAX_SIZE_WRITE_BUFFER 65536

struct write_buffer {
    char *buffer;
    char *cbuffer;
    ssize_t current_size;
    ssize_t max_size;
    char flushable;
};

struct write_buffer *allocate_write_buffer(ssize_t max_size, char flushable);
ssize_t buffer_copy(int fildes, const char *buf, ssize_t len, struct write_buffer *wbuf);
ssize_t buffer_flush(int fildes, struct write_buffer *wbuf);
ssize_t buffer_transfer(int fildes, struct write_buffer *sbuf, struct write_buffer *wbuf);

#endif //PDQ_BUFFER_H
