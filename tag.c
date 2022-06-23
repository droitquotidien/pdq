#include <stdio.h>
#include <regex.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define TAG_RE_DEF "([[:digit:]]{4})([[:digit:]]{2})([[:digit:]]{2})-([[:digit:]]{2})([[:digit:]]{2})([[:digit:]]{2})"

int init_tag_re(regex_t *tag_re)
{
    int rc;
    char errbuf[512];

    rc = regcomp(tag_re, TAG_RE_DEF, REG_EXTENDED);
    if (rc != 0) {
        regerror(rc, tag_re, errbuf, sizeof(errbuf));
        fprintf(stderr, "tag regexp compilation error: %s\n",
                errbuf);
        return -1;
    }

    return 0;
}

void get_group(regmatch_t *group, int *target, char *s)
{
    char buf[5];
    int size;
    size = group->rm_eo - group->rm_so;
    memcpy(buf, s + group->rm_so, size);
    buf[size] = 0;
    *target = atoi(buf);
}

int parse_tag(char *s, regex_t *tag_re, struct tm *tag)
{
    int rc;
    regmatch_t matches[7];
    regmatch_t *group;
    char errbuf[512];

    rc = regexec(tag_re, s, sizeof(matches)/sizeof(matches[0]),
                 (regmatch_t*)&matches, 0);
    if (rc != 0) {
        if (rc == REG_NOMATCH)
            return 0;
        regerror(rc, tag_re, errbuf, sizeof(errbuf));
        fprintf(stderr, "tag regexp match error: %s\n", errbuf);
        return -1;
    }

    get_group(&matches[1], &tag->tm_year, s);
    get_group(&matches[2], &tag->tm_mon, s);
    get_group(&matches[3], &tag->tm_mday, s);
    get_group(&matches[4], &tag->tm_hour, s);
    get_group(&matches[5], &tag->tm_min, s);
    get_group(&matches[6], &tag->tm_sec, s);

    return 1;
}
