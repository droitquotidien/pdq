#include <stdio.h>

void write_html_start(FILE *f);
void write_head_start(FILE *f);
void write_title(FILE *f, const char *title);
void write_meta_with_content(FILE *f, const char *name, const char *content);
void write_head_stop(FILE *f);
void write_body_start(FILE *f);
void write_body_stop(FILE *f);
void write_html_stop(FILE *f);