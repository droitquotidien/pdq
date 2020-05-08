/*
 * PDQ - Droit Quotidien Parser
 *
 * Copyright (C) GA Silber, 2020
 */
#include "pdq.h"
#include <string.h>
/*
 * https://github.com/libarchive/libarchive/wiki/Examples#List_contents_of_Archive_stored_in_File
 */
int list_files(char *fname)
{
	struct archive *a;
	struct archive_entry *entry;
	int r;

	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
	r = archive_read_open_filename(a, fname, 10240);
	if (r != ARCHIVE_OK)
		return 1;
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		printf("%s\n", archive_entry_pathname(entry));
		archive_read_data_skip(a);
	}
	r = archive_read_free(a);
	if (r != ARCHIVE_OK)
		return 1;

	return 0;
}

int archive_filename(struct archive *a, struct archive_entry *entry)
{
	printf("%s\n", archive_entry_pathname(entry));
	archive_read_data_skip(a);
	return 0;
}

/*
 * JORFCONT
 * <STRUCTURE_TXT>
 *
 * JORFTEXT TEXTE_VERSION
 * <CID>JORFTEXT000041856940</CID>
 * <NUM_PARUTION>0113</NUM_PARUTION>
 * <NUM_SEQUENCE>15</NUM_SEQUENCE>
 * <NOR>ECOI1935482R</NOR>
 * <DATE_TEXTE>2020-05-07</DATE_TEXTE>
 * <ORIGINE_PUBLI>JORF n°0113 du 8 mai 2020</ORIGINE_PUBLI>
 * <PAGE_DEB_PUBLI/>
 * <PAGE_FIN_PUBLI/>
 * <TITREFULL>
 * <AUTORITE/>
 * <MINISTERE>Ministère de l'économie et des finances</MINISTERE>
 * <MCS_TXT/>
 * <LIENS/>
 * <NOTICE>
 * <VISAS>
 * <SIGNATAIRES>
 * <TP>
 * <ABRO>
 * <RECT>
 * <SM>
 * <ENTREPRISE>
 *
 * JORFTEXT TEXTELR
 * <VERSIONS>
 * <STRUCT>
 * 
 * JORFSCTA
 * <COMMENTAIRE/>
 * <CONTEXTE>
 * <STRUCTURE_TA>
 *
 * JORFARTI
 * <MCS_ART>
 * <CONTEXTE>
 * <VERSIONS>
 * <SM>
 * <BLOC_TEXTUEL>
 * <LIENS>
 */


#define FIELD_LEN_ID 20
static const xmlChar *FIELD_NAME_ID = BAD_CAST "ID";
#define FIELD_LEN_NATURE 60
static const xmlChar *FIELD_NAME_NATURE = BAD_CAST "NATURE";
#define FIELD_LEN_TYPE 30
static const xmlChar *FIELD_NAME_TYPE = BAD_CAST "TYPE";
#define FIELD_LEN_DATE 10
static const xmlChar *FIELD_NAME_DATE_DEBUT = BAD_CAST "DATE_DEBUT";
static const xmlChar *FIELD_NAME_DATE_FIN = BAD_CAST "DATE_FIN";
static const xmlChar *FIELD_NAME_DATE_PUBLI = BAD_CAST "DATE_PUBLI";
#define FIELD_LEN_NUM 60
static const xmlChar *FIELD_NAME_NUM = BAD_CAST "NUM";
#define FIELD_LEN_TITRE 1024
static const xmlChar *FIELD_NAME_TITRE = BAD_CAST "TITRE";
static const xmlChar *FIELD_NAME_TITRE_TA = BAD_CAST "TITRE_TA";

struct parsed_data {
    char id[FIELD_LEN_ID+1]; /* JORFARTI000037658324 + \0 */
    char nature[FIELD_LEN_NATURE+1]; /* DECRET + \0 */
    char type[FIELD_LEN_TYPE+1]; /* ENTIEREMENT_MODIF + \0 */
    char date_debut[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_fin[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char num[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char titre[FIELD_LEN_TITRE+1]; /* JORF n 0113 du 8 mai 2020 + \0 */
    char date_publi[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    int current_size;
    char *current_field;
    const xmlChar *current_name;
};

void print_parsed_data(struct parsed_data *pdata)
{
	printf("ID = %s\n", pdata->id);
	printf("NATURE = %s\n", pdata->nature);
	printf("TYPE = %s\n", pdata->type);
	printf("DATE_DEBUT = %s\n", pdata->date_debut);
	printf("DATE_FIN = %s\n", pdata->date_fin);
	printf("NUM = %s\n", pdata->num);
	printf("TITRE = %s\n", pdata->titre);
	printf("DATE_PUBLI = %s\n", pdata->date_publi);
}

void start_element_callback(void *user_data, const xmlChar *name, const xmlChar **attrs)
{
	struct parsed_data *pdata = user_data;

	/*
	printf("Beginning of element : %s \n", name);
	while (NULL != attrs && NULL != attrs[0]) {
		printf("attribute: %s=%s\n",attrs[0],attrs[1]);
		attrs = &attrs[2];
	}
	*/
	if (xmlStrEqual(name, FIELD_NAME_ID)) {
		pdata->current_field = pdata->id;
		pdata->current_size = FIELD_LEN_ID;
		pdata->current_name = FIELD_NAME_ID;
	}
	if (xmlStrEqual(name, FIELD_NAME_NATURE)) {
		pdata->current_field = pdata->nature;
		pdata->current_size = FIELD_LEN_NATURE;
		pdata->current_name = FIELD_NAME_NATURE;
	}
	if (xmlStrEqual(name, FIELD_NAME_TYPE)) {
		pdata->current_field = pdata->type;
		pdata->current_size = FIELD_LEN_TYPE;
		pdata->current_name = FIELD_NAME_TYPE;
	}
	if (xmlStrEqual(name, FIELD_NAME_DATE_DEBUT)) {
		pdata->current_field = pdata->date_debut;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_DEBUT;
	}
	if (xmlStrEqual(name, FIELD_NAME_DATE_FIN)) {
		pdata->current_field = pdata->date_fin;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_FIN;
	}
	if (xmlStrEqual(name, FIELD_NAME_NUM)) {
		pdata->current_field = pdata->num;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM;
	}
	if (xmlStrEqual(name, FIELD_NAME_TITRE)) {
		pdata->current_field = pdata->titre;
		pdata->current_size = FIELD_LEN_TITRE;
		pdata->current_name = FIELD_NAME_TITRE;
	}
	if (xmlStrEqual(name, FIELD_NAME_TITRE)) {
		pdata->current_field = pdata->titre;
		pdata->current_size = FIELD_LEN_TITRE;
		pdata->current_name = FIELD_NAME_TITRE;
	}
	if (xmlStrEqual(name, FIELD_NAME_DATE_PUBLI)) {
		pdata->current_field = pdata->date_publi;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_PUBLI;
	}
	if (xmlStrEqual(name, FIELD_NAME_TITRE_TA)) {
		pdata->current_field = pdata->titre;
		pdata->current_size = FIELD_LEN_TITRE;
		pdata->current_name = FIELD_NAME_TITRE_TA;
	}
}

void end_element_callback(void *user_data, const xmlChar *name)
{
	struct parsed_data *pdata = user_data;
	if (xmlStrEqual(name, FIELD_NAME_ID)
		|| xmlStrEqual(name, FIELD_NAME_NATURE)
		|| xmlStrEqual(name, FIELD_NAME_TYPE)
		|| xmlStrEqual(name, FIELD_NAME_DATE_DEBUT)
		|| xmlStrEqual(name, FIELD_NAME_DATE_FIN)
		|| xmlStrEqual(name, FIELD_NAME_NUM)
		|| xmlStrEqual(name, FIELD_NAME_TITRE)
		|| xmlStrEqual(name, FIELD_NAME_TITRE_TA)
		|| xmlStrEqual(name, FIELD_NAME_DATE_PUBLI)
		) {
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = NULL;
	}

}

void characters_callback(void *user_data, const xmlChar *chars, int len)
{
	struct parsed_data *pdata = user_data;
	if (pdata->current_field != NULL) {
		if (len <= pdata->current_size) {
			/*printf("COPY %s %d\n", pdata->current_name, len);*/
			memcpy(pdata->current_field, chars, len);
			pdata->current_field += len;
			pdata->current_size -= len;
		} else {
			fprintf(stderr, "no more size in current field\n");
		}
	}
}

int archive_show(struct archive *a, struct archive_entry *entry)
{
	int r;
	const void *buff;
	size_t len;
	off_t offset;
	xmlParserCtxtPtr ctxt;
	struct parsed_data pdata;
	xmlSAXHandler parser_handler = {0};

	memset(&pdata, 0, sizeof(pdata));

	parser_handler.startElement = start_element_callback;
	parser_handler.endElement = end_element_callback;
	parser_handler.characters = characters_callback;

	printf("%s\n", archive_entry_pathname(entry));
	/*printf("%lld\n", archive_entry_size(entry));*/
	ctxt = xmlCreatePushParserCtxt(&parser_handler, &pdata, NULL, 0, NULL);
	for (;;) {
		r = archive_read_data_block(a, &buff, &len, &offset);
		if (r == ARCHIVE_EOF) {
			print_parsed_data(&pdata);
			return 0;
		}
		if (r < ARCHIVE_OK)
			return r;
		/*printf("read len = %zu (offset = %lld)\n", len, offset);*/
		r = xmlParseChunk(ctxt, buff, len, 0);
		if (r != 0) {
			fprintf(stderr, "error reading XML chunk.\n");
			return r;
		}
	}
	/*
	int
	archive_read_data_block(struct archive *, const void **buff, size_t *len,
				off_t *offset);
				*/
	archive_read_data_skip(a);
}

int iterate_archive(char *fname, int (*f)(struct archive *, struct archive_entry *))
{
	struct archive *a;
	struct archive_entry *entry;
	int r;

	a = archive_read_new();
	archive_read_support_filter_all(a);
	archive_read_support_format_all(a);
	r = archive_read_open_filename(a, fname, 10240);
	if (r != ARCHIVE_OK)
		return 1;
	while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
		r = f(a, entry);
		if (r != 0)
			break;
	}
	if (r != 0)
		return 1;
	r = archive_read_free(a);
	if (r != ARCHIVE_OK)
		return 1;

	return 0;
}

/*int archive_read_data_block(struct archive *, const void **buff, size_t *len,
			off_t *offset);
*/

struct params {
	char *list_file;
	char *show_file;
};

void print_usage()
{
	printf("Usage: pdq [-l FILE] [-s FILE]\n");
}

int set_params(int argc, char *argv[], struct params *params)
{
	int c;
	memset(params, 0, sizeof(struct params));
	while((c = getopt(argc, argv, "l:s:")) != -1) {
		switch(c) {
			case 'l':
				params->list_file = optarg;
				break;
			case 's':
				params->show_file = optarg;
				break;
			case '?':
			default:
				return 1;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{

	struct params params;

	if (set_params(argc, argv, &params) == 1) {
		print_usage();
		return 1;
	}

	if (params.list_file != NULL) {
		/*list_files(params.list_file);*/
		iterate_archive(params.list_file, archive_filename);
		return 0;
	}
	if (params.show_file != NULL) {
		/*list_files(params.list_file);*/
		iterate_archive(params.show_file, archive_show);
		return 0;
	}
	/*LIBXML_TEST_VERSION*/
	/*xmlMemoryDump();*/

	return 0;
}
