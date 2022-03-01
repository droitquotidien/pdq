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

struct parsed_data {
    struct uri uri_parts;
    char id[FIELD_LEN_ID+1]; /* JORFARTI000037658324 + \0 */
    char cid[FIELD_LEN_ID+1]; /* JORFTEXT000041856940 + \0 */
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
    char origine[FIELD_LEN_ORIGINE+1];

    char uri[MAX_URI];

    int current_size;
    char *current_field;
    const xmlChar *current_name;
    int status;
};

#endif //PDQ_PARSE_H
