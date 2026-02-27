#ifndef PDQ_PARSE_H
#define PDQ_PARSE_H
#include <libxml/xmlreader.h>

#define FIELD_LEN_ID 20
#define FIELD_LEN_NOR 15
#define FIELD_LEN_NATURE 62
#define FIELD_LEN_TYPE 30
#define FIELD_LEN_DATE 10
#define FIELD_LEN_NUM 70
#define FIELD_LEN_TITRE 1024
#define FIELD_LEN_TITREFULL 2048
#define FIELD_LEN_ORIGINE_PUBLI 192
#define FIELD_LEN_PAGE 8
#define FIELD_LEN_AUTORITE 256  /* Note: meme taille pour AUTORITE et MINISTERE */
#define FIELD_LEN_ETAT 64
#define FIELD_LEN_ORIGINE 20
#define FIELD_LEN_SENS 8
#define FIELD_LEN_TYPELIEN 16
#define FIELD_LEN_MC 256
#define FIELD_LEN_DOMAINE 256
#define FIELD_LEN_ENTREPRISE 8
#define FIELD_LEN_COMMENTAIRE 2048

#define MAX_VERSIONS_A_VENIR 24
#define MAX_VERSIONS 80 /* MAX JORF/LEGI 71 */
#define MAX_TOCITEMS 4000 /* MAX JORF 3730 LEGI 600 (LIEN_ART) */
#define MAX_LIENS 28000 /* MAX JORF 23325 / LEGI 8858 */
#define MAX_MCS 1024
#define MAX_DATE_EFFET 256
#define MAX_DOMAINES 256

enum document_kind {
    DOCKIND_EMPTY = 0,
    DOCKIND_TITLE,
    DOCKIND_TEXT,
    DOCKIND_SECTION,
    DOCKIND_ARTICLE,
};

struct document_version {
    enum document_kind kind;
    char id[FIELD_LEN_ID+1]; /* JORFVERS / TEXTVERS */
    char date_debut[FIELD_LEN_DATE+1];
    char date_fin[FIELD_LEN_DATE+1];
    char etat[FIELD_LEN_ETAT+1];
    char num[FIELD_LEN_NUM+1];
};

struct contexte {
    char cid[FIELD_LEN_ID+1];
    char date_publi[FIELD_LEN_DATE+1];
    char date_signature[FIELD_LEN_DATE+1];
    char nature[FIELD_LEN_NATURE+1];
    char nor[FIELD_LEN_NOR+1];
    char num[FIELD_LEN_NUM+1];
    /* Ignored: autorite, ministere, num_parution_jo */
    /* Note: pas utile, en fait.
    struct document_version versions[MAX_VERSIONS+1];
    int nb_versions;
    */
};

struct versions {
    struct document_version versions[MAX_VERSIONS+1]; /* NULL ended */
    int nb_versions;
    int max_versions;
};

struct mcs {
    char mc[MAX_MCS+1][FIELD_LEN_MC]; /* NULL ended */
    int nb_mcs;
    int max_mcs;
};

struct entreprise {
    char texte_entreprise[FIELD_LEN_ENTREPRISE];
    char dates_effet[MAX_DATE_EFFET+1][FIELD_LEN_DATE]; /* NULL ended */
    int nb_dates_effet;
    int max_dates_effet;
    char domaines[MAX_DOMAINES+1][FIELD_LEN_DOMAINE]; /* NULL ended */
    int nb_domaines;
    int max_domaines;
};

struct tocitem {
    int niv;
    enum document_kind kind;
    char id[FIELD_LEN_ID+1];
    char cid[FIELD_LEN_ID+1];
    char date_debut[FIELD_LEN_DATE+1];
    char date_fin[FIELD_LEN_DATE+1];
    char etat[FIELD_LEN_ETAT+1];
    char num[FIELD_LEN_NUM+1];
    char titrefull[FIELD_LEN_TITREFULL+1];
};

/* TEXTELR/STRUCT | STRUCTURE_TA/SECTION_TA */
struct toc {
    struct tocitem tocitems[MAX_TOCITEMS+1]; /* NULL ended */
    int nb_tocitems;
    int max_tocitems;
};

struct lien {
    /* <LIEN cidtexte="JORFTEXT000000754945"
     *   datesignatexte="1998-07-08"
     *   id="JORFTEXT000000754945"
     *   naturetexte="LOI" nortexte="AGRX9500132L"
     *   num="" numtexte="98-565" sens="source"
     *   typelien="CODIFICATION">Loi 98-565 1998-07-08</LIEN>
     */
    char cid_texte[FIELD_LEN_ID+1];
    char date_signature_texte[FIELD_LEN_DATE+1];
    char nature_texte[FIELD_LEN_NATURE+1];
    char nor_texte[FIELD_LEN_NOR+1];
    char num_texte[FIELD_LEN_NUM+1];
    char id[FIELD_LEN_ID+1];
    char num[FIELD_LEN_NUM+1];
    char sens[FIELD_LEN_SENS+1];
    char typelien[FIELD_LEN_TYPELIEN+1];
    char titre[FIELD_LEN_TITRE+1];
};

struct liens {
    struct lien liens[MAX_LIENS+1]; /* NULL ended */
    int nb_liens;
    int max_liens;
    int surplus_liens;
};

#define INITIAL_CONTENT_NOTICE 4096
#define INITIAL_CONTENT_VISAS 4096
#define INITIAL_CONTENT_SIGNATAIRES 4096
#define INITIAL_CONTENT_NOTA 4096
#define INITIAL_CONTENT_TP 256
#define INITIAL_CONTENT_ABRO 256
#define INITIAL_CONTENT_RECT 256
#define INITIAL_CONTENT_SM 256
#define INITIAL_CONTENT_BLOC_TEXTUEL 4096

struct dtext {
    size_t size;
    char *text;
};

struct contenu {
    struct dtext notice;
    struct dtext visas;
    struct dtext signataires;
    struct dtext nota;
    struct dtext tp;
    struct dtext abro;
    struct dtext rect;
    struct dtext sm;
    struct dtext bloc_textuel;
};

enum parent_element {
    PE_EMPTY,
    PE_VERSION,
    PE_STRUCT,
    PE_NOTICE,
    PE_VISAS,
    PE_CONTENU,
    PE_SIGNATAIRES,
    PE_NOTA,
    PE_TP, /* Travaux préparatoires */
    PE_ABRO, /* Abrogations */
    PE_RECT, /* Rectificatifs */
    PE_SM, /* Résumé LEX */
    PE_BLOC_TEXTUEL,
    PE_MCS, /* Mots clés: MCS_ART ou MCS_TXT */
    PE_DATES_EFFET,
    PE_DOMAINES,
};

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

struct metadata {
    char id[FIELD_LEN_ID+1]; /* JORFARTI000037658324 + \0 */
    char cid[FIELD_LEN_ID+1]; /* JORFTEXT000041856940 + \0 */
    char nature[FIELD_LEN_NATURE+1]; /* DECRET + \0 */
    char type[FIELD_LEN_TYPE+1]; /* ENTIEREMENT_MODIF + \0 */
    char date_debut[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_fin[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_publi[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_texte[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char num[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char num_parution[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char num_sequence[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char titre[FIELD_LEN_TITRE+1]; /* JORF n 0113 du 8 mai 2020 + \0 */
    char titrefull[FIELD_LEN_TITREFULL+1]; /* JORF n 0113 du 8 mai 2020 + \0 */
    char nor[FIELD_LEN_NOR+1]; /* ECOI1935482R + \0 */
    char origine_publi[FIELD_LEN_ORIGINE_PUBLI+1]; /* ECOI1935482R + \0 */
    char page_deb_publi[FIELD_LEN_PAGE+1]; /* 12 + \0 */
    char page_fin_publi[FIELD_LEN_PAGE+1]; /* 13 + \0 */
    char autorite[FIELD_LEN_AUTORITE+1]; /* Autorité... + \0 */
    char ministere[FIELD_LEN_AUTORITE+1]; /* Ministère de... + \0 */
    char etat[FIELD_LEN_ETAT+1];
    char derniere_modification[FIELD_LEN_DATE+1];
    char versions_a_venir[MAX_VERSIONS_A_VENIR+1][FIELD_LEN_DATE+1]; /* NULL ended */
    int nb_versions_a_venir;
    char origine[FIELD_LEN_ORIGINE+1];
    char commentaire[FIELD_LEN_COMMENTAIRE+1];
    struct contexte contexte;
    enum fund fund;
    enum doctype doctype;
    char base[5];
    //struct uri uri_parts;
    //char uri[MAX_URI];
    char rid[FIELD_LEN_ID+1]; /* JORFVERS000037658324 + \0 */
};

struct parsed_data {
    struct metadata *metadata;
    struct versions *versions;
    struct liens *liens;
    struct toc *toc;
    struct contenu *contenu;
    struct mcs *mcs;
    struct entreprise *entreprise;
    struct dtext *current_dtext;
    size_t current_size;
    size_t current_oversize;
    char *current_field;
    const xmlChar *current_name;
    enum parent_element parent_element;
    const xmlChar *current_parent;
    int depth;
    int status;
};

#endif //PDQ_PARSE_H
