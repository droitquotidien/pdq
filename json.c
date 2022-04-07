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

ssize_t write_json(struct parsed_data *pdata, int fildes, struct write_buffer *wbuf)
{
	ssize_t r;
	ssize_t rt = 0;
	struct metadata *mdata = pdata->metadata;
	struct mcs *mcs = pdata->mcs;
	struct contenu *contenu = pdata->contenu;
	struct toc *toc = pdata->toc;

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
			// TODO: versions
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
			// TODO: mcs
			// TODO: liens
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_contenu_json(fildes, contenu, wbuf);
			break;
		case JORFARTI_DOCTYPE:
			JATTR(fildes, cid, 3, mdata->id, wbuf);
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			// \n in numbers (ex: JORFARTI000019283159)
			CONTENT_WRITE(fildes, num, 3, mdata->num, wbuf);
			// TODO: mcs
			JATTR(fildes, date_debut, 10, mdata->date_debut, wbuf);
			JATTR(fildes, date_fin, 8, mdata->date_fin, wbuf);
			JATTR(fildes, type, 4, mdata->type, wbuf);
			// TODO: versions->[{}, ...]
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_contenu_json(fildes, contenu, wbuf);
			// TODO: liens
			break;
		case JORFSCTA_DOCTYPE:
			JATTR(fildes, cid, 3, mdata->id, wbuf);
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
			// TODO: versions
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
			// TODO: liens
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_contenu_json(fildes, contenu, wbuf);
			break;
		case LEGISCTA_DOCTYPE:
			JATTR(fildes, cid, 3, mdata->id, wbuf);
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			CONTENT_WRITE(fildes, titrefull, 9, mdata->titrefull, wbuf);
			CONTENT_WRITE(fildes, commentaire, 11, mdata->commentaire, wbuf);
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_toc_json(fildes, toc, wbuf);
			break;
		case LEGIARTI_DOCTYPE:
			JATTR(fildes, cid, 3, mdata->id, wbuf);
			JATTR(fildes, rid, 3, mdata->rid, wbuf);
			JATTR(fildes, num, 3, mdata->num, wbuf);
			JATTR(fildes, etat, 4, mdata->etat, wbuf);
			JATTR(fildes, date_debut, 10, mdata->date_debut, wbuf);
			JATTR(fildes, date_fin, 8, mdata->date_fin, wbuf);
			JATTR(fildes, type, 4, mdata->type, wbuf);
			// TODO: versions->[{}, ...]
			write_contexte_json(fildes, &mdata->contexte, wbuf);
			write_contenu_json(fildes, contenu, wbuf);
			// TODO: liens
			break;
		default:
			break;
	}
	write_uri_parts_json(fildes, &mdata->uri_parts, wbuf);
	WTYPE(fildes, mdata->rid, 8, wbuf);
	CHECK_WRITE(fildes, "}\n", 2, wbuf)

	return rt;
}