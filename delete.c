#include <regex.h>
#include <stdio.h>
#include "buffer.h"
#include "db.h"
/*
 Supression de documents:

gasilber@toupatou:~/Dropbox/data_cons/source_data/DILA$ more 20220419-215052/liste_suppression_jorf.dat
jorf/global/texte/version/JORF/TEXT/00/00/37/15/71/JORFTEXT000037157123
jorf/global/texte/struct/JORF/TEXT/00/00/37/15/71/JORFTEXT000037157123
jorf/global/article/JORF/ARTI/00/00/37/15/71/JORFARTI000037157126
jorf/global/article/JORF/ARTI/00/00/37/15/71/JORFARTI000037157128
jorf/global/article/JORF/ARTI/00/00/37/15/71/JORFARTI000037157130
jorf/global/article/JORF/ARTI/00/00/37/15/71/JORFARTI000037157132

"^.*(version|struct|article|section_ta|conteneur).*((JORF|LEGI)[[:alnum:]]+[0-9]+)$"

gasilber@toupatou:~/Dropbox/data_cons/source_data/DILA$ more 20220419-215052/liste_suppression_legi.dat
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/28/JORFTEXT000045592863/article/LEGI/ARTI/00/00/45/59/74/LEGIARTI000045597403
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/28/JORFTEXT000045592863/article/LEGI/ARTI/00/00/45/59/74/LEGIARTI000045597405
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/28/JORFTEXT000045592863/article/LEGI/ARTI/00/00/45/59/74/LEGIARTI000045597406
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/28/JORFTEXT000045592863/texte/version/LEGITEXT000045597401
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/28/JORFTEXT000045592863/texte/struct/LEGITEXT000045597401
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/34/50/28/JORFTEXT000034502881/article/LEGI/ARTI/00/00/45/60/18/LEGIARTI000045601801
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050037
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050036
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050038
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050039
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050040
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050041
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050042
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050045
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050047
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050048
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/article/LEGI/ARTI/00/00/33/05/00/LEGIARTI000033050043
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/texte/version/LEGITEXT000033050027
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/32/97/51/JORFTEXT000032975145/texte/struct/LEGITEXT000033050027
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/30/JORFTEXT000045593063/section_ta/LEGI/SCTA/00/00/45/59/93/LEGISCTA000045599322
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/30/JORFTEXT000045593063/article/LEGI/ARTI/00/00/45/59/93/LEGIARTI000045599325
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/30/JORFTEXT000045593063/article/LEGI/ARTI/00/00/45/59/93/LEGIARTI000045599323
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/30/JORFTEXT000045593063/article/LEGI/ARTI/00/00/45/59/93/LEGIARTI000045599329
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/30/JORFTEXT000045593063/article/LEGI/ARTI/00/00/45/59/93/LEGIARTI000045599327
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/30/JORFTEXT000045593063/texte/version/LEGITEXT000045599318
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/45/59/30/JORFTEXT000045593063/texte/struct/LEGITEXT000045599318
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/00/17/43/JORFTEXT000000174332/article/LEGI/ARTI/00/00/34/43/18/LEGIARTI000034431800
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/00/17/43/JORFTEXT000000174332/article/LEGI/ARTI/00/00/36/49/04/LEGIARTI000036490449
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/00/17/43/JORFTEXT000000174332/article/LEGI/ARTI/00/00/06/37/34/LEGIARTI000006373490
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/00/17/43/JORFTEXT000000174332/texte/version/LEGITEXT000006078007
legi/global/code_et_TNC_en_vigueur/TNC_en_vigueur/JORF/TEXT/00/00/00/17/43/JORFTEXT000000174332/texte/struct/LEGITEXT000006078007

 "^.*(version|struct|article|section_ta|conteneur).*((JORF|LEGI)[[:alnum:]]+[0-9]+)$"

 */
#define SUPPR_RE "^.*(version|struct|article|section_ta|conteneur).*"\
    "((JORF|LEGI)[[:alnum:]]+[0-9]+)$"

regex_t suppr_re;

int init_delete_re()
{
	int rc;
	char errbuf[512];

	rc = regcomp(&suppr_re, SUPPR_RE, REG_EXTENDED);
	if (rc != 0) {
		regerror(rc, &suppr_re, errbuf,
			 sizeof(errbuf));
		fprintf(stderr, "regexp compilation error: %s\n",
			errbuf);
		return 1;
	}

	return 0;
}

void get_match(regmatch_t *group, char *target, char *buf, int maxsize)
{
	int size;
	size = group->rm_eo - group->rm_so;
	assert (size < maxsize);
	memcpy(target, buf + group->rm_so, size);
	target[size] = 0;
}

enum doctype str_to_doctype_and_rid(const char *kind, char *id)
{
	if (strcmp(kind, "article") == 0) {
		if (strncmp(id, "JORF", 4) == 0) {
			return JORFARTI_DOCTYPE;
		} else {
			return LEGIARTI_DOCTYPE;
		}
	} else if (strcmp(kind, "section_ta") == 0) {
		if (strncmp(id, "JORF", 4) == 0) {
			return JORFSCTA_DOCTYPE;
		} else {
			return LEGISCTA_DOCTYPE;
		}
	} else if (strcmp(kind, "conteneur") == 0) {
		return JORFCONT_DOCTYPE;
	} else if (strcmp(kind, "struct") == 0) {
		if (strncmp(id, "JORF", 4) == 0) {
			return JORFTEXT_DOCTYPE;
		} else {
			return LEGITEXT_DOCTYPE;
		}
	} else if (strcmp(kind, "version") == 0) {
		id[4] = 'V'; id[5] = 'E'; id[6] = 'R'; id[7] = 'S';
		if (strncmp(id, "JORF", 4) == 0) {
			return JORFVERS_DOCTYPE;
		} else {
			return LEGIVERS_DOCTYPE;
		}
	}
	return EMPTY_DOCTYPE;
}

int apply_deletions(struct write_buffer *buf, PGconn *pg_conn, struct tm *tag,
		    FILE *log_file)
{
	int rc;
	regmatch_t matches[3];
	char errbuf[512];
	char kind[12];
	char id[32];
	char *start_line;
	char *stop_line;
	enum doctype doctype;
	char stag[20];
	int deleted;

	sprintf(stag, "%04d-%02d-%02d %02d:%02d:%02d",
		tag->tm_year, tag->tm_mon, tag->tm_mday,
		tag->tm_hour, tag->tm_min, tag->tm_sec);
	/*fprintf(stderr, "DELETIONS FILE:\n");
	fwrite(buf->buffer, 1, buf->current_size, stderr);
	*/
	buf->buffer[buf->current_size] = 0;
	start_line = buf->buffer;
	stop_line = start_line;

	while (*stop_line != 0) {
		while (*stop_line != 0 && *stop_line != '\n') {
			stop_line++;
		}
		if (*stop_line == '\n') {
			*stop_line = '\0';
			rc = regexec(&suppr_re, start_line,
				     sizeof(matches) / sizeof(matches[0]),
				     (regmatch_t *) &matches, 0);
			if (rc != 0) {
				if (rc == REG_NOMATCH) {
					//fprintf(stderr, "NOMATCH\n");
					return 0;
				}
				regerror(rc, &suppr_re, errbuf, sizeof(errbuf));
				fprintf(stderr, "ts regexp match error: %s\n",
					errbuf);
				return -1;
			}
			//fprintf(stderr, "MATCH\n");
			get_match(&matches[1], kind, start_line, 12);
			get_match(&matches[2], id, start_line, 32);
			//fprintf(stderr, "%s %s\n", kind, id);
			doctype = str_to_doctype_and_rid(kind, id);
			if (doctype != EMPTY_DOCTYPE) {
				deleted = delete(pg_conn, doctype,
						 id, stag);
				if (deleted) {
					fprintf(log_file,
						"DELETED:%s:%s\n", id, stag);
				} else {
					fprintf(log_file,
						"UNKNOWN:%s:%s\n", id, stag);
				}
			}
			start_line = stop_line + 1;
			stop_line = start_line;
		}
	}
	return 0;
}

void free_delete_re()
{
	regfree(&suppr_re);
}
