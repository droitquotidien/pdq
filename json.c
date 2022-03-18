#include "parse.h"

#define JATTR(B, F) if (*B != 0)  fprintf(f, "  "#F"=\"%s\",\n", B);

void write_json(FILE *f, struct parsed_data *pdata)
{
	struct metadata *mdata = pdata->metadata;
	char *cid = (*mdata->contexte.cid == 0?(*mdata->cid == 0?NULL:mdata->cid):mdata->contexte.cid);

	fprintf(f, "{\n  __type=\"%.8s\",\n  id=\"%s\",\n  rid=\"%s\",\n",
		mdata->rid, mdata->id, mdata->rid);
	if (cid != NULL) {
		fprintf(f, "  cid=\"%s\",\n", cid);
	}
	switch (mdata->uri_parts.doctype) {
		case JORFCONT_DOCTYPE:
			JATTR(mdata->nature, nature);
			JATTR(mdata->num, num);
			JATTR(mdata->titre, titre);
			JATTR(mdata->titrefull, titrefull);
			/*fprintf(f, "  title=\"%s\",\n", mdata->titre);*/
			break;
		case JORFTEXT_DOCTYPE:
			JATTR(mdata->date_publi, date_publi);
			break;
		case JORFVERS_DOCTYPE:
			JATTR(mdata->date_publi, date_publi);
			JATTR(mdata->titre, titre);
			JATTR(mdata->titrefull, titrefull);
			break;
		default:
			break;
	}
	fprintf(f, "  uri_parts={\n");
	if (mdata->uri_parts.num1kind != EMPTY_NUMKIND) {
		fprintf(f, "    naturel=\"%s\",\n", mdata->uri_parts.num1);
	}
	if (mdata->uri_parts.num2kind != EMPTY_NUMKIND) {
		fprintf(f, "    nor=\"%s\",\n", mdata->uri_parts.num2);
	}
	if (mdata->uri_parts.num3kind != EMPTY_NUMKIND) {
		fprintf(f, "    id=\"%s\",\n", mdata->uri_parts.num3);
	}
	fprintf(f, "    kind=%d\n  },\n", mdata->uri_parts.kind);
	fprintf(f, "  uri=\"%s\"\n}\n", mdata->uri);
}