#ifndef PDQ_DB_H
#define PDQ_DB_H
#include "libpq-fe.h"
#include "buffer.h"
#include "parse.h"
#include "json.h"
void exit_nicely(PGconn *conn);
PGconn *db_connect(char *conninfo);
int db_import(PGconn *conn, struct parsed_data *pdata, struct write_buffer *buf,
        struct write_buffer *alt_buf);
#endif //PDQ_DB_H
