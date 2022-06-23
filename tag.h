#ifndef PDQ_TAG_H
#define PDQ_TAG_H
#include <regex.h>
#include <time.h>
int init_tag_re(regex_t *tag_re);
int parse_tag(char *s, regex_t *tag_re, struct tm *tag);
#endif //PDQ_TAG_H
