#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "db.h"

/*
 * https://gist.github.com/ictlyh/12fe787ec265b33fd7e4b0bd08bc27cb
 * https://www.postgresql.org/docs/current/libpq-exec.html
 */

void exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}

PGconn *db_connect(char *conninfo)
{
    PGconn *conn;
    PGresult *res;

    conn = PQconnectdb(conninfo);

    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "%s", PQerrorMessage(conn));
        exit_nicely(conn);
    }

    /* Set always-secure search path, so malicious users can't take control. */
    res = PQexec(conn,
                 "SELECT pg_catalog.set_config('search_path', '', false)");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
        PQclear(res);
        exit_nicely(conn);
    }

    /*
     * Should PQclear PGresult whenever it is no longer needed to avoid memory
     * leaks
     */
    PQclear(res);

    return conn;
}

int db_import(PGconn *conn, struct tm *tag, struct parsed_data *pdata,
        struct write_buffer *wbuf, struct write_buffer *alt_buf, double *tt)
{
    ssize_t r;
    struct metadata *mdata = pdata->metadata;
    struct toc *toc = pdata->toc;
    PGresult *res;
    const char *param_values[16];
    clock_t ct;
    char stag[20];

    sprintf(stag, "%04d-%02d-%02d %02d:%02d:%02d",
            tag->tm_year, tag->tm_mon, tag->tm_mday,
            tag->tm_hour, tag->tm_min, tag->tm_sec);

    switch (mdata->uri_parts.doctype) {
        case JORFCONT_DOCTYPE:
            fprintf(stderr, "INFO:%s: insert in DB\n", mdata->id);
            /*
             * mdata->id : id (char)
             * mdata->titre : titre (char)
             * mdata->num : num (char)
             * mdata->date_publi : date_publi (date)
             * toc : structure_txt (json)
             * mdata->uri : uri (char)
             * mdata->uri_parts : uri_parts (json)
             */
            ct = clock();
            r = write_toc_json(-1, toc, wbuf, 1);
            if (r < 0) exit_nicely(conn);
            wbuf->buffer[wbuf->current_size] = 0;
            r = write_uri_parts_json(-1, &mdata->uri_parts, alt_buf, 1);
            if (r < 0) exit_nicely(conn);
            alt_buf->buffer[alt_buf->current_size] = 0;

            //printf("%s\n", alt_buf->buffer);
            param_values[0] = mdata->id;
            param_values[1] = mdata->titre;
            param_values[2] = mdata->num;
            param_values[3] = mdata->date_publi;
            param_values[4] = mdata->uri;
            param_values[5] = wbuf->buffer;
            param_values[6] = alt_buf->buffer;
            param_values[7] = stag;
            param_values[8] = "abc";
            param_values[9] = NULL;
            /* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
            res = PQexecParams(conn,
                               "INSERT INTO raw_jorflegi.import_jorf_container"
                               " (id, titre, num, date_publi, uri, structure_txt, uri_parts, tag, sig, mod_tm)"
                               " VALUES"
                               " ($1, $2, $3, $4, $5, $6, $7, $8, $9, NOW()::timestamp)"
                               " ON CONFLICT DO NOTHING",
                               9,
                               NULL,
                               param_values,
                               NULL,
                               NULL,
                               0);
            if (PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                fprintf(stderr, "INSERT failed: %s\n", PQresultErrorMessage(res));
                PQclear(res);
                exit_nicely(conn);
            }
            buffer_reset(wbuf);
            buffer_reset(alt_buf);
            PQclear(res);
            ct = clock() - ct;
            *tt = ((double)ct) / CLOCKS_PER_SEC;
            return 1;
            break;
        default:
            break;
    }
    /*
        PGresult *PQexecParams(PGconn *conn,
                           const char *command,
                           int nParams,
                           const Oid *paramTypes,
                           const char * const *paramValues,
                           const int *paramLengths,
                           const int *paramFormats,
                           int resultFormat);
                           */
    return 0;
}