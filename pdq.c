/*
 * PDQ - Droit Quotidien Parser
 *
 * Copyright (C) GA Silber, 2020
 */
#include <string.h>
#include <assert.h>
#include "pdq.h"
#include "uri.h"
#include "parse.h"
#include "jorflegi.h"
#include "fs.h"
#include "buffer.h"

struct gen_uri_info {
    enum fund fund;
    int xml_files;
    struct parsed_data *pdata;
    xmlSAXHandler parser_handler;
    xmlParserCtxtPtr ctxt;
    struct write_buffer *wbuf;
	char bootstrap;
	char force;
	char *target_dir;
	char *data_file;
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
static const xmlChar *FIELD_NAME_STRUCTURE_TXT = BAD_CAST "STRUCTURE_TXT";
static const xmlChar *FIELD_NAME_TM = BAD_CAST "TM";
static const xmlChar *FIELD_NAME_TITRE_TM = BAD_CAST "TITRE_TM";
static const xmlChar *FIELD_NAME_CONTENU = BAD_CAST "CONTENU";
static const xmlChar *FIELD_NAME_NOTICE = BAD_CAST "NOTICE";
static const xmlChar *FIELD_NAME_VISAS = BAD_CAST "VISAS";
static const xmlChar *FIELD_NAME_SIGNATAIRES = BAD_CAST "SIGNATAIRES";
static const xmlChar *FIELD_NAME_NOTA = BAD_CAST "NOTA";
static const xmlChar *FIELD_NAME_TP = BAD_CAST "TP";
static const xmlChar *FIELD_NAME_ABRO = BAD_CAST "ABRO";
static const xmlChar *FIELD_NAME_RECT = BAD_CAST "RECT";
static const xmlChar *FIELD_NAME_SM = BAD_CAST "SM";
static const xmlChar *FIELD_NAME_BLOC_TEXTUEL = BAD_CAST "BLOC_TEXTUEL";
static const xmlChar *FIELD_NAME_MCS_ART = BAD_CAST "MCS_ART";
static const xmlChar *FIELD_NAME_MCS_TXT = BAD_CAST "MCS_TXT";
static const xmlChar *FIELD_NAME_MC = BAD_CAST "MC";
static const xmlChar *FIELD_NAME_ENTREPRISE = BAD_CAST "ENTREPRISE";
static const xmlChar *FIELD_NAME_DATES_EFFET = BAD_CAST "DATES_EFFET";
static const xmlChar *FIELD_NAME_DATE_EFFET = BAD_CAST "DATE_EFFET";
static const xmlChar *FIELD_NAME_DOMAINES = BAD_CAST "DOMAINES";
static const xmlChar *FIELD_NAME_DOMAINE = BAD_CAST "DOMAINE";
static const xmlChar *FIELD_NAME_COMMENTAIRE = BAD_CAST "COMMENTAIRE";

int fprintf_parsed_data(FILE *f, struct parsed_data *pdata)
{
	int r = 0;
	int i;
	struct metadata *mdata = pdata->metadata;
	struct versions *versions = pdata->versions;
	struct liens *liens = pdata->liens;
	struct toc *toc = pdata->toc;
	struct contenu *contenu = pdata->contenu;
	struct document_version *docversion;
	struct lien *lien;
	struct tocitem *tocitem;
	struct mcs *mcs = pdata->mcs;
	struct entreprise *entreprise = pdata->entreprise;
	char *buf;

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

	/*
	if (*contenu->notice.text != 0) {
		fprintf(f, "\tcontenu.notice;\"\"\"%s\"\"\"\n", contenu->notice.text);
	}
	*/
	/*
	if (*contenu->visas.text != 0) {
		fprintf(f, "\tcontenu.visas;\"\"\"%s\"\"\"\n", contenu->visas.text);
	}
	*/
	/*
	if (*contenu->signataires != 0) {
		fprintf(f, "\tcontenu.signataires;\"\"\"%s\"\"\"\n", contenu->signataires);
	}
	 */
	/*
	if (*contenu->nota != 0) {
		fprintf(f, "\tcontenu.nota;\"\"\"%s\"\"\"\n", contenu->nota);
	}*/
	/*
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
	 */
	/*
	for (i = 0; i < toc->nb_tocitems; i++) {
		tocitem = &toc->tocitems[i];
		fprintf(f, "\ttoc.tocitem;%d;%d;%s;%s;%s;%s;%s;%s\n",
			tocitem->niv, tocitem->kind, tocitem->id, tocitem->cid,
			tocitem->date_debut, tocitem->date_fin,
			tocitem->etat, tocitem->titrefull);
	}
	*/
	/*
	for (i = 0; i < mcs->nb_mcs; i++) {
		buf = mcs->mc[i];
		fprintf(f, "\tmc;%s\n", buf);
	}
	 */
	/*
	if (*entreprise->texte_entreprise != 0 && strcmp(entreprise->texte_entreprise, "oui") == 0) {
		fprintf(f, "\tentreprise.texte_entreprise;%s\n", entreprise->texte_entreprise);
		for (i = 0; i < entreprise->nb_dates_effet; i++) {
			buf = entreprise->dates_effet[i];
			fprintf(f, "\tentreprise.date_effet;%s\n", buf);
		}
		for (i = 0; i < entreprise->nb_domaines; i++) {
			buf = entreprise->domaines[i];
			fprintf(f, "\tentreprise.domaine;%s\n", buf);
		}
	}
	 */
	return r;
}

int copy_attr_to_field(char *id, const xmlChar *attr_name, const xmlChar *attr_value, int len,
	char *field_name, char *field, int field_size)
{
	if (len <= field_size) {
		memcpy(field, attr_value, len);
	} else {
		fprintf(stderr, "ERROR:%s: current field %s too small for attr %s (%d) [%.*s]\n",
			id, field_name, attr_name, len - field_size, len, attr_value);
		return -1;
	}
	return len;
}

void reset_current(struct parsed_data *pdata)
{
	pdata->current_dtext = NULL;
	pdata->current_field = NULL;
	pdata->current_size = 0;
	pdata->current_oversize = 0;
	pdata->current_name = NULL;
	pdata->current_parent = NULL;
}

void oversize_current(struct parsed_data *pdata, size_t len)
{
	int missing = 0;

	if (pdata->current_size > 0) {
		missing = len - pdata->current_size;
		pdata->current_size = 0;
	} else {
		missing = len;
	}
	pdata->current_oversize += missing;
}

void reset_parent(struct parsed_data *pdata)
{
	pdata->parent_element = PE_EMPTY;
	pdata->depth = 0;
}

struct contenu *allocate_contenu()
{
	struct contenu *contenu;

	contenu = calloc(1, sizeof(struct contenu));
	if (contenu == NULL) {
		perror("calloc contenu");
		return NULL;
	}
	contenu->notice.size = INITIAL_CONTENT_NOTICE;
	contenu->visas.size = INITIAL_CONTENT_VISAS;
	contenu->signataires.size = INITIAL_CONTENT_SIGNATAIRES;
	contenu->nota.size = INITIAL_CONTENT_NOTA;
	contenu->tp.size = INITIAL_CONTENT_TP;
	contenu->abro.size = INITIAL_CONTENT_ABRO;
	contenu->rect.size = INITIAL_CONTENT_RECT;
	contenu->sm.size = INITIAL_CONTENT_SM;
	contenu->bloc_textuel.size = INITIAL_CONTENT_BLOC_TEXTUEL;

	contenu->notice.text = calloc(1, INITIAL_CONTENT_NOTICE+1);
	if (contenu->notice.text == NULL) {
		perror("calloc contenu->notice.text");
		return NULL;
	}
	contenu->visas.text = calloc(1, INITIAL_CONTENT_VISAS+1);
	if (contenu->visas.text == NULL) {
		perror("calloc contenu->visas.text");
		return NULL;
	}
	contenu->signataires.text = calloc(1, INITIAL_CONTENT_SIGNATAIRES+1);
	if (contenu->signataires.text == NULL) {
		perror("calloc contenu->signataires.text");
		return NULL;
	}
	contenu->nota.text = calloc(1, INITIAL_CONTENT_NOTA+1);
	if (contenu->nota.text == NULL) {
		perror("calloc contenu->nota.text");
		return NULL;
	}
	contenu->tp.text = calloc(1, INITIAL_CONTENT_TP+1);
	if (contenu->tp.text == NULL) {
		perror("calloc contenu->tp.text");
		return NULL;
	}
	contenu->abro.text = calloc(1, INITIAL_CONTENT_ABRO+1);
	if (contenu->abro.text == NULL) {
		perror("calloc contenu->abro.text");
		return NULL;
	}
	contenu->rect.text = calloc(1, INITIAL_CONTENT_RECT+1);
	if (contenu->rect.text == NULL) {
		perror("calloc contenu->rect.text");
		return NULL;
	}
	contenu->sm.text = calloc(1, INITIAL_CONTENT_SM+1);
	if (contenu->sm.text == NULL) {
		perror("calloc contenu->sm.text");
		return NULL;
	}
	contenu->bloc_textuel.text = calloc(1, INITIAL_CONTENT_BLOC_TEXTUEL+1);
	if (contenu->bloc_textuel.text == NULL) {
		perror("calloc contenu->bloc_textuel.text");
		return NULL;
	}

	return contenu;
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
	pdata->contenu = allocate_contenu();
	if (pdata->contenu == NULL) {
		return NULL;
	}
	pdata->mcs = calloc(1, sizeof(struct mcs));
	if (pdata->mcs == NULL) {
		perror("calloc parsed_data->mcs");
		return NULL;
	}
	pdata->mcs->max_mcs = MAX_MCS;
	pdata->entreprise = calloc(1, sizeof(struct entreprise));
	if (pdata->entreprise == NULL) {
		perror("calloc parsed_data->entreprise");
		return NULL;
	}
	pdata->entreprise->max_dates_effet = MAX_DATE_EFFET;
	pdata->entreprise->max_domaines = MAX_DOMAINES;

	return pdata;
}

void free_contenu(struct contenu *contenu)
{
	free(contenu->notice.text);
	free(contenu->visas.text);
	free(contenu->signataires.text);
	free(contenu->nota.text);
	free(contenu->tp.text);
	free(contenu->abro.text);
	free(contenu->rect.text);
	free(contenu->sm.text);
	free(contenu->bloc_textuel.text);
}

void free_parsed_data(struct parsed_data *pdata)
{
	free(pdata->metadata);
	free(pdata->versions);
	free(pdata->liens);
	free(pdata->toc);
	free_contenu(pdata->contenu);
	free(pdata->contenu);
	free(pdata->mcs);
	free(pdata->entreprise);
	free(pdata);
}

void reset_contenu(struct contenu *contenu)
{
	*contenu->notice.text = '\0';
	*contenu->visas.text = '\0';
	*contenu->signataires.text = '\0';
	*contenu->nota.text = '\0';
	*contenu->tp.text = '\0';
	*contenu->abro.text = '\0';
	*contenu->rect.text = '\0';
	*contenu->sm.text = '\0';
	*contenu->bloc_textuel.text = '\0';
}

void reset_parsed_data(struct parsed_data *pdata)
{
	reset_current(pdata);
	memset(pdata->metadata, 0, sizeof(struct metadata));
	pdata->liens->nb_liens = 0;
	pdata->versions->nb_versions = 0;
	pdata->toc->nb_tocitems = 0;
	pdata->mcs->nb_mcs = 0;
	pdata->entreprise->nb_dates_effet = 0;
	pdata->entreprise->nb_domaines = 0;
	reset_contenu(pdata->contenu);
}

void set_dtext(struct parsed_data *pdata, struct dtext *dtext)
{
	pdata->current_dtext = dtext;
	pdata->current_field = dtext->text;
	pdata->current_size = dtext->size;
}

void set_field(struct parsed_data *pdata,
	       const unsigned char *name,
	       char *field, size_t size)
{
	pdata->current_dtext = NULL;
	pdata->current_field = field;
	pdata->current_size = size;
	pdata->current_name = name;
}


void set_parent_content(struct parsed_data *pdata,
	const unsigned char *name,
	enum parent_element pe)
{
	pdata->current_dtext = NULL;
	pdata->current_field = NULL;
	pdata->current_size = 0;
	pdata->current_name = name;
	pdata->current_parent = name;
	pdata->parent_element = pe;
}

char *grow_dtext(struct parsed_data *pdata)
{
	char *newtext;
	struct dtext *dtext;
	size_t offset;
	size_t remaining;
	size_t newsize;

	dtext = pdata->current_dtext;
	assert(dtext != NULL);
	if (dtext != NULL) {
		newsize = dtext->size * 2;
		offset = pdata->current_field - dtext->text;
		remaining = dtext->size + pdata->current_size;

		/*
		fprintf(stderr, "DEBUG: dtext->text=%p pdata->current_field=%p\n", dtext->text, pdata->current_field);
		fprintf(stderr, "DEBUG: dtext->size=%zu pdata->current_size=%zu\n", dtext->size, pdata->current_size);
		fprintf(stderr, "DEBUG: newsize=%zu offset=%zu remaining=%zu\n", newsize, offset, remaining);
		*/

		newtext = realloc(dtext->text, newsize + 1);
		if (newtext == NULL) {
			fprintf(stderr, "CRITICAL:%s: cannot realloc size for %s/%s (%zu)",
				pdata->metadata->id,
				pdata->current_parent, pdata->current_name, newsize + 1);
			return NULL;
		}
		dtext->text = newtext;
		pdata->current_field = newtext + offset;
		pdata->current_size = remaining;
		dtext->size = newsize;
		fprintf(stderr, "INFO:%s: new size for %s/%s: %zu (current_size=%zu)\n",
			pdata->metadata->id,
			pdata->current_parent, pdata->current_name,
			dtext->size, pdata->current_size);
	}

	return newtext;
}

void start_element_callback(void *user_data, const xmlChar *name, const xmlChar **attrs)
{
	struct parsed_data *pdata = user_data;
	struct metadata *mdata;
	struct versions *versions;
	struct liens *liens;
	struct toc *toc;
	struct contenu *contenu;
	struct document_version *docversion;
	struct lien *lien;
	struct tocitem *tocitem;
	struct mcs *mcs;
	struct entreprise *entreprise;
	char *mc;
	char *buf;
	int r;
	int niv;
	int len, len1, len2;

	mdata = pdata->metadata;
	versions = pdata->versions;
	liens = pdata->liens;
	toc = pdata->toc;
	contenu = pdata->contenu;
	mcs = pdata->mcs;
	entreprise = pdata->entreprise;

	if (mdata->uri_parts.doctype == EMPTY_DOCTYPE) {
		if (strcmp(mdata->uri_parts.base, "JORF") == 0) {
			if (xmlStrEqual(name, ROOT_JORFCONT)) {
				mdata->uri_parts.doctype = JORFCONT_DOCTYPE;
				strcpy(mdata->rid, "JORFCONT");
			} else if (xmlStrEqual(name, ROOT_JORFTEXT)) {
				mdata->uri_parts.doctype = JORFTEXT_DOCTYPE;
				strcpy(mdata->rid, "JORFTEXT");
			} else if (xmlStrEqual(name, ROOT_JORFVERS)) {
				mdata->uri_parts.doctype = JORFVERS_DOCTYPE;
				strcpy(mdata->rid, "JORFVERS");
			} else if (xmlStrEqual(name, ROOT_JORFSCTA)) {
				mdata->uri_parts.doctype = JORFSCTA_DOCTYPE;
				strcpy(mdata->rid, "JORFSCTA");
			} else if (xmlStrEqual(name, ROOT_JORFARTI)) {
				mdata->uri_parts.doctype = JORFARTI_DOCTYPE;
				strcpy(mdata->rid, "JORFARTI");
			} else {
				fprintf(stderr, "Unknown root for XML file: %s\n",
					(char *)name);
				exit(1);
			}
		} else if (strcmp(mdata->uri_parts.base, "LEGI") == 0) {
			if (xmlStrEqual(name, ROOT_LEGITEXT)) {
				mdata->uri_parts.doctype = LEGITEXT_DOCTYPE;
				strcpy(mdata->rid, "LEGITEXT");
			} else if (xmlStrEqual(name, ROOT_LEGIVERS)) {
				mdata->uri_parts.doctype = LEGIVERS_DOCTYPE;
				strcpy(mdata->rid, "LEGIVERS");
			} else if (xmlStrEqual(name, ROOT_LEGISCTA)) {
				mdata->uri_parts.doctype = LEGISCTA_DOCTYPE;
				strcpy(mdata->rid, "LEGISCTA");
			} else if (xmlStrEqual(name, ROOT_LEGIARTI)) {
				mdata->uri_parts.doctype = LEGIARTI_DOCTYPE;
				strcpy(mdata->rid, "LEGIARTI");
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
	if (pdata->parent_element == PE_CONTENU) {
		/* mode copie: écrit les éléments XML */
		if (pdata->current_field != NULL) {
			len = xmlStrlen(name) + 1; /* 1=len("<") */
			if (len > pdata->current_size) {
				if (grow_dtext(pdata) == NULL) {
					exit(EXIT_FAILURE);
				}
			}
			*pdata->current_field++ = '<';
			memcpy(pdata->current_field, name, len - 1);
			pdata->current_field += len - 1;
			pdata->current_size -= len;
			*pdata->current_field = '\0';  /* always ends with a 0 */
			while (NULL != attrs && NULL != attrs[0]) {
				len1 = xmlStrlen(attrs[0]);
				len2 = xmlStrlen(attrs[1]);
				len = len1 + len2 + 4; /* 4=len(' =""') */
				if (len > pdata->current_size) {
					if (grow_dtext(pdata) == NULL) {
						exit(EXIT_FAILURE);
					}
				}
				*pdata->current_field++ = ' ';
				memcpy(pdata->current_field, attrs[0], len1);
				pdata->current_field += len1;
				*pdata->current_field++ = '=';
				*pdata->current_field++ = '"';
				memcpy(pdata->current_field, attrs[1], len2);
				pdata->current_field += len2;
				*pdata->current_field++ = '"';
				pdata->current_size -= len;
				*pdata->current_field = '\0';  /* always ends with a 0 */
				attrs = &attrs[2];
			}
			len = 1; /* 1=len(">") */
			if (len > pdata->current_size) {
				if (grow_dtext(pdata) == NULL) {
					exit(EXIT_FAILURE);
				}
			}
			*pdata->current_field++ = '>';
			pdata->current_size -= len;
			*pdata->current_field = '\0';  /* always ends with a 0 */
		}
	} else if (xmlStrEqual(name, FIELD_NAME_ID)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->id;
		pdata->current_size = FIELD_LEN_ID;
		pdata->current_name = FIELD_NAME_ID;
	} else if (xmlStrEqual(name, FIELD_NAME_CID)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->cid;
		pdata->current_size = FIELD_LEN_ID;
		pdata->current_name = FIELD_NAME_CID;
	} else if (xmlStrEqual(name, FIELD_NAME_NATURE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->nature;
		pdata->current_size = FIELD_LEN_NATURE;
		pdata->current_name = FIELD_NAME_NATURE;
	} else if (xmlStrEqual(name, FIELD_NAME_TYPE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->type;
		pdata->current_size = FIELD_LEN_TYPE;
		pdata->current_name = FIELD_NAME_TYPE;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_DEBUT)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->date_debut;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_DEBUT;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_FIN)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->date_fin;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_FIN;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->num;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM;
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->titre;
		pdata->current_size = FIELD_LEN_TITRE;
		pdata->current_name = FIELD_NAME_TITRE;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_PUBLI)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->date_publi;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE_TA)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->titrefull;
		pdata->current_size = FIELD_LEN_TITREFULL;
		pdata->current_name = FIELD_NAME_TITRE_TA;
	} else if (xmlStrEqual(name, FIELD_NAME_NOR)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->nor;
		pdata->current_size = FIELD_LEN_NOR;
		pdata->current_name = FIELD_NAME_NOR;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_TEXTE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->date_texte;
		pdata->current_size = FIELD_LEN_DATE;
		pdata->current_name = FIELD_NAME_DATE_TEXTE;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM_PARUTION)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->num_parution;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM_PARUTION;
	} else if (xmlStrEqual(name, FIELD_NAME_NUM_SEQUENCE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->num_sequence;
		pdata->current_size = FIELD_LEN_NUM;
		pdata->current_name = FIELD_NAME_NUM_SEQUENCE;
	} else if (xmlStrEqual(name, FIELD_NAME_ORIGINE_PUBLI)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->origine_publi;
		pdata->current_size = FIELD_LEN_ORIGINE_PUBLI;
		pdata->current_name = FIELD_NAME_ORIGINE_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_PAGE_DEB_PUBLI)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->page_deb_publi;
		pdata->current_size = FIELD_LEN_PAGE;
		pdata->current_name = FIELD_NAME_PAGE_DEB_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_PAGE_FIN_PUBLI)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->page_fin_publi;
		pdata->current_size = FIELD_LEN_PAGE;
		pdata->current_name = FIELD_NAME_PAGE_FIN_PUBLI;
	} else if (xmlStrEqual(name, FIELD_NAME_TITREFULL)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->titrefull;
		pdata->current_size = FIELD_LEN_TITREFULL;
		pdata->current_name = FIELD_NAME_TITREFULL;
	} else if (xmlStrEqual(name, FIELD_NAME_AUTORITE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->autorite;
		pdata->current_size = FIELD_LEN_AUTORITE;
		pdata->current_name = FIELD_NAME_AUTORITE;
	} else if (xmlStrEqual(name, FIELD_NAME_MINISTERE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->ministere;
		pdata->current_size = FIELD_LEN_AUTORITE;
		pdata->current_name = FIELD_NAME_MINISTERE;
	} else if (xmlStrEqual(name, FIELD_NAME_ETAT)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->etat;
		pdata->current_size = FIELD_LEN_ETAT;
		pdata->current_name = FIELD_NAME_ETAT;
	} else if (xmlStrEqual(name, FIELD_NAME_COMMENTAIRE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->commentaire;
		pdata->current_size = FIELD_LEN_COMMENTAIRE;
		pdata->current_name = FIELD_NAME_COMMENTAIRE;
	} else if (xmlStrEqual(name, FIELD_NAME_ORIGINE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = mdata->origine;
		pdata->current_size = FIELD_LEN_ORIGINE;
		pdata->current_name = FIELD_NAME_ORIGINE;
	} else if (xmlStrEqual(name, FIELD_NAME_DERNIERE_MODIFICATION)) {
		set_field(pdata, name, mdata->derniere_modification, FIELD_LEN_DATE);
	} else if (xmlStrEqual(name, FIELD_NAME_STRUCTURE_TXT)) {
		pdata->current_dtext = NULL;
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = FIELD_NAME_STRUCTURE_TXT;
		pdata->parent_element = PE_STRUCT;
	} else if (xmlStrEqual(name, FIELD_NAME_MCS_ART) || xmlStrEqual(name, FIELD_NAME_MCS_TXT)) {
		pdata->current_dtext = NULL;
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = name;
		pdata->parent_element = PE_MCS;
	} else if (xmlStrEqual(name, FIELD_NAME_MC)) {
		if (pdata->parent_element == PE_MCS) {
			if (mcs->nb_mcs < mcs->max_mcs) {
				mc = mcs->mc[mcs->nb_mcs];
				*mc = 0;
				mcs->nb_mcs++;
				pdata->current_dtext = NULL;
				pdata->current_field = mc;
				pdata->current_size = FIELD_LEN_MC;
				pdata->current_name = name;
			} else {
				fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
					mdata->id, name, mcs->max_mcs);
			}
		}
	} else if (xmlStrEqual(name, FIELD_NAME_DATES_EFFET)) {
		pdata->current_dtext = NULL;
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = name;
		pdata->parent_element = PE_DATES_EFFET;
	} else if (xmlStrEqual(name, FIELD_NAME_DATE_EFFET)) {
		if (pdata->parent_element == PE_DATES_EFFET) {
			if (entreprise->nb_dates_effet < entreprise->max_dates_effet) {
				buf = entreprise->dates_effet[entreprise->nb_dates_effet];
				*buf = 0;
				entreprise->nb_dates_effet++;
				pdata->current_dtext = NULL;
				pdata->current_field = buf;
				pdata->current_size = FIELD_LEN_DATE;
				pdata->current_name = name;
			} else {
				fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
					mdata->id, name, entreprise->max_dates_effet);
			}
		}
	} else if (xmlStrEqual(name, FIELD_NAME_DOMAINES)) {
		pdata->current_dtext = NULL;
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = name;
		pdata->parent_element = PE_DOMAINES;
	} else if (xmlStrEqual(name, FIELD_NAME_DOMAINE)) {
		if (pdata->parent_element == PE_DOMAINES) {
			if (entreprise->nb_domaines < entreprise->max_domaines) {
				buf = entreprise->domaines[entreprise->nb_domaines];
				*buf = 0;
				entreprise->nb_domaines++;
				pdata->current_dtext = NULL;
				pdata->current_field = buf;
				pdata->current_size = FIELD_LEN_DOMAINE;
				pdata->current_name = name;
			} else {
				fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
					mdata->id, name, entreprise->max_domaines);
			}
		}
	} else if (xmlStrEqual(name, FIELD_NAME_NOTICE)) {
		set_parent_content(pdata, name, PE_NOTICE);
	} else if (xmlStrEqual(name, FIELD_NAME_VISAS)) {
		set_parent_content(pdata, name, PE_VISAS);
	} else if (xmlStrEqual(name, FIELD_NAME_SIGNATAIRES)) {
		set_parent_content(pdata, name, PE_SIGNATAIRES);
	} else if (xmlStrEqual(name, FIELD_NAME_NOTA)) {
		set_parent_content(pdata, name, PE_NOTA);
	} else if (xmlStrEqual(name, FIELD_NAME_TP)) {
		set_parent_content(pdata, name, PE_TP);
	} else if (xmlStrEqual(name, FIELD_NAME_ABRO)) {
		set_parent_content(pdata, name, PE_ABRO);
	} else if (xmlStrEqual(name, FIELD_NAME_RECT)) {
		set_parent_content(pdata, name, PE_RECT);
	} else if (xmlStrEqual(name, FIELD_NAME_SM)) {
		set_parent_content(pdata, name, PE_SM);
	} else if (xmlStrEqual(name, FIELD_NAME_BLOC_TEXTUEL)) {
		set_parent_content(pdata, name, PE_BLOC_TEXTUEL);
	} else if (xmlStrEqual(name, FIELD_NAME_CONTENU)) {
		if (pdata->parent_element == PE_VISAS) {
			set_dtext(pdata, &contenu->visas);
			pdata->parent_element = PE_CONTENU;
		} else if (pdata->parent_element == PE_NOTICE) {
			set_dtext(pdata, &contenu->notice);
			pdata->parent_element = PE_CONTENU;
		} else if (pdata->parent_element == PE_NOTA) {
			set_dtext(pdata, &contenu->nota);
			pdata->parent_element = PE_CONTENU;
		} else if (pdata->parent_element == PE_SIGNATAIRES) {
			set_dtext(pdata, &contenu->signataires);
			pdata->parent_element = PE_CONTENU;
		} else if (pdata->parent_element == PE_TP) {
			set_dtext(pdata, &contenu->tp);
			pdata->parent_element = PE_CONTENU;
		} else if (pdata->parent_element == PE_ABRO) {
			set_dtext(pdata, &contenu->abro);
			pdata->parent_element = PE_CONTENU;
		} else if (pdata->parent_element == PE_RECT) {
			set_dtext(pdata, &contenu->rect);
			pdata->parent_element = PE_CONTENU;
		} else if (pdata->parent_element == PE_SM) {
			set_dtext(pdata, &contenu->sm);
			pdata->parent_element = PE_CONTENU;
		} else if (pdata->parent_element == PE_BLOC_TEXTUEL) {
			set_dtext(pdata, &contenu->bloc_textuel);
			pdata->parent_element = PE_CONTENU;
		}
		pdata->current_name = name;
	} else if (xmlStrEqual(name, FIELD_NAME_TM)) {
		/* TM appears also in <CONTEXTE> */
		if (pdata->parent_element == PE_STRUCT) {
			pdata->current_dtext = NULL;
			pdata->current_field = NULL;
			pdata->current_size = 0;
			pdata->current_name = FIELD_NAME_TM;
			pdata->depth++;
			while (NULL != attrs && NULL != attrs[0]) {
				r = 0;
				if (strcmp((const char *) attrs[0], "niv") == 0) {
					if (xmlStrlen(attrs[1]) > 0) {
						niv = atoi((const char *) attrs[1]);
					}
				}
				// TODO: que faire quand r < 0 ?
				attrs = &attrs[2];
			}
			assert(niv == pdata->depth);
		}
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE_TM)) {
		if (pdata->parent_element == PE_STRUCT) {
			if (toc->nb_tocitems < toc->max_tocitems) {
				tocitem = &toc->tocitems[toc->nb_tocitems];
				memset(tocitem, 0, sizeof(struct tocitem));
				toc->nb_tocitems++;
				tocitem->niv = pdata->depth;
				tocitem->kind = DOCKIND_TITLE;
				pdata->current_dtext = NULL;
				pdata->current_field = tocitem->titrefull;
				pdata->current_size = FIELD_LEN_TITREFULL;
				pdata->current_name = name;
			} else {
				fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
					mdata->id, name, toc->max_tocitems);
				/*reset_current(pdata);*/
			}
		}
	} else if (xmlStrEqual(name, FIELD_NAME_VERSION_A_VENIR)) {
		if (mdata->nb_versions_a_venir < MAX_VERSIONS) {
			pdata->current_dtext = NULL;
			pdata->current_field = mdata->versions_a_venir[mdata->nb_versions_a_venir++];
			pdata->current_size = FIELD_LEN_DATE;
			pdata->current_name = FIELD_NAME_VERSION_A_VENIR;
		} else {
			fprintf(stderr, "WARNING:%s: too much VERSION_A_VENIR (MAX_VERSIONS=%d)\n",
				mdata->id, MAX_VERSIONS);
			/*reset_current(pdata);*/
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
		pdata->current_dtext = NULL;
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
		pdata->current_dtext = NULL;
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = FIELD_NAME_TITRE_TXT;
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
			pdata->current_dtext = NULL;
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
			fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
				mdata->id, FIELD_NAME_VERSION, versions->max_versions);
			/*reset_current(pdata);*/
		}
	} else if (xmlStrEqual(name, FIELD_NAME_LIEN_SECTION_TA)) {
		if (pdata->parent_element == PE_STRUCT) {
			/*
			<LIEN_SECTION_TA cid="LEGISCTA000006138434" debut="2003-09-06" etat="VIGUEUR" fin="2999-01-01"
      				id="LEGISCTA000006138434" niv="3"
				url="/LEGI/SCTA/00/00/06/13/84/LEGISCTA000006138434.xml">
			 Titre Ier : Dispositions générales</LIEN_SECTION_TA>
			*/
			if (toc->nb_tocitems < toc->max_tocitems) {
				tocitem = &toc->tocitems[toc->nb_tocitems];
				memset(tocitem, 0, sizeof(struct tocitem));
				toc->nb_tocitems++;
				tocitem->kind = DOCKIND_SECTION;
				pdata->current_field = tocitem->titrefull;
				pdata->current_size = FIELD_LEN_TITREFULL;
				pdata->current_name = name;
				while (NULL != attrs && NULL != attrs[0]) {
					r = 0;
					if (strcmp((const char *) attrs[0], "id") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.id",
							tocitem->id,
							FIELD_LEN_ID);
					else if (strcmp((const char *) attrs[0], "cid") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.cid",
							tocitem->cid,
							FIELD_LEN_ID);
					else if (strcmp((const char *) attrs[0], "num") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.num",
							tocitem->num,
							FIELD_LEN_NUM);
					else if (strcmp((const char *) attrs[0], "etat") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.etat",
							tocitem->etat,
							FIELD_LEN_ETAT);
					else if (strcmp((const char *) attrs[0], "debut") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.date_debut",
							tocitem->date_debut,
							FIELD_LEN_DATE);
					else if (strcmp((const char *) attrs[0], "fin") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.date_fin",
							tocitem->date_fin,
							FIELD_LEN_DATE);
					else if (strcmp((const char *) attrs[0], "niv") == 0) {
						if (xmlStrlen(attrs[1]) > 0) {
							tocitem->niv = atoi((const char *)attrs[1]);
						}
					}
					// TODO: que faire quand r < 0 ?
					attrs = &attrs[2];
				}
			} else {
				fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
					mdata->id, name, toc->max_tocitems);
				/*reset_current(pdata);*/
			}
		}
	} else if (xmlStrEqual(name, FIELD_NAME_LIEN_ART)) {
		if (pdata->parent_element == PE_VERSION) {
			docversion = &versions->versions[versions->nb_versions-1];
			docversion->kind = DOCKIND_ARTICLE;
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
		} else if (pdata->parent_element == PE_STRUCT) {
			/*
			<LIEN_ART debut="2007-01-01" etat="MODIFIE_MORT_NE" fin="2006-01-06"
			 id="LEGIARTI000006584667" num="L641-1-1" origine="LEGI"/>
			*/
			if (toc->nb_tocitems < toc->max_tocitems) {
				tocitem = &toc->tocitems[toc->nb_tocitems];
				memset(tocitem, 0, sizeof(struct tocitem));
				toc->nb_tocitems++;
				pdata->current_dtext = NULL;
				pdata->current_field = NULL;
				pdata->current_size = 0;
				pdata->current_name = name;
				tocitem->kind = DOCKIND_ARTICLE;
				while (NULL != attrs && NULL != attrs[0]) {
					r = 0;
					if (strcmp((const char *) attrs[0], "id") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.id",
							tocitem->id,
							FIELD_LEN_ID);
					else if (strcmp((const char *) attrs[0], "num") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.num",
							tocitem->num,
							FIELD_LEN_NUM);
					else if (strcmp((const char *) attrs[0], "etat") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.etat",
							tocitem->etat,
							FIELD_LEN_ETAT);
					else if (strcmp((const char *) attrs[0], "debut") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.date_debut",
							tocitem->date_debut,
							FIELD_LEN_DATE);
					else if (strcmp((const char *) attrs[0], "fin") == 0)
						r = copy_attr_to_field(
							mdata->id,
							attrs[0], attrs[1],
							xmlStrlen(attrs[1]),
							"tocitem.date_fin",
							tocitem->date_fin,
							FIELD_LEN_DATE);
					// TODO: que faire quand r < 0 ?
					attrs = &attrs[2];
				}
			} else {
				fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
					mdata->id, name, toc->max_tocitems);
				/*reset_current(pdata);*/
			}
		}
	} else if (xmlStrEqual(name, FIELD_NAME_LIEN_TXT)) {
		if (pdata->parent_element == PE_VERSION) {
			docversion = &versions->versions[versions->nb_versions-1];
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
		} else if (pdata->parent_element == PE_STRUCT) {
			/*
			 * <LIEN_TXT idtxt="JORFTEXT000043933898"
			 *   titretxt="Décret n° 2021-1071 ..."/>
			 */
			if (toc->nb_tocitems < toc->max_tocitems) {
				tocitem = &toc->tocitems[toc->nb_tocitems];
				memset(tocitem, 0, sizeof(struct tocitem));
				toc->nb_tocitems++;
				tocitem->niv = pdata->depth;
				tocitem->kind = DOCKIND_TEXT;
				pdata->current_dtext = NULL;
				pdata->current_field = NULL;
				pdata->current_size = 0;
				pdata->current_name = name;
			} else {
				fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
					mdata->id, name, toc->max_tocitems);
				/*reset_current(pdata);*/
			}
			while (NULL != attrs && NULL != attrs[0]) {
				r = 0;
				if (strcmp((const char *) attrs[0], "idtxt") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"toc.tocitem[].id",
						tocitem->id,
						FIELD_LEN_ID);
				else if (strcmp((const char *) attrs[0], "titretxt") == 0)
					r = copy_attr_to_field(
						mdata->id,
						attrs[0], attrs[1],
						xmlStrlen(attrs[1]),
						"toc.tocitem[].titrefull",
						tocitem->titrefull,
						FIELD_LEN_TITREFULL);
				// TODO: que faire quand r < 0 ?
				attrs = &attrs[2];
			}
		}
	} else if (xmlStrEqual(name, FIELD_NAME_LIEN)) {
		if (liens->nb_liens < MAX_LIENS) {
			lien = &liens->liens[liens->nb_liens];
			memset(lien, 0, sizeof(struct lien));
			liens->nb_liens++;
			pdata->current_dtext = NULL;
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
			fprintf(stderr, "WARNING:%s: too much %s (%d)\n",
				mdata->id, FIELD_NAME_LIEN, liens->max_liens);
			/*reset_current(pdata);*/
		}
	} else if (xmlStrEqual(name, FIELD_NAME_STRUCT) || xmlStrEqual(name, FIELD_NAME_STRUCTURE_TA)) {
		pdata->current_dtext = NULL;
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = name;
		pdata->parent_element = PE_STRUCT;
	} else if (xmlStrEqual(name, FIELD_NAME_ENTREPRISE)) {
		pdata->current_dtext = NULL;
		pdata->current_field = NULL;
		pdata->current_size = 0;
		pdata->current_name = name;
		pdata->parent_element = PE_EMPTY;
		while (NULL != attrs && NULL != attrs[0]) {
			r = 0;
			if (strcmp((const char *) attrs[0], "texte_entreprise") == 0)
				r = copy_attr_to_field(
					mdata->id,
					attrs[0], attrs[1],
					xmlStrlen(attrs[1]),
					"entreprise.texte_entreprise",
					entreprise->texte_entreprise,
					FIELD_LEN_ENTREPRISE);
				// TODO: que faire quand r < 0 ?
			attrs = &attrs[2];
		}
	}
}

void end_element_callback(void *user_data, const xmlChar *name)
{
	int len;
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
		|| xmlStrEqual(name, FIELD_NAME_STRUCTURE_TA)
		|| xmlStrEqual(name, FIELD_NAME_STRUCT)
		|| xmlStrEqual(name, FIELD_NAME_STRUCTURE_TXT)
		|| xmlStrEqual(name, FIELD_NAME_NOTICE)
		|| xmlStrEqual(name, FIELD_NAME_VISAS)
		|| xmlStrEqual(name, FIELD_NAME_SIGNATAIRES)
		|| xmlStrEqual(name, FIELD_NAME_NOTA)
		|| xmlStrEqual(name, FIELD_NAME_TP)
		|| xmlStrEqual(name, FIELD_NAME_ABRO)
		|| xmlStrEqual(name, FIELD_NAME_RECT)
		|| xmlStrEqual(name, FIELD_NAME_SM)
		|| xmlStrEqual(name, FIELD_NAME_BLOC_TEXTUEL)
		|| xmlStrEqual(name, FIELD_NAME_CONTENU)
		|| xmlStrEqual(name, FIELD_NAME_MCS_TXT)
		|| xmlStrEqual(name, FIELD_NAME_MCS_ART)
		|| xmlStrEqual(name, FIELD_NAME_ENTREPRISE)
		|| xmlStrEqual(name, FIELD_NAME_DATES_EFFET)
		|| xmlStrEqual(name, FIELD_NAME_DOMAINES)
		) {
		reset_current(pdata);
		reset_parent(pdata);
	} else if (xmlStrEqual(name, FIELD_NAME_TM)) {
		reset_current(pdata);
		pdata->depth--;
	} else if (xmlStrEqual(name, FIELD_NAME_TITRE_TM)
		|| xmlStrEqual(name, FIELD_NAME_LIEN_TXT)
		|| xmlStrEqual(name, FIELD_NAME_LIEN_SECTION_TA)
		|| xmlStrEqual(name, FIELD_NAME_LIEN_ART)
		|| xmlStrEqual(name, FIELD_NAME_MC)
		|| xmlStrEqual(name, FIELD_NAME_DATE_EFFET)
		|| xmlStrEqual(name, FIELD_NAME_DOMAINE)
		) {
		reset_current(pdata);
	} else if (pdata->parent_element == PE_CONTENU) {
		if (pdata->current_field != NULL) {
			len = xmlStrlen(name) + 3; /* 3=len("</>") */
			if (len > pdata->current_size) {
				if (grow_dtext(pdata) == NULL) {
					exit(EXIT_FAILURE);
				}
			}
			*pdata->current_field++ = '<';
			*pdata->current_field++ = '/';
			memcpy(pdata->current_field, name, len - 3);
			pdata->current_field += len - 3;
			*pdata->current_field++ = '>';
			pdata->current_size -= len;
			*pdata->current_field = '\0';  /* always ends with a 0 */
		}
	}
}

void characters_callback(void *user_data, const xmlChar *chars, int len)
{
	struct parsed_data *pdata = user_data;
	int can_copy = 1;

	if (pdata->current_field != NULL) {
		if (len > pdata->current_size) {
			if (pdata->current_dtext != NULL) {
				if (grow_dtext(pdata) == NULL) {
					exit(EXIT_FAILURE);
				}
			} else {
				oversize_current(pdata, len);
				fprintf(stderr, "ERROR:%s: no more size in current field %s (%zu) [%.*s]\n",
					pdata->metadata->id, pdata->current_name, pdata->current_oversize, len, chars);
				pdata->status = 1;
				can_copy = 0;
			}
		}
		if (can_copy) {
			memcpy(pdata->current_field, chars, len);
			pdata->current_field += len;
			pdata->current_size -= len;
			*pdata->current_field = '\0';  /* always ends with a 0 */
		}
	}
}

int archive_parse_file(struct archive *a, struct archive_entry *entry, void *user_data)
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
	struct fs_backend fs;

	fs.rootdir = infos->target_dir;

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

	if (xmlCtxtResetPush(ctxt, NULL, 0, NULL, NULL) != 0) {
		fprintf(stderr, "cannot reset parser ctxt.\n");
		return -1;
	}
	for (;;) {
		r = archive_read_data_block(a, &buff, &len, &offset);
		if (r == ARCHIVE_EOF) {
			uri_len = set_jorflegi_uri(mdata);
			if (uri_len == -1) {
				fprintf_doctype(stderr, mdata->uri_parts.doctype);
				fprintf(stderr, "%s ERROR\n", mdata->id + 8);
				// return -1;
			} else if (uri_len == 0) {
				fprintf_doctype(stderr, mdata->uri_parts.doctype);
				fprintf(stderr, "%s IGNORED\n", mdata->id + 8);
			} else {
				if (mdata->uri_parts.kind != EMPTY_URI_KIND) {
					uri_cpy(&mdata->uri_parts, mdata->uri);
				}
				if (mdata->contexte.uri_parts.kind != EMPTY_URI_KIND) {
					uri_cpy(&mdata->contexte.uri_parts, mdata->contexte.uri);
				}
				fprintf_parsed_data(stderr, pdata);
				write_fs(&fs, pdata, infos->wbuf, 0);
			}
			return 0;
		}
		if (r < ARCHIVE_OK)
			return r;
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

struct params {
	char *data_file;
	char *target_dir;
	char force;
	char bootstrap;
};

void print_usage()
{
	printf("Usage: pdq [-f] [-b] -d data.tar.gz -t target_dir\n");
}

int set_params(int argc, char *argv[], struct params *params)
{
	int c;
	if (argc < 2) {
		return 1;
	}
	memset(params, 0, sizeof(struct params));
	while((c = getopt(argc, argv, "bfd:t:")) != -1) {
		switch(c) {
			case 'f':
				params->force = 1;
				break;
			case 'b':
				params->bootstrap = 1;
				break;
			case 'd':
				params->data_file = optarg;
				break;
			case 't':
				params->target_dir = optarg;
				break;
			case '?':
			default:
				return 1;
		}
	}
	return 0;
}

int generate_uris(struct gen_uri_info *infos)
{
	int r;
	xmlSAXHandler parser_handler = {0};

	infos->pdata = allocate_parsed_data();
	if (infos->pdata == NULL) {
		return -1;
	}
	infos->wbuf = allocate_write_buffer(MAX_SIZE_WRITE_BUFFER, 1);
	if (infos->wbuf == NULL) {
		exit(EXIT_FAILURE);
	}

	r = create_dir(infos->target_dir);
	if (r < 0) {
		exit(EXIT_FAILURE);
	}
	parser_handler.startElement = start_element_callback;
	parser_handler.endElement = end_element_callback;
	parser_handler.characters = characters_callback;
	infos->ctxt = xmlCreatePushParserCtxt(&parser_handler, infos->pdata, NULL, 0, NULL);
	r = iterate_archive(infos->data_file, archive_parse_file, infos);
	xmlFreeParserCtxt(infos->ctxt);
	free_parsed_data(infos->pdata);

	return r;
}

int main(int argc, char **argv)
{
	int r = 0;
	struct params params;
	struct gen_uri_info infos = {0};

	if (set_params(argc, argv, &params) == 1) {
		print_usage();
		return 1;
	}
	if (params.target_dir == NULL || params.data_file == NULL) {
		print_usage();
		return 1;
	}
	infos.target_dir = params.target_dir;
	infos.data_file = params.data_file;
	infos.bootstrap = params.bootstrap;
	infos.force = params.force;
	infos.fund = JORFLEGI_FUND;

	r = generate_uris(&infos);

	return r;
}
