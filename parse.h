#ifndef PDQ_PARSE_H
#define PDQ_PARSE_H
#include <libxml/xmlreader.h>
#include "uri.h"

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
#define FIELD_LEN_AUTORITE 256
#define FIELD_LEN_ETAT 64
#define FIELD_LEN_ORIGINE 20
#define FIELD_LEN_SENS 8
#define FIELD_LEN_TYPELIEN 16

#define MAX_VERSIONS_A_VENIR 24
#define MAX_VERSIONS 80 /* MAX JORF/LEGI 71 */
#define MAX_TOCITEMS 4000 /* MAX JORF 3730 LEGI 600 (LIEN_ART) */
#define MAX_LIENS 24000 /* MAX JORF 23325 / LEGI 8858 */

/*
  <SECTION_TA>
    <STRUCTURE_TA>
      <LIEN_SECTION_TA cid="LEGISCTA000006138434" debut="2003-09-06" etat="VIGUEUR" fin="2999-01-01"
      id="LEGISCTA000006138434" niv="3"
      url="/LEGI/SCTA/00/00/06/13/84/LEGISCTA000006138434.xml">Titre Ier : Dispositions générales</LIEN_SECTION_TA>
      <LIEN_SECTION_TA cid="LEGISCTA000006138435" debut="2003-09-06" etat="VIGUEUR" fin="2999-01-01"
      id="LEGISCTA000006138435" niv="3"
      url="/LEGI/SCTA/00/00/06/13/84/LEGISCTA000006138435.xml">Titre II : Les organismes d'intervention</LIEN_SECTION_TA>
      <LIEN_ART debut="2007-01-01" etat="MODIFIE_MORT_NE" fin="2006-01-06" id="LEGIARTI000006584667" num="L641-1-1"
        origine="LEGI"/>

  <TEXTELR>
    <VERSIONS>
      <VERSION etat="VIGUEUR">
        <LIEN_TXT debut="2010-05-08" fin="2999-01-01" id="LEGITEXT000022197698" num=""/>
      </VERSION>
      <VERSION etat="MODIFIE">
        <LIEN_TXT debut="1979-12-01" fin="2010-05-08" id="LEGITEXT000006071367" num=""/>
      </VERSION>
    </VERSIONS>
    <STRUCT>
      <LIEN_SECTION_TA cid="LEGISCTA000006098262" debut="1982-12-01" etat="VIGUEUR" fin="2999-01-01" id="LEGISCTA000006098262" niv="1" url="/LEGI/SCTA/00/00/06/09/82/LEGISCTA000006098262.xml">Partie législative</LIEN_SECTION_TA>
      <LIEN_SECTION_TA cid="LEGISCTA000006098287" debut="1979-12-01" etat="VIGUEUR" fin="2999-01-01" id="LEGISCTA000006098287" niv="1" url="/LEGI/SCTA/00/00/06/09/82/LEGISCTA000006098287.xml">Partie réglementaire</LIEN_SECTION_TA>
      <LIEN_SECTION_TA cid="LEGISCTA000006098333" debut="1996-05-15" etat="VIGUEUR" fin="2999-01-01" id="LEGISCTA000006098333" niv="1" url="/LEGI/SCTA/00/00/06/09/83/LEGISCTA000006098333.xml">Annexes</LIEN_SECTION_TA>
      <LIEN_ART debut="2007-01-01" etat="MODIFIE_MORT_NE" fin="2006-01-06" id="LEGIARTI000006584667" num="L641-1-1" origine="LEGI"/>
    </STRUCT>

  <ARTICLE>
    <VERSIONS>
	<VERSION etat="MODIFIE">
	<LIEN_ART debut="1998-07-09" etat="MODIFIE" fin="1999-07-10" id="LEGIARTI000006584675" num="L641-3" origine="LEGI"/>
	</VERSION>
	<VERSION etat="MODIFIE">
	<LIEN_ART debut="1999-07-10" etat="MODIFIE" fin="2001-07-11" id="LEGIARTI000006584676" num="L641-3" origine="LEGI"/>
	</VERSION>
	<VERSION etat="MODIFIE">
	<LIEN_ART debut="2001-07-11" etat="MODIFIE" fin="2007-01-01" id="LEGIARTI000006584677" num="L641-3" origine="LEGI"/>
	</VERSION>
	<VERSION etat="MODIFIE_MORT_NE">
	<LIEN_ART debut="2007-01-01" etat="MODIFIE_MORT_NE" fin="2006-12-08" id="LEGIARTI000006584678" num="L641-3" origine="LEGI"/>
	</VERSION>
	<VERSION etat="VIGUEUR">
	<LIEN_ART debut="2007-01-01" etat="VIGUEUR" fin="2999-01-01" id="LEGIARTI000006584679" num="L641-3" origine="LEGI"/>
	</VERSION>
    <LIENS>
      <LIEN cidtexte="JORFTEXT000000754945" datesignatexte="1998-07-08" id="JORFTEXT000000754945" naturetexte="LOI" nortexte="AGRX9500132L" num="" numtexte="98-565" sens="source" typelien="CODIFICATION">Loi 98-565 1998-07-08</LIEN>
      <LIEN cidtexte="JORFTEXT000000754945" datesignatexte="1998-07-08" id="LEGIARTI000006600924" naturetexte="LOI" nortexte="AGRX9500132L" num="1" numtexte="98-565" sens="source" typelien="CREATION">Loi n°98-565 du 8 juillet 1998 - art. 1 (V) JORF 9 juillet 1998</LIEN>

  <JO>
    <STRUCTURE_TXT>
      <TM niv="1">
        <TITRE_TM>Journal officiel "Lois et Décrets"</TITRE_TM>
        <TM niv="2">
          <TITRE_TM>Décrets, arrêtés, circulaires</TITRE_TM>
            <TM niv="3">
              <TITRE_TM>Textes généraux</TITRE_TM>
              <TM niv="4">
                <TITRE_TM>Ministère de l'Europe et des affaires étrangères</TITRE_TM>
                <LIEN_TXT idtxt="JORFTEXT000043933898" titretxt="Décret n° 2021-1071 du 12 août 2021 portant modification du décret n° 2013-1154 créant un Conseil national du développement et de la solidarité internationale"/>
                <LIEN_TXT idtxt="JORFTEXT000043933906" titretxt="Décret n° 2021-1072 du 12 août 2021 portant convocation du collège électoral pour l'élection des sénateurs représentant les Français établis hors de France"/>
              </TM>

 */

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

/*
LEGIVERS:
  <TP><CONTENU/></TP>
  <NOTA><CONTENU/></NOTA>
  <ABRO><CONTENU/></ABRO>
  <RECT><CONTENU/></RECT>

JORFVERS:
  <TP><CONTENU/></TP>
  <ABRO><CONTENU/></ABRO>
  <RECT><CONTENU/></RECT>
  <SM><CONTENU/></SM>
  <ENTREPRISE texte_entreprise="non">
    <DATES_EFFET/>
    <DOMAINES/>
  </ENTREPRISE>

JORFARTI / LEGIARTI
  <SM><CONTENU/></SM>
  <NOTA><CONTENU/></NOTA>
  <BLOC_TEXTUEL><CONTENU>
      <br/>   Chaque appellation d'origine contrôlée est définie par décret sur proposition de l'Institut national des appellations d'origine, sans préjudice pour les vins et eaux-de-vie, cidres, poirés, apéritifs à base de cidres, de poirés ou de vins des dispositions de l'article L. 641-15.<br/>
      <br/>   Le décret délimite l'aire géographique de production et détermine les conditions de production et d'agrément du produit.<br/>
      </CONTENU></BLOC_TEXTUEL>

 MCS_ART/MC
 MCS_TXT/MC
 */

#define INITIAL_CONTENT_NOTICE 4096
#define INITIAL_CONTENT_VISAS 4096
#define INITIAL_CONTENT_SIGNATAIRES 4096
#define INITIAL_CONTENT_NOTA 4096

struct dtext {
    size_t size;
    char *text;
};

struct contenu {
    struct dtext notice;
    struct dtext visas;
    struct dtext signataires;
    struct dtext nota;
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
    struct contexte contexte;
    struct uri uri_parts;
    char uri[MAX_URI];
};

struct parsed_data {
    struct metadata *metadata;
    struct versions *versions;
    struct liens *liens;
    struct toc *toc;
    struct contenu *contenu;
    /* struct mc *mc; */
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
