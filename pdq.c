/*
 * PDQ - Droit Quotidien Parser
 *
 * Copyright (C) GA Silber, 2020
 */
#include <string.h>
#include "pdq.h"
#include "uri.h"
#include "parse.h"
#include "jorflegi.h"

struct archive_info {
    enum fund fund;
    int xml_files;
};

struct gen_uri_info {
    enum fund fund;
    int xml_files;
    struct parsed_data pdata;
    xmlSAXHandler parser_handler;
    xmlParserCtxtPtr ctxt;
};

int has_xml_suffix(const char *s, size_t size)
{
	return s != NULL && size >= 5 && s[size-5] != 's' && s[size-4] == '.' && s[size-3] == 'x'
		&& s[size-2] == 'm' && s[size-1] == 'l';
}

void set_base(const char *pathname, size_t size, char *base)
{
	if (pathname == NULL || size < 24)
		return;
	base[0] = pathname[size-24];
	base[1] = pathname[size-23];
	base[2] = pathname[size-22];
	base[3] = pathname[size-21];
	base[4] = 0;
}

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

int archive_filename(struct archive *a, struct archive_entry *entry, void *user_data)
{
	char base[5];
	const char *pathname;
	struct archive_info *infos = user_data;
	size_t size;

	pathname = archive_entry_pathname(entry);
	size = strlen(pathname);
	if (!has_xml_suffix(pathname, size)) {
		return 0;
	}
	set_base(pathname, size, base);
	infos->xml_files++;
	printf("%s %s\n", base, pathname);
	archive_read_data_skip(a);

	return 0;
}

/*
 * = JORFCONT_DOCTYPE
 * <STRUCTURE_TXT>
 *
 * = JORFTEXT_DOCTYPE TEXTE_VERSION
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
 * = JORFTEXT_DOCTYPE TEXTELR
 * <VERSIONS_DOCTYPE>
 * <STRUCT>
 *
 * = JORFSCTA_DOCTYPE
 * <COMMENTAIRE/>
 * <CONTEXTE>
 * <STRUCTURE_TA>
 *
 * = JORFARTI_DOCTYPE
 * <MCS_ART>
 * <CONTEXTE>
 * <VERSIONS_DOCTYPE>
 * <SM>
 * <BLOC_TEXTUEL>
 * <LIENS>
 *
 * = LEGITEXT_DOCTYPE TEXTE_VERSION
 * <LIENS/>
 * <VISAS>
 * <SIGNATAIRES>
 * <TP>
 * <NOTA>
 * <ABRO>
 * <RECT>
 *
 * = LEGITEXT_DOCTYPE TEXTELR
 * <VERSIONS_A_VENIR>
 * <VERSIONS_DOCTYPE>
 * <STRUCT>
 *
 * = LEGISCTA_DOCTYPE
 * <CONTEXTE>
 * <STRUCTURE_TA>
 *
 * = LEGIARTI_DOCTYPE
 * <CONTEXTE>
 * <NOTA>
 * <BLOC_TEXTUEL>
 * <LIENS>
 */

static const xmlChar *ROOT_JORFARTI = BAD_CAST "ARTICLE";
static const xmlChar *ROOT_JORFCONT = BAD_CAST "JO";
static const xmlChar *ROOT_JORFSCTA = BAD_CAST "SECTION_TA";
static const xmlChar *ROOT_JORFTEXT = BAD_CAST "TEXTELR";
static const xmlChar *ROOT_JORFVERS = BAD_CAST "TEXTE_VERSION";
static const xmlChar *ROOT_LEGIARTI = BAD_CAST "ARTICLE";
static const xmlChar *ROOT_LEGISCTA = BAD_CAST "SECTION_TA";
static const xmlChar *ROOT_LEGITEXT = BAD_CAST "TEXTELR";
static const xmlChar *ROOT_LEGIVERS = BAD_CAST "TEXTE_VERSION";
static const xmlChar *ROOT_VERSIONS = BAD_CAST "VERSIONS";
static const xmlChar *ROOT_ID = BAD_CAST "ID";

static const xmlChar *FIELD_NAME_ID = BAD_CAST "ID";
static const xmlChar *FIELD_NAME_CID = BAD_CAST "CID";
static const xmlChar *FIELD_NAME_NOR = BAD_CAST "NOR";
static const xmlChar *FIELD_NAME_NATURE = BAD_CAST "NATURE";
static const xmlChar *FIELD_NAME_TYPE = BAD_CAST "TYPE";
static const xmlChar *FIELD_NAME_DATE_DEBUT = BAD_CAST "DATE_DEBUT";
static const xmlChar *FIELD_NAME_DATE_FIN = BAD_CAST "DATE_FIN";
static const xmlChar *FIELD_NAME_DATE_PUBLI = BAD_CAST "DATE_PUBLI";
static const xmlChar *FIELD_NAME_DATE_TEXTE = BAD_CAST "DATE_TEXTE";
static const xmlChar *FIELD_NAME_DERNIERE_MODIFICATION = BAD_CAST "DERNIERE_MODIFICATION";
static const xmlChar *FIELD_NAME_NUM = BAD_CAST "NUM";
static const xmlChar *FIELD_NAME_NUM_PARUTION = BAD_CAST "NUM_PARUTION";
static const xmlChar *FIELD_NAME_NUM_SEQUENCE = BAD_CAST "NUM_SEQUENCE";
static const xmlChar *FIELD_NAME_TITRE = BAD_CAST "TITRE";
static const xmlChar *FIELD_NAME_TITREFULL = BAD_CAST "TITREFULL";
static const xmlChar *FIELD_NAME_TITRE_TA = BAD_CAST "TITRE_TA";
static const xmlChar *FIELD_NAME_ORIGINE_PUBLI = BAD_CAST "ORIGINE_PUBLI";
static const xmlChar *FIELD_NAME_PAGE_DEB_PUBLI = BAD_CAST "PAGE_DEB_PUBLI";
static const xmlChar *FIELD_NAME_PAGE_FIN_PUBLI = BAD_CAST "PAGE_FIN_PUBLI";
static const xmlChar *FIELD_NAME_AUTORITE = BAD_CAST "AUTORITE";
static const xmlChar *FIELD_NAME_MINISTERE = BAD_CAST "MINISTERE";
static const xmlChar *FIELD_NAME_ETAT = BAD_CAST "ETAT";
static const xmlChar *FIELD_NAME_TEXTE = BAD_CAST "TEXTE";
static const xmlChar *FIELD_NAME_TITRE_TXT = BAD_CAST "TITRE_TXT";
static const xmlChar *FIELD_NAME_ORIGINE = BAD_CAST "ORIGINE";

int fprintf_parsed_data(FILE *f, struct parsed_data *pdata)
{
	int r = 0;
	//printf("fund = %d\n", pdata->uri_parts.fund);
	//printf("doctype = %d\n", pdata->uri_parts.doctype);
	//printf("uri_kind = %d\n", pdata->uri_parts.kind);
	/* ID;base;nature;PCID;uri_kind;uri;num1;num2;num3 */
	r += fprintf_doctype(f, pdata->uri_parts.doctype);
	r += fprintf(f, "%s;%s;%s;%s;%d;%s;%d;%s;%d;%s;%d;%s\n", pdata->id + 8,
		pdata->uri_parts.base,
		pdata->nature,
		(*pdata->texte_cid == 0?(*pdata->cid == 0?"":pdata->cid):pdata->texte_cid),
		pdata->uri_parts.kind,
		pdata->uri,
		pdata->uri_parts.num1kind,
		(pdata->uri_parts.num1kind == EMPTY_NUMKIND?"":pdata->uri_parts.num1),
		pdata->uri_parts.num2kind,
		(pdata->uri_parts.num2kind == EMPTY_NUMKIND?"":pdata->uri_parts.num2),
		pdata->uri_parts.num3kind,
		(pdata->uri_parts.num3kind == EMPTY_NUMKIND?"":pdata->uri_parts.num3));
	/*
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
	 */
	return r;
}

int copy_attr_to_field(char *id, const xmlChar *attr_name, const xmlChar *attr_value, int len,
	char *field_name, char *field, int field_size)
{
	if (len <= field_size) {
		memcpy(field, attr_value, len);
	} else {
		fprintf(stderr, "critical:%s: current field %s too small for attr %s (%d) [%.*s]\n",
			id, field_name, attr_name, len - field_size, len, attr_value);
		return -1;
	}
	return len;
}

void start_element_callback(void *user_data, const xmlChar *name, const xmlChar **attrs)
{
	struct parsed_data *pdata = user_data;
	int r;

	if (pdata->uri_parts.doctype == EMPTY_DOCTYPE) {
		if (strcmp(pdata->uri_parts.base, "JORF") == 0) {
			if (xmlStrEqual(name, ROOT_JORFCONT)) {
				pdata->uri_parts.doctype = JORFCONT_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_JORFTEXT)) {
				pdata->uri_parts.doctype = JORFTEXT_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_JORFVERS)) {
				pdata->uri_parts.doctype = JORFVERS_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_JORFSCTA)) {
				pdata->uri_parts.doctype = JORFSCTA_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_JORFARTI)) {
				pdata->uri_parts.doctype = JORFARTI_DOCTYPE;
			} else {
				fprintf(stderr, "Unknown root for XML file: %s\n",
					(char *)name);
				exit(1);
			}
		} else if (strcmp(pdata->uri_parts.base, "LEGI") == 0) {
			if (xmlStrEqual(name, ROOT_LEGITEXT)) {
				pdata->uri_parts.doctype = LEGITEXT_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_LEGIVERS)) {
				pdata->uri_parts.doctype = LEGIVERS_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_LEGISCTA)) {
				pdata->uri_parts.doctype = LEGISCTA_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_LEGIARTI)) {
				pdata->uri_parts.doctype = LEGIARTI_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_VERSIONS)) {
				pdata->uri_parts.doctype = VERSIONS_DOCTYPE;
			} else {
				fprintf(stderr, "Unknown root for XML file: %s\n",
					(char *) name);
				exit(1);
			}
		} else {
			if (xmlStrEqual(name, ROOT_ID)) {
				pdata->uri_parts.doctype = VERSIONS_ID_DOCTYPE;
			}
		}
	}
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
	} else if (xmlStrEqual(name, FIELD_NAME_CID)) {
		pdata->current_field = pdata->cid;
		pdata->current_size = FIELD_LEN_ID;
		pdata->current_name = FIELD_NAME_CID;
	} else if (xmlStrEqual(name, FIELD_NAME_NATURE)) {
		pdata->current_field = pdata->nature;
		pdata->current_size = FIELD_LEN_NATURE;
		pdata->current_name = FIELD_NAME_NATURE;
	} else if (xmlStrEqual(name, FIELD_NAME_TYPE)) {
		pdata->current_field = pdata->type;
		pdata->current_size = FIELD_LEN_TYPE;
		pdata->current_name = FIELD_NAME_TYPE;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_DEBUT)) {
		pdata->current_field = pdata->date_debut;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_DEBUT;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_FIN)) {
		pdata->current_field = pdata->date_fin;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_FIN;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM)) {
		pdata->current_field = pdata->num;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM;
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE)) {
		pdata->current_field = pdata->titre;
		pdata->current_size = FIELD_LEN_TITRE;
		pdata->current_name = FIELD_NAME_TITRE;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_PUBLI)) {
		pdata->current_field = pdata->date_publi;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE_TA)) {
		pdata->current_field = pdata->titrefull;
		pdata->current_size = FIELD_LEN_TITREFULL;
		pdata->current_name = FIELD_NAME_TITRE_TA;
	} else if (xmlStrEqual(name, FIELD_NAME_NOR)) {
		pdata->current_field = pdata->nor;
		pdata->current_size = FIELD_LEN_NOR;
		pdata->current_name = FIELD_NAME_NOR;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_TEXTE)) {
		pdata->current_field = pdata->date_texte;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_TEXTE;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM_PARUTION)) {
		pdata->current_field = pdata->num_parution;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM_PARUTION;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM_SEQUENCE)) {
		pdata->current_field = pdata->num_sequence;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM_SEQUENCE;
	} else if (xmlStrEqual(name, FIELD_NAME_ORIGINE_PUBLI)) {
		pdata->current_field = pdata->origine_publi;
		pdata->current_size = FIELD_LEN_ORIGINE_PUBLI;
		pdata->current_name = FIELD_NAME_ORIGINE_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_PAGE_DEB_PUBLI)) {
		pdata->current_field = pdata->page_deb_publi;
		pdata->current_size = FIELD_LEN_PAGE;
		pdata->current_name = FIELD_NAME_PAGE_DEB_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_PAGE_FIN_PUBLI)) {
		pdata->current_field = pdata->page_fin_publi;
		pdata->current_size = FIELD_LEN_PAGE;
		pdata->current_name = FIELD_NAME_PAGE_FIN_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_TITREFULL)) {
		pdata->current_field = pdata->titrefull;
		pdata->current_size = FIELD_LEN_TITREFULL;
		pdata->current_name = FIELD_NAME_TITREFULL;
	} else if (xmlStrEqual(name, FIELD_NAME_AUTORITE)) {
		pdata->current_field = pdata->autorite;
		pdata->current_size = FIELD_LEN_AUTORITE;
		pdata->current_name = FIELD_NAME_AUTORITE;
	} else if (xmlStrEqual(name, FIELD_NAME_MINISTERE)) {
		pdata->current_field = pdata->ministere;
		pdata->current_size = FIELD_LEN_AUTORITE;
		pdata->current_name = FIELD_NAME_MINISTERE;
	} else if (xmlStrEqual(name, FIELD_NAME_ETAT)) {
		pdata->current_field = pdata->etat;
		pdata->current_size = FIELD_LEN_ETAT;
		pdata->current_name = FIELD_NAME_ETAT;
	} else if (xmlStrEqual(name, FIELD_NAME_ORIGINE)) {
		pdata->current_field = pdata->origine;
		pdata->current_size = FIELD_LEN_ORIGINE;
		pdata->current_name = FIELD_NAME_ORIGINE;
	} else if (xmlStrEqual(name, FIELD_NAME_DERNIERE_MODIFICATION)) {
		pdata->current_field = pdata->derniere_modification;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DERNIERE_MODIFICATION;
	} else if (pdata->uri_parts.doctype == JORFSCTA_DOCTYPE
			|| pdata->uri_parts.doctype == JORFARTI_DOCTYPE
			|| pdata->uri_parts.doctype == LEGISCTA_DOCTYPE
			|| pdata->uri_parts.doctype == LEGIARTI_DOCTYPE) {
		if (xmlStrEqual(name, FIELD_NAME_TEXTE)) {
			while (NULL != attrs && NULL != attrs[0]) {
				r = 0;
				if (strcmp((const char *)attrs[0], "cid") == 0)
					r = copy_attr_to_field(pdata->id,
						attrs[0], attrs[1], xmlStrlen(attrs[1]),
						"texte_cid", pdata->texte_cid, FIELD_LEN_ID);
					//strcpy(pdata->texte_cid, (const char *)attrs[1]);
				else if (strcmp((const char *)attrs[0], "nature") == 0)
					r = copy_attr_to_field(pdata->id,
						attrs[0], attrs[1], xmlStrlen(attrs[1]),
						"texte_nature", pdata->texte_nature, FIELD_LEN_NATURE);
				else if (strcmp((const char *)attrs[0], "nor") == 0)
					r = copy_attr_to_field(pdata->id,
						attrs[0], attrs[1], xmlStrlen(attrs[1]),
						"texte_nor", pdata->texte_nor, FIELD_LEN_NOR);
				else if (strcmp((const char *)attrs[0], "num") == 0)
					r = copy_attr_to_field(pdata->id,
						attrs[0], attrs[1], xmlStrlen(attrs[1]),
						"texte_num", pdata->texte_num, FIELD_LEN_NUM);
				else if (strcmp((const char *)attrs[0], "date_signature") == 0)
					r = copy_attr_to_field(pdata->id,
						attrs[0], attrs[1], xmlStrlen(attrs[1]),
						"date_texte", pdata->date_texte, FIELD_LEN_DATE);
				else if (strcmp((const char *)attrs[0], "date_publi") == 0)
					r = copy_attr_to_field(pdata->id,
						attrs[0], attrs[1], xmlStrlen(attrs[1]),
						"date_publi", pdata->date_publi, FIELD_LEN_DATE);
				// TODO: que faire quand r < 0 ?
				attrs = &attrs[2];
			}
		} else if (xmlStrEqual(name, FIELD_NAME_TITRE_TXT)) {
			while (NULL != attrs && NULL != attrs[0]) {
				r = 0;
				if (strcmp((const char *)attrs[0], "debut") == 0)
					if (*pdata->texte_debut == 0) /* Set value only once (first text) */
						r = copy_attr_to_field(pdata->id,
							attrs[0], attrs[1], xmlStrlen(attrs[1]),
							"texte_debut", pdata->texte_debut, FIELD_LEN_DATE);
				// TODO: que faire quand r < 0 ?
				attrs = &attrs[2];
			}
		}
	}
	/* <CONTEXTE>
	 * <TEXTE cid="JORFTEXT000032974294" date_publi="2016-08-06" date_signature="2016-07-26"
	 * nature="ARRETE" nor="AFSS1621351A" num="" num_parution_jo="0182">
	*/
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
		|| xmlStrEqual(name, FIELD_NAME_ORIGINE)
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
			fprintf(stderr, "critical:%s: no more size in current field %s (%d) [%.*s]\n",
				pdata->id, pdata->current_name, len - pdata->current_size, len, chars);
			pdata->current_field = NULL;
			pdata->current_size = 0;
			pdata->current_name = NULL;
			pdata->status = 1;
		}
	}
}

int archive_show(struct archive *a, struct archive_entry *entry, void *user_data)
{
	int r;
	int uri_len;
	const void *buff;
	size_t len;
	off_t offset;
	xmlParserCtxtPtr ctxt;
	struct parsed_data *pdata;
	xmlSAXHandler parser_handler = {0};
	const char *fname;
	struct gen_uri_info *infos = user_data;
	size_t size;
	char base[5];

	fname = archive_entry_pathname(entry);
	size = strlen(fname);
	if (!has_xml_suffix(fname, size)) {
		return 0;
	}

	pdata = &infos->pdata;
	memset(pdata, 0, sizeof(infos->pdata));
	pdata->uri_parts.fund = infos->fund;
	set_base(fname, size, pdata->uri_parts.base);
	infos->xml_files++;
	ctxt = infos->ctxt;

	/*
	parser_handler.startElement = start_element_callback;
	parser_handler.endElement = end_element_callback;
	parser_handler.characters = characters_callback;
	*/
	//printf("%s\n", fname);
	//printf("%lld\n", archive_entry_size(entry));
	// ctxt = xmlCreatePushParserCtxt(&parser_handler, &pdata, NULL, 0, NULL);
	if (xmlCtxtResetPush(ctxt, NULL, 0, NULL, NULL) != 0) {
		fprintf(stderr, "cannot reset parser ctxt.\n");
		return -1;
	}
	for (;;) {
		r = archive_read_data_block(a, &buff, &len, &offset);
		if (r == ARCHIVE_EOF) {
			uri_len = set_jorflegi_uri(pdata);
			if (uri_len == -1) {
				fprintf_doctype(stdout, pdata->uri_parts.doctype);
				fprintf(stdout, "%s ERROR\n", pdata->id + 8);
				// return -1;
			} else if (uri_len == 0) {
				fprintf_doctype(stdout, pdata->uri_parts.doctype);
				fprintf(stdout, "%s IGNORED\n", pdata->id + 8);
			} else {
				if (pdata->uri_parts.kind != EMPTY_URI_KIND) {
					uri_cpy(&pdata->uri_parts, pdata->uri);
				}
				fprintf_parsed_data(stdout, pdata);
			}
			//xmlFreeParserCtxt(ctxt);
			return 0;
		}
		if (r < ARCHIVE_OK)
			return r;
		//printf("read len = %zu (offset = %lld)\n", len, offset);
		r = xmlParseChunk(ctxt, buff, len, 0);
		if (r != 0) {
			fprintf(stderr, "error reading XML chunk.\n");
			return r;
		}
	}
}

int iterate_archive(char *fname, int (*f)(struct archive *, struct archive_entry *, void *),
	void *user_data)
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
		r = f(a, entry, user_data);
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
	if (argc < 2) {
		return 1;
	}
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


int generate_uris(char *fname, enum fund fund)
{
	int r;
	struct gen_uri_info infos = {0};
	xmlSAXHandler parser_handler = {0};

	parser_handler.startElement = start_element_callback;
	parser_handler.endElement = end_element_callback;
	parser_handler.characters = characters_callback;

	infos.ctxt = xmlCreatePushParserCtxt(&parser_handler, &infos.pdata, NULL, 0, NULL);
	infos.fund = fund;

	r = iterate_archive(fname, archive_show, &infos);

	xmlFreeParserCtxt(infos.ctxt);

	return r;
}

int main(int argc, char **argv)
{
	int r = 0;
	struct params params;
	struct archive_info infos = {0};

	infos.fund = JORFLEGI_FUND;

	if (set_params(argc, argv, &params) == 1) {
		print_usage();
		return 1;
	}

	if (params.list_file != NULL) {
		/*list_files(params.list_file);*/
		iterate_archive(params.list_file, archive_filename, &infos);
	}
	if (params.show_file != NULL) {
		/*list_files(params.list_file);*/
		/*LIBXML_TEST_VERSION*/
		/* TODO: fund should be a parameter */
		r = generate_uris(params.show_file, JORFLEGI_FUND);
		//r = iterate_archive(params.show_file, archive_show, &infos);
	}
	/*LIBXML_TEST_VERSION*/
	/*xmlMemoryDump();*/

	/*fprintf(stderr, "%d xml files\n", infos.xml_files);*/

	return r;
}
