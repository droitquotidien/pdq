#ifndef PDQ_DB_H
#define PDQ_DB_H
#include <openssl/evp.h>
#include "libpq-fe.h"
#include "buffer.h"
#include "parse.h"
#include "json.h"
#include "timings.h"
void exit_nicely(PGconn *conn);
PGconn *db_connect(char *conninfo);
int db_import(PGconn *conn, const EVP_MD *md, struct tm *tag,
	struct parsed_data *pdata, struct write_buffer *dbbuf1,
        struct write_buffer *dbbuf2, struct write_buffer *dbbuf3,
	struct write_buffer *dbbuf4, struct write_buffer *dbbuf5,
	struct write_buffer *dbbuf6,
	struct timings *tt, char bootstrap);
int delete(PGconn *conn, enum doctype doctype, char id[], char tag[]);
#endif //PDQ_DB_H
