#include "buffer.h"

struct write_buffer *allocate_write_buffer(ssize_t max_size, char flushable)
{
	struct write_buffer *buf;

	buf = calloc(1, sizeof(struct write_buffer));
	if (buf == NULL) {
		perror("cannot allocate write_buffer");
		return NULL;
	}

	buf->buffer = calloc(1, max_size);
	if (buf->buffer == NULL) {
		free(buf);
		perror("cannot allocate write_buffer->buffer");
		return NULL;
	}
	buf->max_size = max_size;
	buf->cbuffer = buf->buffer;
	buf->flushable = flushable;

	return buf;
}

ssize_t buffer_flush(int fildes, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;

	r = write(fildes, wbuf->buffer, wbuf->current_size);
	if (r < 0) {
		perror("cannot write");
		return -1;
	}
	rt += r;
	wbuf->cbuffer = wbuf->buffer;
	wbuf->current_size = 0;

	return rt;
}

ssize_t buffer_copy(int fildes, const char *buf, ssize_t len, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;

	if ((len + wbuf->current_size) > wbuf->max_size) {
		if (wbuf->flushable) {
			r = buffer_flush(fildes, wbuf);
			if (r < 0) return -1;
			rt += r;
		} else {
			fprintf(stderr,
				"ERROR: write_buffer too small (max: %zu, exceed: %zu)\n",
				wbuf->max_size,
				((len + wbuf->current_size) - wbuf->max_size));
			return -1;
		}
	}
	if ((len + wbuf->current_size) > wbuf->max_size) {
		fprintf(stderr, "ERROR: write_buffer too small (even after flush) (%zu bytes missing)\n",
			((len + wbuf->current_size) - wbuf->max_size));
	}
	assert ((len + wbuf->current_size) <= wbuf->max_size);
	memcpy(wbuf->cbuffer, buf, len);
	wbuf->cbuffer += len;
	wbuf->current_size += len;

	return rt;
}

ssize_t buffer_transfer(int fildes, struct write_buffer *sbuf, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;

	r = buffer_copy(fildes, sbuf->buffer, sbuf->current_size, wbuf);
	if (r < 0) return -1;
	rt += r;
	sbuf->cbuffer = sbuf->buffer;
	sbuf->current_size = 0;

	return rt;
}

void buffer_reset(struct write_buffer *buf)
{
    buf->cbuffer = buf->buffer;
    buf->current_size = 0;
}
