#include "json.h"


#define JATTR(f, n, l, a, wbuf) if (*(a) != 0) {\
	CHECK_WRITE(f, "\""#n"\":\"", 4 + (l), wbuf)\
	CHECK_WRITE(fildes, (a), strlen(a), wbuf)\
	CHECK_WRITE(f, "\",\n", 3, wbuf)}

#define JATTRINT(f, n, l, a, wbuf) if (*(a) != 0) {\
	CHECK_WRITE(f, "\""#n"\":", 3 + (l), wbuf)\
	CHECK_WRITE(fildes, (a), strlen(a), wbuf)\
	CHECK_WRITE(f, ",\n", 2, wbuf)}

#define WTYPE(f, s, l, wbuf) {CHECK_WRITE(f, "\"__type\":\"", 10, wbuf) \
	CHECK_WRITE(f, s, l, wbuf)                                             \
	CHECK_WRITE(f, "\"\n", 2, wbuf)}

#define OPENBLOCK(f, n, l, wbuf) CHECK_WRITE(f, "\""#n"\":{\n", 5 + (l), wbuf)
#define OPENLIST(f, n, l, wbuf) CHECK_WRITE(f, "\""#n"\":[\n", 5 + (l), wbuf)
#define CLOSELIST(f, wbuf) CHECK_WRITE(f, "],\n", 3, wbuf)

ssize_t write_json_stripped_chars(
	int fildes,
	const char *data, ssize_t len,
	struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;
	ssize_t i;
	const char *str = data;
	const char *chunk_start = data;
	ssize_t bufferlen = 0;
	int has_non_blank = 0;

	// Voir https://stackoverflow.com/questions/19176024/how-to-escape-special-characters-in-building-a-json-string
	for(i = 0; i < len; i++,str++) {
		if (*str == 0) {
			break;
		} else if (*str == '"') {
				if (bufferlen > 0) {
					CHECK_WRITE(fildes, chunk_start, bufferlen, wbuf)
				}
				CHECK_WRITE(fildes, "\\\"", 2, wbuf)
				chunk_start = str + 1;
				bufferlen = 0;
		} else if (*str == '\t') {
			// LEGIARTI000034488233 (avec formules également)
			if (bufferlen > 0) {
				CHECK_WRITE(fildes, chunk_start, bufferlen, wbuf)
			}
			CHECK_WRITE(fildes, "\\t", 2, wbuf)
			chunk_start = str + 1;
			bufferlen = 0;
		} else if (*str == '\\') {
			// JORFVERS000000409176
			if (bufferlen > 0) {
				CHECK_WRITE(fildes, chunk_start, bufferlen, wbuf)
			}
			CHECK_WRITE(fildes, "\\\\", 2, wbuf)
			chunk_start = str + 1;
			bufferlen = 0;
		} else if (*str == '\n' || *str == '\r') {
			// ignore
			if (bufferlen > 0) {
				CHECK_WRITE(fildes, chunk_start, bufferlen, wbuf)
			}
			chunk_start = str + 1;
			bufferlen = 0;
		} else {
			bufferlen++;
		}
		if (has_non_blank == 0 && (*str != ' ' && *str != '\t' && *str != '\n' && *str != '\r'))
			has_non_blank = 1;
	}

	/* flush tail */
	if (bufferlen > 0 && has_non_blank) {
		CHECK_WRITE(fildes, chunk_start, bufferlen, wbuf)
	}

	return rt;
}

#define CONTENT_WRITE(f, n, l, a, wbuf) if (*(a) != 0) {\
	CHECK_WRITE(f, "\""#n"\":\"", 4 + (l), wbuf)           \
	r = write_json_stripped_chars(f, (a), strlen(a), wbuf); \
	if (r < 0) return -1;                                  \
	rt += r;\
	CHECK_WRITE(f, "\",\n", 3, wbuf)}

#define STRING_WRITE(f, a, wbuf) if (*(a) != 0) {\
	CHECK_WRITE(f, "\"", 1, wbuf)           \
	r = write_json_stripped_chars(f, (a), strlen(a), wbuf); \
	if (r < 0) return -1;                                  \
	rt += r;\
	CHECK_WRITE(f, "\"", 1, wbuf)}

ssize_t write_contenu_json(int fildes, struct contenu *contenu, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;

	OPENBLOCK(fildes, contenu, 7, wbuf);
	if (contenu->notice.size > 0) {
		CONTENT_WRITE(fildes, notice, 6, contenu->notice.text, wbuf);
	}
	if (contenu->visas.size > 0) {
		CONTENT_WRITE(fildes, visas, 5, contenu->visas.text, wbuf);
	}
	if (contenu->bloc_textuel.size > 0) {
		CONTENT_WRITE(fildes, bloc_textuel, 12, contenu->bloc_textuel.text, wbuf);
	}
	if (contenu->signataires.size > 0) {
		CONTENT_WRITE(fildes, signataires, 11, contenu->signataires.text, wbuf);
	}
	if (contenu->nota.size > 0) {
		CONTENT_WRITE(fildes, nota, 4, contenu->nota.text, wbuf);
	}
	if (contenu->tp.size > 0) {
		CONTENT_WRITE(fildes, tp, 2, contenu->tp.text, wbuf);
	}
	if (contenu->abro.size > 0) {
		CONTENT_WRITE(fildes, abro, 4, contenu->abro.text, wbuf);
	}
	if (contenu->rect.size > 0) {
		CONTENT_WRITE(fildes, rect, 4, contenu->rect.text, wbuf);
	}
	if (contenu->sm.size > 0) {
		CONTENT_WRITE(fildes, sm, 2, contenu->sm.text, wbuf);
	}
	WTYPE(fildes, "contenu", 7, wbuf);
	CHECK_WRITE(fildes, "},\n", 3, wbuf)

	return rt;
}

ssize_t write_uri_parts_json(int fildes, struct uri *uri_parts, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;
	char skind[5];

	OPENBLOCK(fildes, uri_parts, 9, wbuf);
	if (uri_parts->num1kind != EMPTY_NUMKIND) {
		CONTENT_WRITE(fildes, naturel, 7, uri_parts->num1, wbuf);
	}
	if (uri_parts->num2kind != EMPTY_NUMKIND) {
		JATTR(fildes, nor, 3, uri_parts->num2, wbuf);
	}
	if (uri_parts->num3kind != EMPTY_NUMKIND) {
		JATTR(fildes, id, 2, uri_parts->num3, wbuf);
	}
	snprintf(skind, 4, "%d", uri_parts->kind);
	JATTRINT(fildes, kind, 4, skind, wbuf);
	WTYPE(fildes, "uri", 3, wbuf);
	CHECK_WRITE(fildes, "},\n", 3, wbuf)

	return rt;
}

ssize_t write_contexte_json(int fildes, struct contexte *contexte, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;

	OPENBLOCK(fildes, contexte, 8, wbuf);
	JATTR(fildes, cid, 3, contexte->cid, wbuf);
	JATTR(fildes, date_publi, 10, contexte->date_publi, wbuf);
	JATTR(fildes, date_signature, 14, contexte->date_signature, wbuf);
	JATTR(fildes, nature, 6, contexte->nature, wbuf);
	JATTR(fildes, nor, 3, contexte->nor, wbuf);
	JATTR(fildes, num, 3, contexte->num, wbuf);
	r = write_uri_parts_json(fildes, &contexte->uri_parts, wbuf);
	if (r < 0) return -1;
	rt += r;
	JATTR(fildes, uri, 3, contexte->uri, wbuf);
	WTYPE(fildes, "contexte", 8, wbuf);
	CHECK_WRITE(fildes, "},\n", 3, wbuf)

	return rt;
}

ssize_t write_tocitem_json(int fildes, struct tocitem *tocitem, struct write_buffer *wbuf, char last)
{
	ssize_t r;
	ssize_t rt = 0;
	char sbuf[5];

	CHECK_WRITE(fildes, "{", 1, wbuf)
	snprintf(sbuf, 4, "%d", tocitem->niv);
	JATTRINT(fildes, niv, 3, sbuf, wbuf);
	snprintf(sbuf, 4, "%d", tocitem->kind);
	JATTRINT(fildes, kind, 4, sbuf, wbuf);
	JATTR(fildes, id, 2, tocitem->id, wbuf);
	JATTR(fildes, cid, 3, tocitem->cid, wbuf);
	JATTR(fildes, date_debut, 10, tocitem->date_debut, wbuf);
	JATTR(fildes, date_fin, 8, tocitem->date_fin, wbuf);
	JATTR(fildes, etat, 4, tocitem->etat, wbuf);
	CONTENT_WRITE(fildes, num, 3, tocitem->num, wbuf);
	CONTENT_WRITE(fildes, titrefull, 9, tocitem->titrefull, wbuf);
	WTYPE(fildes, "tocitem", 7, wbuf);
	if (last) {
		CHECK_WRITE(fildes, "}\n", 2, wbuf);
	} else {
		CHECK_WRITE(fildes, "},\n", 3, wbuf);
	}

	return rt;
}

ssize_t write_toc_json(int fildes, struct toc *toc, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;
	int i;
	struct tocitem *tocitem;
	char last;

	OPENBLOCK(fildes, toc, 3, wbuf);
	OPENLIST(fildes, tocitems, 8, wbuf);
	for (i = 0; i < toc->nb_tocitems; i++) {
		tocitem = &toc->tocitems[i];
		last = (i == (toc->nb_tocitems - 1));
		write_tocitem_json(fildes, tocitem, wbuf, last);
	}
	CLOSELIST(fildes, wbuf);
	WTYPE(fildes, "toc", 3, wbuf);
	CHECK_WRITE(fildes, "},\n", 3, wbuf)

	return rt;
}

ssize_t write_version_json(int fildes, struct document_version *version,
			   struct write_buffer *wbuf, char last)
{
	ssize_t r;
	ssize_t rt = 0;
	char sbuf[5];

	CHECK_WRITE(fildes, "{", 1, wbuf)
	snprintf(sbuf, 4, "%d", version->kind);
	JATTRINT(fildes, kind, 4, sbuf, wbuf);
	JATTR(fildes, id, 2, version->id, wbuf);
	JATTR(fildes, date_debut, 10, version->date_debut, wbuf);
	JATTR(fildes, date_fin, 8, version->date_fin, wbuf);
	JATTR(fildes, etat, 4, version->etat, wbuf);
	CONTENT_WRITE(fildes, num, 3, version->num, wbuf);
	WTYPE(fildes, "version", 7, wbuf);
	if (last) {
		CHECK_WRITE(fildes, "}\n", 2, wbuf);
	} else {
		CHECK_WRITE(fildes, "},\n", 3, wbuf);
	}

	return rt;
}

ssize_t write_versions_json(int fildes, struct versions *versions, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;
	int i;
	struct document_version *version;
	char last;

	OPENBLOCK(fildes, versions, 8, wbuf);
	OPENLIST(fildes, versions, 8, wbuf);
	for (i = 0; i < versions->nb_versions; i++) {
		version = &versions->versions[i];
		last = (i == (versions->nb_versions - 1));
		write_version_json(fildes, version, wbuf, last);
	}
	CLOSELIST(fildes, wbuf);
	WTYPE(fildes, "versions", 8, wbuf);
	CHECK_WRITE(fildes, "},\n", 3, wbuf)

	return rt;
}

ssize_t write_lien_json(int fildes, struct lien *lien,
			struct write_buffer *wbuf, char last)
{
	ssize_t r;
	ssize_t rt = 0;
	char sbuf[5];

	CHECK_WRITE(fildes, "{", 1, wbuf)
	JATTR(fildes, cid_texte, 9, lien->cid_texte, wbuf);
	JATTR(fildes, date_signature_texte, 20, lien->date_signature_texte, wbuf);
	JATTR(fildes, nature_texte, 12, lien->nature_texte, wbuf);
	JATTR(fildes, nor_texte, 9, lien->nor_texte, wbuf);
	JATTR(fildes, num_texte, 9, lien->num_texte, wbuf);	
	JATTR(fildes, id, 2, lien->id, wbuf);
	CONTENT_WRITE(fildes, num, 3, lien->num, wbuf);
	JATTR(fildes, sens, 4, lien->sens, wbuf);
	JATTR(fildes, typelien, 8, lien->typelien, wbuf);
	CONTENT_WRITE(fildes, titre, 5, lien->titre, wbuf);
	WTYPE(fildes, "lien", 4, wbuf);
	if (last) {
		CHECK_WRITE(fildes, "}\n", 2, wbuf);
	} else {
		CHECK_WRITE(fildes, "},\n", 3, wbuf);
	}

	return rt;
}

ssize_t write_liens_json(int fildes, struct liens *liens, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;
	int i;
	struct lien *lien;
	char last;

	OPENBLOCK(fildes, liens, 5, wbuf);
	OPENLIST(fildes, liens, 5, wbuf);
	for (i = 0; i < liens->nb_liens; i++) {
		lien = &liens->liens[i];
		last = (i == (liens->nb_liens - 1));
		write_lien_json(fildes, lien, wbuf, last);
	}
	CLOSELIST(fildes, wbuf);
	WTYPE(fildes, "liens", 5, wbuf);
	CHECK_WRITE(fildes, "},\n", 3, wbuf)

	return rt;
}

ssize_t write_mcs_json(int fildes, struct mcs *mcs, struct write_buffer *wbuf)
{
    ssize_t r;
    ssize_t rt = 0;
    int i;
    struct lien *lien;
    char last;
    char *mc;

    OPENBLOCK(fildes, mcs, 3, wbuf);
    OPENLIST(fildes, mcs, 3, wbuf);
    for (i = 0; i < mcs->nb_mcs; i++) {
        mc = mcs->mc[i];
        STRING_WRITE(fildes, mc, wbuf);
        last = (i == (mcs->nb_mcs - 1));
        if (last) {
            CHECK_WRITE(fildes, "\n", 1, wbuf);
        } else {
            CHECK_WRITE(fildes, ",\n", 2, wbuf);
        }
    }
    CLOSELIST(fildes, wbuf);
    WTYPE(fildes, "mcs", 3, wbuf);
    CHECK_WRITE(fildes, "},\n", 3, wbuf)

    return rt;
}

ssize_t write_json(struct parsed_data *pdata, int fildes, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;
	struct metadata *mdata = pdata->metadata;
	struct mcs *mcs = pdata->mcs;
	struct contenu *contenu = pdata->contenu;
	struct toc *toc = pdata->toc;
	struct versions *versions = pdata->versions;
	struct liens *liens = pdata->liens;
	
	char *cid = (*mdata->contexte.cid == 0?(*mdata->cid == 0?NULL:mdata->cid):mdata->contexte.cid);

	CHECK_WRITE(fildes, "{\n", 2, wbuf)
	JATTR(fildes, id, 2, mdata->id, wbuf);
	JATTR(fildes, nature, 6, mdata->nature, wbuf);
	JATTR(fildes, uri, 3, mdata->uri, wbuf);
	switch (mdata->uri_parts.doctype) {
		case JORFCONT_DOCTYPE:
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			JATTR(fildes, cid, 3, mdata->id, wbuf);
			CONTENT_WRITE(fildes, titre, 5, mdata->titre, wbuf);
			JATTR(fildes, num, 3, mdata->num, wbuf);
			JATTR(fildes, date_publi, 10, mdata->date_publi, wbuf);
			write_toc_json(fildes, toc, wbuf);
			break;
		case JORFTEXT_DOCTYPE:
			// meta_texte_chronicle.dtd
			JATTR(fildes, cid, 3, mdata->cid, wbuf);
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			JATTR(fildes, num, 3, mdata->num, wbuf);
			JATTR(fildes, num_parution, 12, mdata->num_parution, wbuf);
			JATTR(fildes, num_sequence, 12, mdata->num_sequence, wbuf);
			JATTR(fildes, nor, 3, mdata->nor, wbuf);
			JATTR(fildes, date_publi, 10, mdata->date_publi, wbuf);
			JATTR(fildes, date_texte, 10, mdata->date_texte, wbuf);
			JATTR(fildes, derniere_modification, 21, mdata->derniere_modification, wbuf);
			// TODO: versions_a_venir (loop)
			JATTR(fildes, origine_publi, 13, mdata->origine_publi, wbuf);
			JATTR(fildes, page_deb_publi, 14, mdata->page_deb_publi, wbuf);
			JATTR(fildes, page_fin_publi, 14, mdata->page_fin_publi, wbuf);
			write_toc_json(fildes, toc, wbuf);
			write_versions_json(fildes, versions, wbuf);
			break;
		case JORFVERS_DOCTYPE:
			// meta_texte_chronicle.dtd
			JATTR(fildes, cid, 3, mdata->cid, wbuf);
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			JATTR(fildes, num, 3, mdata->num, wbuf);
			JATTR(fildes, num_parution, 12, mdata->num_parution, wbuf);
			JATTR(fildes, num_sequence, 12, mdata->num_sequence, wbuf);
			JATTR(fildes, nor, 3, mdata->nor, wbuf);
			JATTR(fildes, date_publi, 10, mdata->date_publi, wbuf);
			JATTR(fildes, date_texte, 10, mdata->date_texte, wbuf);
			JATTR(fildes, derniere_modification, 21, mdata->derniere_modification, wbuf);
			// TODO: versions_a_venir (loop)
			JATTR(fildes, origine_publi, 13, mdata->origine_publi, wbuf);
			JATTR(fildes, page_deb_publi, 14, mdata->page_deb_publi, wbuf);
			JATTR(fildes, page_fin_publi, 14, mdata->page_fin_publi, wbuf);
			// meta_texte_version
			CONTENT_WRITE(fildes, titre, 5, mdata->titre, wbuf);
			CONTENT_WRITE(fildes, titrefull, 9, mdata->titrefull, wbuf);
			JATTR(fildes, date_debut, 10, mdata->date_debut, wbuf);
			JATTR(fildes, date_fin, 8, mdata->date_fin, wbuf);
			CONTENT_WRITE(fildes, autorite, 8, mdata->autorite, wbuf);
			CONTENT_WRITE(fildes, ministere, 9, mdata->ministere, wbuf);
            write_mcs_json(fildes, mcs, wbuf);
			write_liens_json(fildes, liens, wbuf);
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_contenu_json(fildes, contenu, wbuf);
			break;
		case JORFARTI_DOCTYPE:
			JATTR(fildes, cid, 3, mdata->id, wbuf);
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			// Use content writer because there are
			// \n in numbers (ex: JORFARTI000019283159)
			CONTENT_WRITE(fildes, num, 3, mdata->num, wbuf);
			JATTR(fildes, date_debut, 10, mdata->date_debut, wbuf);
			JATTR(fildes, date_fin, 8, mdata->date_fin, wbuf);
			JATTR(fildes, type, 4, mdata->type, wbuf);
			write_versions_json(fildes, versions, wbuf);
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_contenu_json(fildes, contenu, wbuf);
			write_liens_json(fildes, liens, wbuf);
            write_mcs_json(fildes, mcs, wbuf);
			break;
		case JORFSCTA_DOCTYPE:
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			CONTENT_WRITE(fildes, titrefull, 9, mdata->titrefull, wbuf);
			CONTENT_WRITE(fildes, commentaire, 11, mdata->commentaire, wbuf);
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_toc_json(fildes, toc, wbuf);
			break;
		case LEGITEXT_DOCTYPE:
			// meta_texte_chronicle.dtd
			JATTR(fildes, cid, 3, mdata->cid, wbuf);
			JATTR(fildes, rid, 3, mdata->cid, wbuf);
			JATTR(fildes, num, 3, mdata->num, wbuf);
			JATTR(fildes, num_parution, 12, mdata->num_parution, wbuf);
			JATTR(fildes, num_sequence, 12, mdata->num_sequence, wbuf);
			JATTR(fildes, nor, 3, mdata->nor, wbuf);
			JATTR(fildes, date_publi, 10, mdata->date_publi, wbuf);
			JATTR(fildes, date_texte, 10, mdata->date_texte, wbuf);
			JATTR(fildes, derniere_modification, 21, mdata->derniere_modification, wbuf);
			// TODO: versions_a_venir (loop)
			JATTR(fildes, origine_publi, 13, mdata->origine_publi, wbuf);
			JATTR(fildes, page_deb_publi, 14, mdata->page_deb_publi, wbuf);
			JATTR(fildes, page_fin_publi, 14, mdata->page_fin_publi, wbuf);
			write_toc_json(fildes, toc, wbuf);
			write_versions_json(fildes, versions, wbuf);
			break;
		case LEGIVERS_DOCTYPE:
			// meta_texte_chronicle.dtd
			JATTR(fildes, cid, 3, mdata->cid, wbuf);
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			JATTR(fildes, num, 3, mdata->num, wbuf);
			JATTR(fildes, num_parution, 12, mdata->num_parution, wbuf);
			JATTR(fildes, num_sequence, 12, mdata->num_sequence, wbuf);
			JATTR(fildes, nor, 3, mdata->nor, wbuf);
			JATTR(fildes, date_publi, 10, mdata->date_publi, wbuf);
			JATTR(fildes, date_texte, 10, mdata->date_texte, wbuf);
			JATTR(fildes, derniere_modification, 21, mdata->derniere_modification, wbuf);
			// TODO: versions_a_venir (loop)
			JATTR(fildes, origine_publi, 13, mdata->origine_publi, wbuf);
			JATTR(fildes, page_deb_publi, 14, mdata->page_deb_publi, wbuf);
			JATTR(fildes, page_fin_publi, 14, mdata->page_fin_publi, wbuf);
			// meta_texte_version
			CONTENT_WRITE(fildes, titre, 5, mdata->titre, wbuf);
			CONTENT_WRITE(fildes, titrefull, 9, mdata->titrefull, wbuf);
			JATTR(fildes, etat, 4, mdata->etat, wbuf);
			JATTR(fildes, date_debut, 10, mdata->date_debut, wbuf);
			JATTR(fildes, date_fin, 8, mdata->date_fin, wbuf);
			JATTR(fildes, autorite, 8, mdata->autorite, wbuf);
			JATTR(fildes, ministere, 9, mdata->ministere, wbuf);
			write_liens_json(fildes, liens, wbuf);
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_contenu_json(fildes, contenu, wbuf);
			break;
		case LEGISCTA_DOCTYPE:
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			CONTENT_WRITE(fildes, titrefull, 9, mdata->titrefull, wbuf);
			CONTENT_WRITE(fildes, commentaire, 11, mdata->commentaire, wbuf);
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_toc_json(fildes, toc, wbuf);
			break;
		case LEGIARTI_DOCTYPE:
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			CONTENT_WRITE(fildes, num, 3, mdata->num, wbuf);
			JATTR(fildes, etat, 4, mdata->etat, wbuf);
			JATTR(fildes, date_debut, 10, mdata->date_debut, wbuf);
			JATTR(fildes, date_fin, 8, mdata->date_fin, wbuf);
			JATTR(fildes, type, 4, mdata->type, wbuf);
			write_versions_json(fildes, versions, wbuf);
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_contenu_json(fildes, contenu, wbuf);
			write_liens_json(fildes, liens, wbuf);
			break;
		default:
			break;
	}
	write_uri_parts_json(fildes, &mdata->uri_parts, wbuf);
	WTYPE(fildes, mdata->rid, 8, wbuf);
	CHECK_WRITE(fildes, "}\n", 2, wbuf)

	return rt;
}
