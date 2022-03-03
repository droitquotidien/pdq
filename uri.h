#ifndef PDQ_URI_H
#define PDQ_URI_H
#include <string.h>

#define MAX_URI_COUNTRY 3
#define MAX_URI_CORPUS 5
#define MAX_URI_NATURE 70
#define MAX_URI_YEAR 5
#define MAX_URI_MONTH 3
#define MAX_URI_DAY 3
#define MAX_URI_NUMBER 52
#define MAX_URI_VERSION 9
#define MAX_URI 256

enum uri_kind {
    EMPTY_URI_KIND = 0,
    URI_COUNTRY, /* fr */
    URI_CORPUS, /* fr/lr, fr/jp */
    URI_CORPUS_YEAR, /* fr/jo/YYYY */
    URI_CORPUS_MONTH, /* fr/jo/YYYY/MM? */
    URI_CORPUS_DATE, /* fr/jo/YYYY/MM?/DD? */
    URI_CORPUS_DATE_CNUM, /* fr/jo/YYYY/MM?/DD?/CNUM */
    URI_CORPUS_NATURE, /* fr/lr/NATURE */
    URI_CORPUS_NATURE_YEAR, /* fr/lr/NATURE/YYYY */
    URI_CORPUS_NATURE_MONTH, /* fr/lr/NATURE/YYYY/MM? */
    URI_CORPUS_NATURE_DATE, /* fr/lr/NATURE/YYYY/MM?/DD? */
    URI_CORPUS_NATURE_DATE_TNUM, /* fr/lr/NATURE/YYYY/MM?/DD?/TNUM */
    URI_CORPUS_NATURE_DATE_TNUM_ANUM, /* fr/lr/NATURE/YYYY/MM?/DD?/TNUM/ANUM */
    URI_CORPUS_NATURE_DATE_TNUM_SNUM, /* fr/lr/NATURE/YYYY/MM?/DD?/TNUM/SNUM */
    URI_CORPUS_NATURE_DATE_TNUM_VERSION, /* fr/lr/NATURE/YYYY/MM?/DD?/TNUM/YYYYMMDD */
    URI_CORPUS_NATURE_DATE_TNUM_SNUM_VERSION, /* fr/lr/NATURE/YYYY/MM?/DD?/TNUM/SNUM/YYYYMMDD */
    URI_CORPUS_NATURE_DATE_TNUM_ANUM_VERSION, /* fr/lr/NATURE/YYYY/MM?/DD?/TNUM/ANUM/YYYYMMDD */
    URI_CORPUS_TNUM, /* fr/code/civil, fr/constitution/1956 */
    URI_CORPUS_TNUM_VERSION, /* fr/code/civil/YYYYMMDD, fr/constitution/1956/YYYYMMDD */
    URI_CORPUS_TNUM_SNUM_VERSION, /* fr/code/civil/sid123/YYYYMMDD, fr/constitution/1956/sid123/YYYYMMDD */
    URI_CORPUS_TNUM_ANUM_VERSION, /* fr/code/civil/aid123/YYYYMMDD, fr/constitution/1956/aid123/YYYYMMDD */
    PARTIAL_URI_ANUM_VERSION, /* ANUM/YYYYMMDD */
    PARTIAL_URI_SNUM_VERSION, /* SNUM/YYYYMMDD */
    PARTIAL_URI_VERSION, /* YYYYMMDD */
    PARTIAL_URI_SNUM, /* SNUM */
};

/*
 * LEGI
 * ----
 *
 * fr/constitution          URI_CORPUS (conteneur)
 * fr/constitution/1958     URI_CORPUS_YEAR (texte)
 * fr/code                  URI_CORPUS (liste)
 * fr/code/civ              URI_CORPUS_NATURE (texte)
 * fr/lr/...
 *
 */

#define URI_SEP '/'

enum doctype {
    EMPTY_DOCTYPE = 0,
    COUNTRY_DOCTYPE,
    JORFCONT_DOCTYPE,
    JORFTEXT_DOCTYPE,
    JORFVERS_DOCTYPE,
    JORFSCTA_DOCTYPE,
    JORFARTI_DOCTYPE,
    LEGITEXT_DOCTYPE,
    LEGIVERS_DOCTYPE,
    LEGISCTA_DOCTYPE,
    LEGIARTI_DOCTYPE,
    VERSIONS_DOCTYPE,
    VERSIONS_ID_DOCTYPE
};

enum fund {
    EMPTY_FUND = 0,
    JORFLEGI_FUND = 1
};

enum datekind {
    EMPTY_DATEKIND = 0,
    SIG_DATEKIND,
    PUB_DATEKIND,
    BEG_DATEKIND,
    END_DATEKIND
};

enum numkind {
    EMPTY_NUMKIND = 0,
    NATURAL_NUMKIND,
    NOR_NUMKIND,
    ID_NUMKIND
};

struct uri {
    enum fund fund;
    enum doctype doctype;
    enum uri_kind kind;
    char base[5];
    char country[MAX_URI_COUNTRY+1];  /* fr, eu, ... */
    char corpus[MAX_URI_CORPUS+1]; /* jo, lr, cons, ... */
    char nature[MAX_URI_NATURE+1]; /* decret, arrete, ... */
    char year[MAX_URI_YEAR+1]; /* 2022 */
    char month[MAX_URI_MONTH+1]; /* 3, 12 */
    char day[MAX_URI_DAY+1]; /* 1, 2, 12, 30 */
    enum datekind datekind;
    char cnum[MAX_URI_NUMBER+1];
    enum numkind cnumkind;
    char tnum[MAX_URI_NUMBER+1];
    enum numkind tnumkind;
    char snum[MAX_URI_NUMBER+1];
    enum numkind snumkind;
    char anum[MAX_URI_NUMBER+1];
    enum numkind anumkind;
    char aversion[MAX_URI_VERSION+1]; /* 20220203 */
    enum datekind aversion_datekind;
    char sversion[MAX_URI_VERSION+1]; /* 20220203 */
    enum datekind sversion_datekind;
    char tversion[MAX_URI_VERSION+1]; /* 20220203 */
    enum datekind tversion_datekind;
    char cversion[MAX_URI_VERSION+1]; /* 20220203 */
    enum datekind cversion_datekind;
    char num1[MAX_URI_NUMBER+1];
    enum numkind num1kind;
    char num2[MAX_URI_NUMBER+1];
    enum numkind num2kind;
    char num3[MAX_URI_NUMBER+1];
    enum numkind num3kind;
};

char *_uri_cpy(char *dst, const char *src, char sep);
char *uri_cpy(struct uri *source, char *dst);
int fprintf_doctype(FILE *, enum doctype);

#endif //PDQ_URI_H
