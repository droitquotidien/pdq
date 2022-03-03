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
    struct parsed_data *pdata;
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
static const xmlChar *FIELD_NAME_VERSION_A_VENIR = BAD_CAST "VERSION_A_VENIR";
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
static const xmlChar *FIELD_NAME_VERSION = BAD_CAST "VERSION";
static const xmlChar *FIELD_NAME_LIEN_TXT = BAD_CAST "LIEN_TXT";
static const xmlChar *FIELD_NAME_LIEN_ART = BAD_CAST "LIEN_ART";
static const xmlChar *FIELD_NAME_LIEN_SECTION_TA = BAD_CAST "LIEN_SECTION_TA";
static const xmlChar *FIELD_NAME_LIENS = BAD_CAST "LIENS";
static const xmlChar *FIELD_NAME_LIEN = BAD_CAST "LIEN";
static const xmlChar *FIELD_NAME_STRUCT = BAD_CAST "STRUCT";
static const xmlChar *FIELD_NAME_STRUCTURE_TA = BAD_CAST "STRUCTURE_TA";

int fprintf_parsed_data(FILE *f, struct parsed_data *pdata)
{
	int r = 0;
	int i;
	struct metadata *mdata = pdata->metadata;
	struct versions *versions = pdata->versions;
	struct liens *liens = pdata->liens;
	struct toc *toc = pdata->toc;
	struct document_version *docversion;
	struct lien *lien;
	struct tocitem *tocitem;

	r += fprintf_doctype(f, mdata->uri_parts.doctype);
	r += fprintf(f, "%s;%s;%s;%s;%d;%s;%d;%s;%d;%s;%d;%s\n", mdata->id + 8,
		mdata->uri_parts.base,
		mdata->nature,
		(*mdata->contexte.cid == 0?(*mdata->cid == 0?"":mdata->cid):mdata->contexte.cid),
		mdata->uri_parts.kind,
		mdata->uri,
		mdata->uri_parts.num1kind,
		(mdata->uri_parts.num1kind == EMPTY_NUMKIND?"":mdata->uri_parts.num1),
		mdata->uri_parts.num2kind,
		(mdata->uri_parts.num2kind == EMPTY_NUMKIND?"":mdata->uri_parts.num2),
		mdata->uri_parts.num3kind,
		(mdata->uri_parts.num3kind == EMPTY_NUMKIND?"":mdata->uri_parts.num3));
	for (i = 0; i < versions->nb_versions; i++) {
		docversion = &versions->versions[i];
		fprintf(f, "\tversions.version;%s;%s;%s;%s;%s\n",
			docversion->id, docversion->date_debut, docversion->date_fin,
			docversion->etat, docversion->num);
	}
	for (i = 0; i < liens->nb_liens; i++) {
		lien = &liens->liens[i];
		fprintf(f, "\tliens.lien;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
			lien->cid_texte, lien->date_signature_texte, lien->nature_texte,
			lien->nor_texte, lien->num_texte,
			lien->id, lien->num, lien->sens, lien->typelien, lien->titre);
	}
	for (i = 0; i < toc->nb_tocitems; i++) {
		tocitem = &toc->tocitems[i];
		fprintf(f, "\ttoc.tocitem;%d;%d;%s;%s;%s;%s;%s;%s\n",
			tocitem->niv, tocitem->kind, tocitem->id, tocitem->cid,
			tocitem->date_debut, tocitem->date_fin,
			tocitem->etat, tocitem->titrefull);
	}

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

void reset_current(struct parsed_data *pdata)
{
	pdata->current_field = NULL;
	pdata->current_size = 0;
	pdata->current_name = NULL;
	pdata->parent_element = PE_EMPTY;
}

struct parsed_data *allocate_parsed_data()
{
	struct parsed_data *pdata;

	pdata = calloc(1, sizeof(struct parsed_data));
	if (pdata == NULL) {
		perror("calloc parsed_data");
		return NULL;
	}
	pdata->metadata = calloc(1, sizeof(struct metadata));
	if (pdata->metadata == NULL) {
		perror("calloc parsed_data->metadata");
		return NULL;
	}
	pdata->versions = calloc(1, sizeof(struct versions));
	if (pdata->versions == NULL) {
		perror("calloc parsed_data->versions");
		return NULL;
	}
	pdata->versions->max_versions = MAX_VERSIONS;
	pdata->liens = calloc(1, sizeof(struct liens));
	if (pdata->liens == NULL) {
		perror("calloc parsed_data->liens");
		return NULL;
	}
	pdata->liens->max_liens = MAX_LIENS;
	pdata->toc = calloc(1, sizeof(struct toc));
	if (pdata->toc == NULL) {
		perror("calloc parsed_data->toc");
		return NULL;
	}
	pdata->toc->max_tocitems = MAX_TOCITEMS;

	return pdata;
}

void free_parsed_data(struct parsed_data *pdata)
{
	free(pdata->metadata);
	free(pdata->versions);
	free(pdata->liens);
	free(pdata->toc);
	free(pdata);
}

void reset_parsed_data(struct parsed_data *pdata)
{
	reset_current(pdata);
	memset(pdata->metadata, 0, sizeof(struct metadata));
	pdata->liens->nb_liens = 0;
	pdata->versions->nb_versions = 0;
}

void start_element_callback(void *user_data, const xmlChar *name, const xmlChar **attrs)
{
	struct parsed_data *pdata = user_data;
	struct metadata *mdata;
	struct versions *versions;
	struct liens *liens;
	struct document_version *docversion;
	struct lien *lien;
	int r;

	mdata = pdata->metadata;
	versions = pdata->versions;
	liens = pdata->liens;

	if (mdata->uri_parts.doctype == EMPTY_DOCTYPE) {
		if (strcmp(mdata->uri_parts.base, "JORF") == 0) {
			if (xmlStrEqual(name, ROOT_JORFCONT)) {
				mdata->uri_parts.doctype = JORFCONT_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_JORFTEXT)) {
				mdata->uri_parts.doctype = JORFTEXT_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_JORFVERS)) {
				mdata->uri_parts.doctype = JORFVERS_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_JORFSCTA)) {
				mdata->uri_parts.doctype = JORFSCTA_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_JORFARTI)) {
				mdata->uri_parts.doctype = JORFARTI_DOCTYPE;
			} else {
				fprintf(stderr, "Unknown root for XML file: %s\n",
					(char *)name);
				exit(1);
			}
		} else if (strcmp(mdata->uri_parts.base, "LEGI") == 0) {
			if (xmlStrEqual(name, ROOT_LEGITEXT)) {
				mdata->uri_parts.doctype = LEGITEXT_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_LEGIVERS)) {
				mdata->uri_parts.doctype = LEGIVERS_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_LEGISCTA)) {
				mdata->uri_parts.doctype = LEGISCTA_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_LEGIARTI)) {
				mdata->uri_parts.doctype = LEGIARTI_DOCTYPE;
			} else if (xmlStrEqual(name, ROOT_VERSIONS)) {
				mdata->uri_parts.doctype = VERSIONS_DOCTYPE;
			} else {
				fprintf(stderr, "Unknown root for XML file: %s\n",
					(char *) name);
				exit(1);
			}
		} else {
			if (xmlStrEqual(name, ROOT_ID)) {
				mdata->uri_parts.doctype = VERSIONS_ID_DOCTYPE;
			}
		}
	}
	if (xmlStrEqual(name, FIELD_NAME_ID)) {
		pdata->current_field = mdata->id;
		pdata->current_size = FIELD_LEN_ID;
		pdata->current_name = FIELD_NAME_ID;
	} else if (xmlStrEqual(name, FIELD_NAME_CID)) {
		pdata->current_field = mdata->cid;
		pdata->current_size = FIELD_LEN_ID;
		pdata->current_name = FIELD_NAME_CID;
	} else if (xmlStrEqual(name, FIELD_NAME_NATURE)) {
		pdata->current_field = mdata->nature;
		pdata->current_size = FIELD_LEN_NATURE;
		pdata->current_name = FIELD_NAME_NATURE;
	} else if (xmlStrEqual(name, FIELD_NAME_TYPE)) {
		pdata->current_field = mdata->type;
		pdata->current_size = FIELD_LEN_TYPE;
		pdata->current_name = FIELD_NAME_TYPE;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_DEBUT)) {
		pdata->current_field = mdata->date_debut;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_DEBUT;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_FIN)) {
		pdata->current_field = mdata->date_fin;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_FIN;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM)) {
		pdata->current_field = mdata->num;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM;
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE)) {
		pdata->current_field = mdata->titre;
		pdata->current_size = FIELD_LEN_TITRE;
		pdata->current_name = FIELD_NAME_TITRE;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_PUBLI)) {
		pdata->current_field = mdata->date_publi;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE_TA)) {
		pdata->current_field = mdata->titrefull;
		pdata->current_size = FIELD_LEN_TITREFULL;
		pdata->current_name = FIELD_NAME_TITRE_TA;
	} else if (xmlStrEqual(name, FIELD_NAME_NOR)) {
		pdata->current_field = mdata->nor;
		pdata->current_size = FIELD_LEN_NOR;
		pdata->current_name = FIELD_NAME_NOR;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_TEXTE)) {
		pdata->current_field = mdata->date_texte;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_TEXTE;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM_PARUTION)) {
		pdata->current_field = mdata->num_parution;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM_PARUTION;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM_SEQUENCE)) {
		pdata->current_field = mdata->num_sequence;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM_SEQUENCE;
	} else if (xmlStrEqual(name, FIELD_NAME_ORIGINE_PUBLI)) {
		pdata->current_field = mdata->origine_publi;
		pdata->current_size = FIELD_LEN_ORIGINE_PUBLI;
		pdata->current_name = FIELD_NAME_ORIGINE_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_PAGE_DEB_PUBLI)) {
		pdata->current_field = mdata->page_deb_publi;
		pdata->current_size = FIELD_LEN_PAGE;
		pdata->current_name = FIELD_NAME_PAGE_DEB_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_PAGE_FIN_PUBLI)) {
		pdata->current_field = mdata->page_fin_publi;
		pdata->current_size = FIELD_LEN_PAGE;
		pdata->current_name = FIELD_NAME_PAGE_FIN_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_TITREFULL)) {
		pdata->current_field = mdata->titrefull;
		pdata->current_size = FIELD_LEN_TITREFULL;
		pdata->current_name = FIELD_NAME_TITREFULL;
	} else if (xmlStrEqual(name, FIELD_NAME_AUTORITE)) {
		pdata->current_field = mdata->autorite;
		pdata->current_size = FIELD_LEN_AUTORITE;
		pdata->current_name = FIELD_NAME_AUTORITE;
	} else if (xmlStrEqual(name, FIELD_NAME_MINISTERE)) {
		pdata->current_field = mdata->ministere;
		pdata->current_size = FIELD_LEN_AUTORITE;
		pdata->current_name = FIELD_NAME_MINISTERE;
	} else if (xmlStrEqual(name, FIELD_NAME_ETAT)) {
		pdata->current_field = mdata->etat;
		pdata->current_size = FIELD_LEN_ETAT;
		pdata->current_name = FIELD_NAME_ETAT;
	} else if (xmlStrEqual(name, FIELD_NAME_ORIGINE)) {
		pdata->current_field = mdata->origine;
		pdata->current_size = FIELD_LEN_ORIGINE;
		pdata->current_name = FIELD_NAME_ORIGINE;
	} else if (xmlStrEqual(name, FIELD_NAME_DERNIERE_MODIFICATION)) {
		pdata->current_field = mdata->derniere_modification;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DERNIERE_MODIFICATION;
	} else if (xmlStrEqual(name, FIELD_NAME_VERSION_A_VENIR)) {
		if (mdata->nb_versions_a_venir < MAX_VERSIONS) {
			pdata->current_field = mdata->versions_a_venir[mdata->nb_versions_a_venir++];
			pdata->current_size = FIELD_LEN_DATE;
			pdata->current_name = FIELD_NAME_VERSION_A_VENIR;
		} else {
			fprintf(stderr, "warning:%s: too much VERSION_A_VENIR (MAX_VERSIONS=%d)\n",
				mdata->id, MAX_VERSIONS);
			reset_current(pdata);
		}
	} else if (xmlStrEqual(name, FIELD_NAME_TEXTE) && (
			mdata->uri_parts.doctype == JORFSCTA_DOCTYPE
			|| mdata->uri_parts.doctype == JORFARTI_DOCTYPE
			|| mdata->uri_parts.doctype == LEGISCTA_DOCTYPE
			|| mdata->uri_parts.doctype == LEGIARTI_DOCTYPE)) {
		/*
		 * <CONTEXTE>
		 *  <TEXTE autorite="" cid="LEGITEXT000006071367"
		 *    date_publi="2999-01-01" date_signature="2999-01-01"
		 *    ministere="" nature="CODE" nor="" num="" num_parution_jo="">
		 */
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = FIELD_NAME_TEXTE;
		while (NULL != attrs && NULL != attrs[0]) {
			r = 0;
			if (strcmp((const char *)attrs[0], "cid") == 0)
				r = copy_attr_to_field(mdata->id,
					attrs[0], attrs[1], xmlStrlen(attrs[1]),
					"contexte.cid",
					mdata->contexte.cid, FIELD_LEN_ID);
			else if (strcmp((const char *)attrs[0], "nature") == 0)
				r = copy_attr_to_field(mdata->id,
					attrs[0], attrs[1], xmlStrlen(attrs[1]),
					"contexte.nature",
					mdata->contexte.nature, FIELD_LEN_NATURE);
			else if (strcmp((const char *)attrs[0], "nor") == 0)
				r = copy_attr_to_field(mdata->id,
					attrs[0], attrs[1], xmlStrlen(attrs[1]),
					"contexte.nor",
					mdata->contexte.nor, FIELD_LEN_NOR);
			else if (strcmp((const char *)attrs[0], "num") == 0)
				r = copy_attr_to_field(mdata->id,
					attrs[0], attrs[1], xmlStrlen(attrs[1]),
					"contexte.num",
					mdata->contexte.num, FIELD_LEN_NUM);
			else if (strcmp((const char *)attrs[0], "date_signature") == 0)
				r = copy_attr_to_field(mdata->id,
					attrs[0], attrs[1], xmlStrlen(attrs[1]),
					"contexte.date_signature",
					mdata->contexte.date_signature, FIELD_LEN_DATE);
			else if (strcmp((const char *)attrs[0], "date_publi") == 0)
				r = copy_attr_to_field(mdata->id,
					attrs[0], attrs[1], xmlStrlen(attrs[1]),
					"contexte.date_publi",
					mdata->contexte.date_publi, FIELD_LEN_DATE);
			/* Ignored: autorite, ministere, num_parution_jo */
			// TODO: que faire quand r < 0 ?
			attrs = &attrs[2];
		}
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE_TXT) && (
			mdata->uri_parts.doctype == JORFSCTA_DOCTYPE
			|| mdata->uri_parts.doctype == JORFARTI_DOCTYPE
			|| mdata->uri_parts.doctype == LEGISCTA_DOCTYPE
			|| mdata->uri_parts.doctype == LEGIARTI_DOCTYPE)) {
		/*
		 * <CONTEXTE>
		 *   ...
		 *      <TITRE_TXT c_titre_court="Code rural et de la pêche maritime"
		 *        debut="2010-05-08" fin="2999-01-01"
		 *        id_txt="LEGITEXT000022197698">Code rural et de la pêche maritime</TITRE_TXT>
		 *      <TITRE_TXT c_titre_court="Code rural" debut="1979-12-01" fin="2010-05-08"
		 *        id_txt="LEGITEXT000006071367">Code rural (nouveau)</TITRE_TXT>
		 */
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = FIELD_NAME_TITRE_TXT;
		/*
		if (mdata->contexte.nb_versions < MAX_VERSIONS) {
			docversion = &pdata->contexte.versions[pdata->contexte.nb_versions++];
			docversion->kind = DOCKIND_TEXT;
			while (NULL != attrs && NULL != attrs[0]) {
				r = 0;
				if (strcmp((const char *) attrs[0], "debut") == 0)
					r = copy_attr_to_field(
						pdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"contexte.versions[].date_debut",
						docversion->date_debut,
						FIELD_LEN_DATE);
				else if (strcmp((const char *) attrs[0], "fin") == 0)
					r = copy_attr_to_field(
						pdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"contexte.versions[].date_fin",
						docversion->date_fin,
						FIELD_LEN_DATE);
				else if (strcmp((const char *) attrs[0], "id_txt") == 0)
					r = copy_attr_to_field(
						pdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"contexte.versions[].id",
						docversion->id,
						FIELD_LEN_ID);
				attrs = &attrs[2];
			}
		} else {
			fprintf(stderr, "warning:%s: too much %s (%d)\n",
				pdata->id, FIELD_NAME_TITRE_TXT, MAX_VERSIONS);
		}
		 */
	} else if (xmlStrEqual(name, FIELD_NAME_VERSION)) {
		/*
		 * <CONTEXTE>
		 *   ...
		 *      <TITRE_TXT c_titre_court="Code rural et de la pêche maritime"
		 *        debut="2010-05-08" fin="2999-01-01"
		 *        id_txt="LEGITEXT000022197698">Code rural et de la pêche maritime</TITRE_TXT>
		 *      <TITRE_TXT c_titre_court="Code rural" debut="1979-12-01" fin="2010-05-08"
		 *        id_txt="LEGITEXT000006071367">Code rural (nouveau)</TITRE_TXT>
		 */
		if (versions->nb_versions < versions->max_versions) {
			pdata->current_field = NULL;
			pdata->current_size = 0;
			pdata->current_name = FIELD_NAME_VERSION;
			pdata->parent_element = PE_VERSION;
			docversion = &versions->versions[versions->nb_versions];
			memset(docversion, 0, sizeof(struct document_version));
			versions->nb_versions++;
			while (NULL != attrs && NULL != attrs[0]) {
				r = 0;
				if (strcmp((const char *) attrs[0], "etat") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"versions.versions[].etat",
						docversion->etat,
						FIELD_LEN_ETAT);
				// TODO: que faire quand r < 0 ?
				attrs = &attrs[2];
			}
		} else {
			fprintf(stderr, "warning:%s: too much %s (%d)\n",
				mdata->id, FIELD_NAME_VERSION, versions->max_versions);
			reset_current(pdata);
		}
	} else if (xmlStrEqual(name, FIELD_NAME_LIEN_ART) || xmlStrEqual(name, FIELD_NAME_LIEN_TXT)) {
		if (pdata->parent_element == PE_VERSION) {
			docversion = &versions->versions[versions->nb_versions-1];
			if (xmlStrEqual(name, FIELD_NAME_LIEN_ART))
				docversion->kind = DOCKIND_ARTICLE;
			else
				docversion->kind = DOCKIND_TEXT;
			while (NULL != attrs && NULL != attrs[0]) {
				r = 0;
				if (strcmp((const char *) attrs[0], "debut") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"versions.versions[].date_debut",
						docversion->date_debut,
						FIELD_LEN_DATE);
				else if (strcmp((const char *) attrs[0], "fin") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"versions.versions[].date_fin",
						docversion->date_fin,
						FIELD_LEN_DATE);
				else if (strcmp((const char *) attrs[0], "id") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"versions.versions[].id",
						docversion->id,
						FIELD_LEN_ID);
				else if (strcmp((const char *) attrs[0], "num") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"versions.versions[].num",
						docversion->num,
						FIELD_LEN_NUM);
				// TODO: que faire quand r < 0 ?
				attrs = &attrs[2];
			}
		}
	} else if (xmlStrEqual(name, FIELD_NAME_LIEN)) {
		if (liens->nb_liens < MAX_LIENS) {
			lien = &liens->liens[liens->nb_liens];
			memset(lien, 0, sizeof(struct lien));
			liens->nb_liens++;
			pdata->current_field = lien->titre;
			pdata->current_size = FIELD_LEN_TITRE;
			pdata->current_name = FIELD_NAME_LIEN;
			while (NULL != attrs && NULL != attrs[0]) {
				r = 0;
				if (strcmp((const char *) attrs[0], "cidtexte") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.cid_texte",
						lien->cid_texte,
						FIELD_LEN_ID);
				else if (strcmp((const char *) attrs[0], "id") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.id",
						lien->id,
						FIELD_LEN_ID);
				else if (strcmp((const char *) attrs[0], "datesignatexte") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.date_signature_texte",
						lien->date_signature_texte,
						FIELD_LEN_DATE);
				else if (strcmp((const char *) attrs[0], "naturetexte") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.nature_texte",
						lien->nature_texte,
						FIELD_LEN_NATURE);
				else if (strcmp((const char *) attrs[0], "nortexte") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.nor_texte",
						lien->nor_texte,
						FIELD_LEN_NOR);
				else if (strcmp((const char *) attrs[0], "numtexte") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.num_texte",
						lien->num_texte,
						FIELD_LEN_NUM);
				else if (strcmp((const char *) attrs[0], "num") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.num",
						lien->num,
						FIELD_LEN_NUM);
				else if (strcmp((const char *) attrs[0], "sens") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.sens",
						lien->sens,
						FIELD_LEN_SENS);
				else if (strcmp((const char *) attrs[0], "typelien") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"lien.typelien",
						lien->typelien,
						FIELD_LEN_TYPELIEN);
				// TODO: que faire quand r < 0 ?
				attrs = &attrs[2];
			}
		} else {
			fprintf(stderr, "warning:%s: too much %s (%d)\n",
				mdata->id, FIELD_NAME_LIEN, liens->max_liens);
			reset_current(pdata);
		}
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
		|| xmlStrEqual(name, FIELD_NAME_ORIGINE)
		|| xmlStrEqual(name, FIELD_NAME_TEXTE)
		|| xmlStrEqual(name, FIELD_NAME_TITRE_TXT)
		|| xmlStrEqual(name, FIELD_NAME_VERSION_A_VENIR)
		|| xmlStrEqual(name, FIELD_NAME_VERSION)
		|| xmlStrEqual(name, FIELD_NAME_LIENS)
		|| xmlStrEqual(name, FIELD_NAME_LIEN)
		) {
		reset_current(pdata);
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
				pdata->metadata->id, pdata->current_name, len - pdata->current_size, len, chars);
			reset_current(pdata);
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
	struct metadata *mdata;
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

	pdata = infos->pdata;
	reset_parsed_data(pdata);
	mdata = pdata->metadata;
	mdata->uri_parts.fund = infos->fund;
	set_base(fname, size, mdata->uri_parts.base);
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
			uri_len = set_jorflegi_uri(mdata);
			if (uri_len == -1) {
				fprintf_doctype(stdout, mdata->uri_parts.doctype);
				fprintf(stdout, "%s ERROR\n", mdata->id + 8);
				// return -1;
			} else if (uri_len == 0) {
				fprintf_doctype(stdout, mdata->uri_parts.doctype);
				fprintf(stdout, "%s IGNORED\n", mdata->id + 8);
			} else {
				if (mdata->uri_parts.kind != EMPTY_URI_KIND) {
					uri_cpy(&mdata->uri_parts, mdata->uri);
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

	infos.pdata = allocate_parsed_data();
	if (infos.pdata == NULL) {
		exit(EXIT_FAILURE);
	}

	parser_handler.startElement = start_element_callback;
	parser_handler.endElement = end_element_callback;
	parser_handler.characters = characters_callback;

	infos.ctxt = xmlCreatePushParserCtxt(&parser_handler, infos.pdata, NULL, 0, NULL);
	infos.fund = fund;

	r = iterate_archive(fname, archive_show, &infos);

	xmlFreeParserCtxt(infos.ctxt);
	free_parsed_data(infos.pdata);

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
