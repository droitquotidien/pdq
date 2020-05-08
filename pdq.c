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
 * = JORFCONT
 * <STRUCTURE_TXT>
 *
 * = JORFTEXT TEXTE_VERSION
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
 * = JORFTEXT TEXTELR
 * <VERSIONS>
 * <STRUCT>
 *
 * = JORFSCTA
 * <COMMENTAIRE/>
 * <CONTEXTE>
 * <STRUCTURE_TA>
 *
 * = JORFARTI
 * <MCS_ART>
 * <CONTEXTE>
 * <VERSIONS>
 * <SM>
 * <BLOC_TEXTUEL>
 * <LIENS>
 *
 * = LEGITEXT TEXTE_VERSION
 * <LIENS/>
 * <VISAS>
 * <SIGNATAIRES>
 * <TP>
 * <NOTA>
 * <ABRO>
 * <RECT>
 *
 * = LEGITEXT TEXTELR
 * <VERSIONS_A_VENIR>
 * <VERSIONS>
 * <STRUCT>
 *
 * = LEGISCTA
 * <CONTEXTE>
 * <STRUCTURE_TA>
 *
 * = LEGIARTI
 * <CONTEXTE>
 * <NOTA>
 * <BLOC_TEXTUEL>
 * <LIENS>
 */


#define FIELD_LEN_ID 20
static const xmlChar *FIELD_NAME_ID = BAD_CAST "ID";
static const xmlChar *FIELD_NAME_CID = BAD_CAST "CID";
#define FIELD_LEN_NOR 15
static const xmlChar *FIELD_NAME_NOR = BAD_CAST "NOR";
#define FIELD_LEN_NATURE 60
static const xmlChar *FIELD_NAME_NATURE = BAD_CAST "NATURE";
#define FIELD_LEN_TYPE 30
static const xmlChar *FIELD_NAME_TYPE = BAD_CAST "TYPE";
#define FIELD_LEN_DATE 10
static const xmlChar *FIELD_NAME_DATE_DEBUT = BAD_CAST "DATE_DEBUT";
static const xmlChar *FIELD_NAME_DATE_FIN = BAD_CAST "DATE_FIN";
static const xmlChar *FIELD_NAME_DATE_PUBLI = BAD_CAST "DATE_PUBLI";
static const xmlChar *FIELD_NAME_DATE_TEXTE = BAD_CAST "DATE_TEXTE";
static const xmlChar *FIELD_NAME_DERNIERE_MODIFICATION = BAD_CAST "DERNIERE_MODIFICATION";
#define FIELD_LEN_NUM 60
static const xmlChar *FIELD_NAME_NUM = BAD_CAST "NUM";
static const xmlChar *FIELD_NAME_NUM_PARUTION = BAD_CAST "NUM_PARUTION";
static const xmlChar *FIELD_NAME_NUM_SEQUENCE = BAD_CAST "NUM_SEQUENCE";
#define FIELD_LEN_TITRE 1024
static const xmlChar *FIELD_NAME_TITRE = BAD_CAST "TITRE";
#define FIELD_LEN_TITREFULL 2048
static const xmlChar *FIELD_NAME_TITREFULL = BAD_CAST "TITREFULL";
static const xmlChar *FIELD_NAME_TITRE_TA = BAD_CAST "TITRE_TA";
#define FIELD_LEN_ORIGINE_PUBLI 192
static const xmlChar *FIELD_NAME_ORIGINE_PUBLI = BAD_CAST "ORIGINE_PUBLI";
#define FIELD_LEN_PAGE 8
static const xmlChar *FIELD_NAME_PAGE_DEB_PUBLI = BAD_CAST "PAGE_DEB_PUBLI";
static const xmlChar *FIELD_NAME_PAGE_FIN_PUBLI = BAD_CAST "PAGE_FIN_PUBLI";
#define FIELD_LEN_AUTORITE 256
static const xmlChar *FIELD_NAME_AUTORITE = BAD_CAST "AUTORITE";
static const xmlChar *FIELD_NAME_MINISTERE = BAD_CAST "MINISTERE";
#define FIELD_LEN_ETAT 64
static const xmlChar *FIELD_NAME_ETAT = BAD_CAST "ETAT";

struct parsed_data {
    char id[FIELD_LEN_ID+1]; /* JORFARTI000037658324 + \0 */
    char cid[FIELD_LEN_ID+1]; /* JORFTEXT000041856940 + \0 */
    char nature[FIELD_LEN_NATURE+1]; /* DECRET + \0 */
    char type[FIELD_LEN_TYPE+1]; /* ENTIEREMENT_MODIF + \0 */
    char date_debut[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_fin[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_publi[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_texte[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char num[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char num_parution[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char num_sequence[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char titre[FIELD_LEN_TITRE+1]; /* JORF n 0113 du 8 mai 2020 + \0 */
    char titrefull[FIELD_LEN_TITREFULL+1]; /* JORF n 0113 du 8 mai 2020 + \0 */
    char nor[FIELD_LEN_NOR+1]; /* ECOI1935482R + \0 */
    char origine_publi[FIELD_LEN_ORIGINE_PUBLI+1]; /* ECOI1935482R + \0 */
    char page_deb_publi[FIELD_LEN_PAGE+1]; /* 12 + \0 */
    char page_fin_publi[FIELD_LEN_PAGE+1]; /* 13 + \0 */
    char autorite[FIELD_LEN_AUTORITE+1]; /* Autorité... + \0 */
    char ministere[FIELD_LEN_AUTORITE+1]; /* Ministère de... + \0 */
    char etat[FIELD_LEN_ETAT+1];
    char derniere_modification[FIELD_LEN_DATE+1];

    int current_size;
    char *current_field;
    const xmlChar *current_name;
    int status;
};

void print_parsed_data(struct parsed_data *pdata)
{
	printf("ID = %s\n", pdata->id);
	printf("CID = %s\n", pdata->cid);
	printf("NATURE = %s\n", pdata->nature);
	printf("TYPE = %s\n", pdata->type);
	printf("DATE_DEBUT = %s\n", pdata->date_debut);
	printf("DATE_FIN = %s\n", pdata->date_fin);
	printf("NUM = %s\n", pdata->num);
	printf("TITRE = %s\n", pdata->titre);
	printf("DATE_PUBLI = %s\n", pdata->date_publi);
	printf("NOR = %s\n", pdata->nor);
	printf("DATE_TEXTE = %s\n", pdata->date_texte);
	printf("NUM_PARUTION = %s\n", pdata->num_parution);
	printf("NUM_SEQUENCE = %s\n", pdata->num_sequence);
	printf("ORIGINE_PUBLI = %s\n", pdata->origine_publi);
	printf("PAGE_DEB_PUBLI = %s\n", pdata->page_deb_publi);
	printf("PAGE_FIN_PUBLI = %s\n", pdata->page_fin_publi);
	printf("TITREFULL = %s\n", pdata->titrefull);
	printf("AUTORITE = %s\n", pdata->autorite);
	printf("MINISTERE = %s\n", pdata->ministere);
	printf("ETAT = %s\n", pdata->etat);
	printf("DERNIERE_MODIFICATION = %s\n", pdata->derniere_modification);
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
	if (xmlStrEqual(name, FIELD_NAME_CID)) {
		pdata->current_field = pdata->cid;
		pdata->current_size = FIELD_LEN_ID;
		pdata->current_name = FIELD_NAME_CID;
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
	if (xmlStrEqual(name, FIELD_NAME_DATE_PUBLI)) {
		pdata->current_field = pdata->date_publi;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_PUBLI;
	}
	if (xmlStrEqual(name, FIELD_NAME_TITRE_TA)) {
		pdata->current_field = pdata->titrefull;
		pdata->current_size = FIELD_LEN_TITREFULL;
		pdata->current_name = FIELD_NAME_TITRE_TA;
	}
	if (xmlStrEqual(name, FIELD_NAME_NOR)) {
		pdata->current_field = pdata->nor;
		pdata->current_size = FIELD_LEN_NOR;
		pdata->current_name = FIELD_NAME_NOR;
	}
	if (xmlStrEqual(name, FIELD_NAME_DATE_TEXTE)) {
		pdata->current_field = pdata->date_texte;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_TEXTE;
	}
	if (xmlStrEqual(name, FIELD_NAME_NUM_PARUTION)) {
		pdata->current_field = pdata->num_parution;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM_PARUTION;
	}
	if (xmlStrEqual(name, FIELD_NAME_NUM_SEQUENCE)) {
		pdata->current_field = pdata->num_sequence;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM_SEQUENCE;
	}
	if (xmlStrEqual(name, FIELD_NAME_ORIGINE_PUBLI)) {
		pdata->current_field = pdata->origine_publi;
		pdata->current_size = FIELD_LEN_ORIGINE_PUBLI;
		pdata->current_name = FIELD_NAME_ORIGINE_PUBLI;
	}
	if (xmlStrEqual(name, FIELD_NAME_PAGE_DEB_PUBLI)) {
		pdata->current_field = pdata->page_deb_publi;
		pdata->current_size = FIELD_LEN_PAGE;
		pdata->current_name = FIELD_NAME_PAGE_DEB_PUBLI;
	}
	if (xmlStrEqual(name, FIELD_NAME_PAGE_FIN_PUBLI)) {
		pdata->current_field = pdata->page_fin_publi;
		pdata->current_size = FIELD_LEN_PAGE;
		pdata->current_name = FIELD_NAME_PAGE_FIN_PUBLI;
	}
	if (xmlStrEqual(name, FIELD_NAME_TITREFULL)) {
		pdata->current_field = pdata->titrefull;
		pdata->current_size = FIELD_LEN_TITREFULL;
		pdata->current_name = FIELD_NAME_TITREFULL;
	}
	if (xmlStrEqual(name, FIELD_NAME_AUTORITE)) {
		pdata->current_field = pdata->autorite;
		pdata->current_size = FIELD_LEN_AUTORITE;
		pdata->current_name = FIELD_NAME_AUTORITE;
	}
	if (xmlStrEqual(name, FIELD_NAME_MINISTERE)) {
		pdata->current_field = pdata->ministere;
		pdata->current_size = FIELD_LEN_AUTORITE;
		pdata->current_name = FIELD_NAME_MINISTERE;
	}
	if (xmlStrEqual(name, FIELD_NAME_ETAT)) {
		pdata->current_field = pdata->etat;
		pdata->current_size = FIELD_LEN_ETAT;
		pdata->current_name = FIELD_NAME_ETAT;
	}
	if (xmlStrEqual(name, FIELD_NAME_DERNIERE_MODIFICATION)) {
		pdata->current_field = pdata->derniere_modification;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DERNIERE_MODIFICATION;
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
		|| xmlStrEqual(name, FIELD_NAME_CID)
		|| xmlStrEqual(name, FIELD_NAME_NOR)
		|| xmlStrEqual(name, FIELD_NAME_DATE_TEXTE)
		|| xmlStrEqual(name, FIELD_NAME_NUM_PARUTION)
		|| xmlStrEqual(name, FIELD_NAME_NUM_SEQUENCE)
		|| xmlStrEqual(name, FIELD_NAME_ORIGINE_PUBLI)
		|| xmlStrEqual(name, FIELD_NAME_PAGE_DEB_PUBLI)
		|| xmlStrEqual(name, FIELD_NAME_PAGE_FIN_PUBLI)
		|| xmlStrEqual(name, FIELD_NAME_TITREFULL)
		|| xmlStrEqual(name, FIELD_NAME_AUTORITE)
		|| xmlStrEqual(name, FIELD_NAME_MINISTERE)
		|| xmlStrEqual(name, FIELD_NAME_ETAT)
		|| xmlStrEqual(name, FIELD_NAME_DERNIERE_MODIFICATION)
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
			fprintf(stderr, "no more size in current field %s (%d)\n",
				pdata->current_name, len - pdata->current_size);
			fprintf(stderr, "c: %.*s\n", len, chars);
			pdata->current_field = NULL;
			pdata->current_size = 0;
			pdata->current_name = NULL;
			pdata->status = 1;
		}
	}
}

int has_xml_suffix(const char *s)
{
	int ret = 0;

	if (s != NULL) {
		size_t size = strlen(s);

		if (size >= 4 &&
		    s[size-4] == '.' &&
		    s[size-3] == 'x' &&
		    s[size-2] == 'm' &&
		    s[size-1] == 'l') {
			ret = 1;
		}
	}

	return ret;
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
	const char *fname;

	fname = archive_entry_pathname(entry);
	if (!has_xml_suffix(fname)) {
		return 0;
	}
	memset(&pdata, 0, sizeof(pdata));

	parser_handler.startElement = start_element_callback;
	parser_handler.endElement = end_element_callback;
	parser_handler.characters = characters_callback;

	printf("%s\n", fname);
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
		LIBXML_TEST_VERSION
		iterate_archive(params.show_file, archive_show);
		return 0;
	}
	/*LIBXML_TEST_VERSION*/
	/*xmlMemoryDump();*/

	return 0;
}
