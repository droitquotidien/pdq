#include <stddef.h>
#include <stdio.h>
#include <archive.h>
#include "buffer.h"

int read_archive_file(struct archive *a, struct write_buffer *wbuf)
{
	const void *buff;
	size_t len;
	off_t offset;
	int r;

	for (;;) {
		r = archive_read_data_block(a, &buff, &len, &offset);
		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK)
			return r;
		r = copy_to_buffer(buff, len, wbuf);
		if (r != 0)
			return r;
	}
	return 0;
}