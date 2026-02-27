#include <stdio.h>
#include "parse.h"
#include "buffer.h"

ssize_t write_toc_json(int fildes, struct toc *toc, struct write_buffer *wbuf, char standalone);
ssize_t write_mcs_json(int fildes, struct mcs *mcs, struct write_buffer *wbuf,
		       char standalone);
ssize_t write_liens_json(int fildes, struct liens *liens,
			 struct write_buffer *wbuf, char standalone);
ssize_t write_contexte_json(int fildes, struct contexte *contexte,
			    struct write_buffer *wbuf, char standalone);
ssize_t write_contenu_json(int fildes, struct contenu *contenu,
			   struct write_buffer *wbuf, char standalone);
ssize_t write_versions_json(int fildes, struct versions *versions,
			    struct write_buffer *wbuf, char standalone);
ssize_t write_json(struct parsed_data *pdata, int fildes, struct write_buffer *wbuf);
