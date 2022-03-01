#include <stdio.h>
#include "uri.h"

char *_uri_cpy(char *dst, const char *src, char sep)
{
	char *p = dst;
	p = stpcpy(p, src);
	if (sep != 0) {
		*p++ = sep;
		*p = '\0';
	}
	return p;
}

char *uri_cpy(struct uri *source, char *dst)
{
	switch (source->kind) {
		case URI_COUNTRY:
			dst = _uri_cpy(dst, source->country, 0);
			break;
		case URI_CORPUS:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, 0);
			break;
		case URI_CORPUS_YEAR:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->year, 0);
			break;
		case URI_CORPUS_MONTH:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, 0);
			break;
		case URI_CORPUS_DATE:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, 0);
			break;
		case URI_CORPUS_DATE_CNUM:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, URI_SEP);
			dst = _uri_cpy(dst, source->cnum, 0);
			break;
		case URI_CORPUS_NATURE:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->nature, 0);
			break;
		case URI_CORPUS_NATURE_DATE:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->nature, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, 0);
			break;
		case URI_CORPUS_NATURE_DATE_TNUM:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->nature, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, 0);
			break;
		case URI_CORPUS_NATURE_DATE_TNUM_ANUM:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->nature, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, URI_SEP);
			dst = _uri_cpy(dst, source->anum, 0);
			break;
		case URI_CORPUS_NATURE_DATE_TNUM_SNUM:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->nature, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, URI_SEP);
			dst = _uri_cpy(dst, source->snum, 0);
			break;
		case URI_CORPUS_NATURE_DATE_TNUM_VERSION:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->nature, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, URI_SEP);
			dst = _uri_cpy(dst, source->tversion, 0);
			break;
		case URI_CORPUS_NATURE_DATE_TNUM_SNUM_VERSION:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->nature, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, URI_SEP);
			dst = _uri_cpy(dst, source->snum, URI_SEP);
			dst = _uri_cpy(dst, source->tversion, 0);
			break;
		case URI_CORPUS_NATURE_DATE_TNUM_ANUM_VERSION:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->nature, URI_SEP);
			dst = _uri_cpy(dst, source->year, URI_SEP);
			dst = _uri_cpy(dst, source->month, URI_SEP);
			dst = _uri_cpy(dst, source->day, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, URI_SEP);
			dst = _uri_cpy(dst, source->anum, URI_SEP);
			dst = _uri_cpy(dst, source->aversion, 0);
			break;
		case URI_CORPUS_TNUM_VERSION:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, URI_SEP);
			dst = _uri_cpy(dst, source->tversion, 0);
			break;
		case URI_CORPUS_TNUM:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, 0);
				break;
		case URI_CORPUS_TNUM_SNUM_VERSION:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, URI_SEP);
			dst = _uri_cpy(dst, source->snum, URI_SEP);
			dst = _uri_cpy(dst, source->tversion, 0);
			break;
		case URI_CORPUS_TNUM_ANUM_VERSION:
			dst = _uri_cpy(dst, source->country, URI_SEP);
			dst = _uri_cpy(dst, source->corpus, URI_SEP);
			dst = _uri_cpy(dst, source->tnum, URI_SEP);
			dst = _uri_cpy(dst, source->anum, URI_SEP);
			dst = _uri_cpy(dst, source->aversion, 0);
			break;
		case PARTIAL_URI_ANUM_VERSION:
			dst = _uri_cpy(dst, source->anum, URI_SEP);
			dst = _uri_cpy(dst, source->aversion, 0);
			break;
		case PARTIAL_URI_SNUM_VERSION:
			dst = _uri_cpy(dst, source->snum, URI_SEP);
			dst = _uri_cpy(dst, source->sversion, 0);
			break;
		default:
			break;
	}

	return dst;
}

int fprintf_doctype(FILE *f, enum doctype doctype)
{
	int r = 0;

	switch(doctype) {
		case JORFCONT_DOCTYPE:
			r = fprintf(f, "JORFCONT");
			break;
		case JORFTEXT_DOCTYPE:
			r = fprintf(f, "JORFTEXT");
			break;
		case JORFVERS_DOCTYPE:
			r = fprintf(f, "JORFVERS");
			break;
		case JORFSCTA_DOCTYPE:
			r = fprintf(f, "JORFSCTA");
			break;
		case JORFARTI_DOCTYPE:
			r = fprintf(f, "JORFARTI");
			break;
		case LEGITEXT_DOCTYPE:
			r = fprintf(f, "LEGITEXT");
			break;
		case LEGIVERS_DOCTYPE:
			r = fprintf(f, "LEGIVERS");
			break;
		case LEGISCTA_DOCTYPE:
			r = fprintf(f, "LEGISCTA");
			break;
		case LEGIARTI_DOCTYPE:
			r = fprintf(f, "LEGIARTI");
			break;
		case EMPTY_DOCTYPE:
		default:
			r = fprintf(f, "EMPTYDOC");
			break;
	}

	return r;
}