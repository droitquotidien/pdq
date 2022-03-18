#include <stdio.h>

void write_html_start(FILE *f)
{
	fprintf(f, "<!DOCTYPE html>\n<html lang=\"fr\">\n");
}

void write_head_start(FILE *f)
{
	fprintf(f, "  <head>\n    <meta charset=\"UTF-8\"/>\n");
}

void write_title(FILE *f, const char *title)
{
	fprintf(f, "    <title>%s</title>\n", title);
}

void write_meta_with_content(FILE *f, const char *name, const char *content)
{
	fprintf(f, "    <meta name=\"%s\" content=\"%s\">\n", name, content);
}

void write_head_stop(FILE *f)
{
	fprintf(f, "  </head>\n");
}

void write_body_start(FILE *f)
{
	fprintf(f, "  <body>\n");
}

void write_body_stop(FILE *f)
{
	fprintf(f, "  </body>\n");
}

void write_html_stop(FILE *f)
{
	fprintf(f, "</html>\n");
}
