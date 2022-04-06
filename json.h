#include <stdio.h>
#include "parse.h"
#include "buffer.h"

ssize_t write_json(struct parsed_data *pdata, int fildes, struct write_buffer *wbuf);
