#ifndef PDQ_PARSE_H
#define PDQ_PARSE_H
#include <libxml/xmlreader.h>
#include "uri.h"

#define FIELD_LEN_ID 20
#define FIELD_LEN_NOR 15
#define FIELD_LEN_NATURE 62
#define FIELD_LEN_TYPE 30
#define FIELD_LEN_DATE 10
#define FIELD_LEN_NUM 60
#define FIELD_LEN_TITRE 1024
#define FIELD_LEN_TITREFULL 2048
#define FIELD_LEN_ORIGINE_PUBLI 192
#define FIELD_LEN_PAGE 8
#define FIELD_LEN_AUTORITE 256
#define FIELD_LEN_ETAT 64
#define FIELD_LEN_ORIGINE 20

#define MAX_VERSIONS_A_VENIR 24
#define MAX_VERSIONS 3
#define MAX_SECTIONS 20

/*
  <CONTEXTE>
    <TEXTE autorite="" cid="LEGITEXT000005634379" date_publi="2999-01-01" date_signature="2999-01-01"
        ministere="" nature="CODE" nor="" num="" num_parution_jo="">
      <TITRE_TXT c_titre_court="Code de commerce" debut="2000-09-21" fin="2999-01-01"
        id_txt="LEGITEXT000005634379">Code de commerce</TITRE_TXT>
    </TEXTE>
  </CONTEXTE>

  <CONTEXTE>
    <TEXTE autorite="" cid="LEGITEXT000006071367" date_publi="2999-01-01" date_signature="2999-01-01"
        ministere="" nature="CODE" nor="" num="" num_parution_jo="">
      <TITRE_TXT c_titre_court="Code rural et de la pêche maritime" debut="2010-05-08" fin="2999-01-01"
        id_txt="LEGITEXT000022197698">Code rural et de la pêche maritime</TITRE_TXT>
      <TITRE_TXT c_titre_court="Code rural" debut="1979-12-01" fin="2010-05-08"
        id_txt="LEGITEXT000006071367">Code rural (nouveau)</TITRE_TXT>
    </TEXTE>
  </CONTEXTE>
 */

struct contexte_texte_version {
    char id[FIELD_LEN_ID+1]; /* JORFVERS / TEXTVERS */
    char date_debut[FIELD_LEN_DATE+1];
    char date_fin[FIELD_LEN_DATE+1];
};

struct contexte_texte {
    char cid[FIELD_LEN_ID+1];
    char date_publi[FIELD_LEN_DATE+1];
    char date_signature[FIELD_LEN_DATE+1];
    char nature[FIELD_LEN_NATURE+1];
    char nor[FIELD_LEN_NOR+1];
    char num[FIELD_LEN_NUM+1];
    /* Ignored: autorite, ministere, num_parution_jo */
    struct contexte_texte_version versions[MAX_VERSIONS+1]; /* NULL ended */
    int nb_versions;
};

struct section_ta {
    char id[FIELD_LEN_ID+1];
    char cid[FIELD_LEN_ID+1];
    char date_debut[FIELD_LEN_DATE+1];
    char date_fin[FIELD_LEN_DATE+1];
    char etat[FIELD_LEN_ETAT+1];
    int niv;
};

struct structure_ta {
    struct section_ta sections[MAX_SECTIONS+1]; /* NULL ended */
    int nb_sections;
};

struct parsed_data {
    struct uri uri_parts;
    char id[FIELD_LEN_ID+1]; /* JORFARTI000037658324 + \0 */
    char cid[FIELD_LEN_ID+1]; /* JORFTEXT000041856940 + \0 */
    /* TODO: remove texte_* -> use contexte */
    char texte_cid[FIELD_LEN_ID+1]; /* JORFTEXT000041856940 + \0 */
    char nature[FIELD_LEN_NATURE+1]; /* DECRET + \0 */
    char texte_nature[FIELD_LEN_NATURE+1]; /* DECRET + \0 */
    char type[FIELD_LEN_TYPE+1]; /* ENTIEREMENT_MODIF + \0 */
    char date_debut[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char texte_debut[FIELD_LEN_DATE+1];
    char date_fin[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_publi[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char date_texte[FIELD_LEN_DATE+1]; /* 1999-01-01 + \0 */
    char num[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char texte_num[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char num_parution[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char num_sequence[FIELD_LEN_NUM+1]; /* 3 + \0 */
    char titre[FIELD_LEN_TITRE+1]; /* JORF n 0113 du 8 mai 2020 + \0 */
    char titrefull[FIELD_LEN_TITREFULL+1]; /* JORF n 0113 du 8 mai 2020 + \0 */
    char nor[FIELD_LEN_NOR+1]; /* ECOI1935482R + \0 */
    char texte_nor[FIELD_LEN_NOR+1]; /* ECOI1935482R + \0 */
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
    struct structure_ta structure;
    struct contexte_texte contexte;

    char uri[MAX_URI];

    int current_size;
    char *current_field;
    const xmlChar *current_name;
    int status;
};

#endif //PDQ_PARSE_H
