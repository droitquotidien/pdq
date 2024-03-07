#ifndef PDQ_TIMESTAMP_H
#define PDQ_TIMESTAMP_H
#include <regex.h>
#include <time.h>
int init_timestamp_re(regex_t *tag_re);
int parse_timestamp(char *s, regex_t *ts_re, struct tm *tag);
void free_timestamp_re(regex_t *tag_re);
#endif //PDQ_TIMESTAMP_H
