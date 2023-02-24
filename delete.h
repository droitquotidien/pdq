#ifndef PDQ_DELETE_H
#define PDQ_DELETE_H
#include <regex.h>
extern regex_t suppr_re;
int init_delete_re(void);
void free_delete_re(void);
int apply_deletions(struct write_buffer *buf, PGconn *pg_conn, struct tm *ts);
#endif //PDQ_DELETE_H
