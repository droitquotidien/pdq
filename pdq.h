#ifndef PDQ_H
#define PDQ_H
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <archive.h>
#include <archive_entry.h>
#include "jorflegi.h"
#include "db.h"
#include "signature.h"

#define VERSION "0.8"

struct gen_uri_info {
    enum fund fund;
    int xml_files;
    struct parsed_data *pdata;
    xmlSAXHandler parser_handler;
    xmlParserCtxtPtr ctxt;
    struct write_buffer *wbuf;
    struct write_buffer *dbbuf1;
    struct write_buffer *dbbuf2;
    struct write_buffer *dbbuf3;
    struct write_buffer *dbbuf4;
    struct write_buffer *dbbuf5;
    struct write_buffer *dbbuf6;
    char bootstrap;
    char force;
    char *target_dir;
    char *data_file;
    FILE *log_file;
    PGconn *pg_conn;
    const EVP_MD *sig_gen;
    struct tm ts;
    struct timings tt;
};

#endif /* PDQ_H */
