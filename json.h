#include <stdio.h>
#include "parse.h"
#include "buffer.h"

ssize_t write_toc_json(int fildes, struct toc *toc, struct write_buffer *wbuf, char standalone);
ssize_t write_uri_parts_json(int fildes, struct uri *uri_parts, struct write_buffer *wbuf,
                             char standalone);
ssize_t write_json(struct parsed_data *pdata, int fildes, struct write_buffer *wbuf);
