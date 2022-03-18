#include <ctype.h>
#include "parse.h"
#include "uri.h"

const char *abrogated_codes[] = {
	"LEGITEXT000006069441", "commerce_ancien",
	"LEGITEXT000006070680", "procedure_civile_ancien",
	"LEGITEXT000006070882", "industrie_cinematographique",
	"LEGITEXT000006070884", "justice_militaire_ancien",
	"LEGITEXT000006071014", "enseignement_technique",
	"LEGITEXT000006069562", "marches_publics_1964",
	"LEGITEXT000006071029", "penal_ancien",
	"LEGITEXT000006071189", "nationalite_francaise",
	"LEGITEXT000006071344", "tribunaux_administratifs_cours_administratives_appel",
	"LEGITEXT000006071657", "vin",
	"LEGITEXT000006071071", "deontologie_police_nationale",
	"LEGITEXT000006070159", "deontologie_police_municipale",
	"LEGITEXT000006072635", "deontologie_sages_femmes",
	"LEGITEXT000006072634", "deontologie_medicale",
	"LEGITEXT000006074510", "deontologie_expertise_comptable",
	"LEGITEXT000006073422", "caisses_epargne",
	"LEGITEXT000006074947", "route_ancien",
	"LEGITEXT000006072636", "deontologie_chirurgiens_dentistes",
	"LEGITEXT000006069472", "consommation_boissons_mayotte",
	"LEGITEXT000006072360", "deontologie_veterinaire",  // Plus dans le stock ?
	"LEGITEXT000006072664", "deontologie_medecins",
	"LEGITEXT000006069564", "marches_publics_2001",
	"LEGITEXT000006072666", "marches_publics_2004",
	"LEGITEXT000006071103", "deontologie_commissaire_comptes",
	"LEGITEXT000006071646", "ble",
	"LEGITEXT000006075115", "debits_boisson",
	"LEGITEXT000006071737", "office_national_interprofessionnel_ble",
	"LEGITEXT000006076135", "urbanisme_mayotte",
	"LEGITEXT000006074068", "pensions_militaires_invalidite_victimes_guerre_1951",
	"LEGITEXT000006071514", "forestier_ancien",
	"LEGITEXT000006071556", "forestier_mayotte",
	NULL
};

const char *codes_in_force[] = {
	"LEGITEXT000006074069", "action_sociale_familles", "C. action soc. et fam. ", "Code de l'action sociale et des familles",
 	"LEGITEXT000006075116", "artisanat", "C. artisanat ", "Code de l'artisanat",
 	"LEGITEXT000006073984", "assurances", "C. assur. ", "Code des assurances",
 	"LEGITEXT000006074234", "aviation_civile", "C. aviation ", "Code de l'aviation civile",
 	"LEGITEXT000020908868", "cinema_image_animee", "C. cinéma ", "Code du cinéma et de l'image animée",
 	"LEGITEXT000006070721", "civil", "C. civ. ", "Code civil",
 	"LEGITEXT000005634379", "commerce", "C. com. ", "Code de commerce",
 	"LEGITEXT000006070162", "communes", "C. communes ", "Code des communes",
 	"LEGITEXT000006070300", "communes_nouvelle_caledonie", "C. communes Nouvelle-Calédonie ", "Code des communes de la Nouvelle-Calédonie",
 	"LEGITEXT000006069565", "consommation", "C. consom. ", "Code de la consommation",
 	"LEGITEXT000006074096", "construction_habitation", "CCH ", "Code de la construction et de l'habitation",
 	"LEGITEXT000006071307", "defense", "C. défense ", "Code de la défense",
 	"LEGITEXT000006074232", "deontologie_architectes", "C. déont. arch. ", "Code de déontologie des architectes",
 	"LEGITEXT000006071188", "disciplinaire_penal_marine_marchande", "C. disc. pén. mar. march.", "Code disciplinaire et pénal de la marine marchande",
 	"LEGITEXT000006070208", "domaine_etat", "C. dom. Ét.", "Code du domaine de l'Etat",
 	"LEGITEXT000006074235", "domaine_etat_mayotte", "C. dom. Ét. Mayotte", "Code du domaine de l'Etat et des collectivités publiques applicable à la collectivité territoriale de Mayotte",
 	"LEGITEXT000006074237", "domaine_public_fluvial", "C. dom. publ. fluv.", "Code du domaine public fluvial et de la navigation intérieure",
 	"LEGITEXT000006071570", "douanes", "C. douanes", "Code des douanes",
 	"LEGITEXT000006071645", "douanes_mayotte", "C. douanes Mayotte", "Code des douanes de Mayotte",
 	"LEGITEXT000006071191", "education", "C. éduc.", "Code de l'éducation",
 	"LEGITEXT000006070239", "electoral", "C. élect.", "Code électoral",
 	"LEGITEXT000023983208", "energie", "C. énergie", "Code de l'énergie",
 	"LEGITEXT000006070158", "entree_sejour_etrangers_droit_asile", "C. étrangers", "Code de l'entrée et du séjour des étrangers et du droit d'asile",
 	"LEGITEXT000006074220", "environnement", "C. env.", "Code de l'environnement",
 	"LEGITEXT000006074224", "expropriation_utilite_publique", "C. expr.", "Code de l'expropriation pour cause d'utilité publique",
 	"LEGITEXT000006072637", "famille_aide_sociale", "C. fam.", "Code de la famille et de l'aide sociale",
 	"LEGITEXT000025244092", "forestier", "C. for.", "Code forestier (nouveau)",
 	"LEGITEXT000006070299", "propriete_personnes_publiques", "CGPPP", "Code général de la propriété des personnes publiques",
 	"LEGITEXT000006070633", "collectivites_territoriales", "CGCT", "Code général des collectivités territoriales",
 	"LEGITEXT000006069577", "impots", "CGI", "Code général des impôts",
 	"LEGITEXT000006069568", "impots_annexe_1", "CGI, ann. 1", "Code général des impôts, annexe 1",
 	"LEGITEXT000006069569", "impots_annexe_2", "CGI, ann. 2", "Code général des impôts, annexe 2",
 	"LEGITEXT000006069574", "impots_annexe_3", "CGI, ann. 3", "Code général des impôts, annexe 3",
 	"LEGITEXT000006069576", "impots_annexe_4", "CGI, ann. 4", "Code général des impôts, annexe 4",
 	"LEGITEXT000006070666", "instruments_monetaires_medailles", "C. instr. monét.", "Code des instruments monétaires et des médailles",
 	"LEGITEXT000006070249", "juridictions_financieres", "C. jur. fin.", "Code des juridictions financières",
 	"LEGITEXT000006070933", "justice_administrative", "CJA", "Code de justice administrative",
 	"LEGITEXT000006071360", "justice_militaire", "C. just. mil.", "Code de justice militaire (nouveau)",
 	"LEGITEXT000006071007", "legion_honneur_medaille_militaire", "C. légion d'honneur", "Code de la légion d'honneur et de la médaille militaire",
 	"LEGITEXT000006069583", "impots_procedures_fiscales", "LPF", "Livre des procédures fiscales",
 	"LEGITEXT000005627819", "marches_publics", "CMP", "Code des marchés publics (édition 2006)",
 	"LEGITEXT000006071785", "minier_ancien", "C. minier anc.", "Code minier",
 	"LEGITEXT000023501962", "minier", "C. minier", "Code minier (nouveau)",
 	"LEGITEXT000006072026", "monetaire_financier", "C. monét. fin.", "Code monétaire et financier",
 	"LEGITEXT000006074067", "mutualite", "C. mut.", "Code de la mutualité",
 	"LEGITEXT000006071164", "organisation_judiciaire", "COJ", "Code de l'organisation judiciaire",
 	"LEGITEXT000006074236", "patrimoine", "C. patr.", "Code du patrimoine",
 	"LEGITEXT000006070719", "penal", "C. pén.", "Code pénal",
 	"LEGITEXT000006070302", "pensions_civiles_militaires_retraite", "C. pens. retr.", "Code des pensions civiles et militaires de retraite",
 	"LEGITEXT000006074066", "pensions_retraite_marins_francais", "C. pens. retr. marins", "Code des pensions de retraite des marins français du commerce, de pêche ou de plaisance",
 	"LEGITEXT000031712069", "pensions_militaires_invalidite_victimes_guerre", "C. pens. mil.", "Code des pensions militaires d'invalidité et des victimes de guerre",
 	"LEGITEXT000006074233", "ports_maritimes", "C. ports mar.", "Code des ports maritimes",
 	"LEGITEXT000006070987", "postes_communications_electroniques", "C. P et T", "Code des postes et des communications électroniques",
 	"LEGITEXT000006070716", "procedure_civile", "CPC", "Code de procédure civile",
 	"LEGITEXT000006071154", "procedure_penale", "CPP", "Code de procédure pénale",
 	"LEGITEXT000025024948", "procedures_civiles_execution", "C. proc. civ. exé.", "Code des procédures civiles d'exécution",
 	"LEGITEXT000006069414", "propriete_intellectuelle", "CPI", "Code de la propriété intellectuelle",
 	"LEGITEXT000006071190", "recherche", "C. recherche", "Code de la recherche",
 	"LEGITEXT000031366350", "relations_public_administration", "C. rel. pub. adm.", "Code des relations entre le public et l'administration",
 	"LEGITEXT000006074228", "route", "C. route", "Code de la route",
 	"LEGITEXT000006071366", "rural_ancien", "C. rur. ancien", "Code rural (ancien)",
 	"LEGITEXT000006071367", "rural_nouveau", "C. rur.", "Code rural (nouveau)",
 	"LEGITEXT000022197698", "rural_peche_maritime", "C. rur. pêche", "Code rural et de la pêche maritime",
 	"LEGITEXT000006072665", "sante_publique", "C. santé publ.", "Code de la santé publique",
 	"LEGITEXT000025503132", "securite_interieure", "C. sécu. intér.", "Code de la sécurité intérieure",
 	"LEGITEXT000006073189", "securite_sociale", "CSS", "Code de la sécurité sociale",
 	"LEGITEXT000006071335", "service_national", "C. serv. nat.", "Code du service national",
 	"LEGITEXT000006071318", "sport", "C. sport", "Code du sport",
 	"LEGITEXT000006074073", "tourisme", "C. tourisme", "Code du tourisme",
 	"LEGITEXT000023086525", "transports", "C. transports", "Code des transports",
 	"LEGITEXT000006072050", "travail", "C. trav.", "Code du travail",
 	"LEGITEXT000006072052", "travail_mayotte", "C. trav. Mayotte", "Code du travail applicable à Mayotte",
 	"LEGITEXT000006072051", "travail_maritime", "C. trav. mar.", "Code du travail maritime",
 	"LEGITEXT000006074075", "urbanisme", "C. urb.", "Code de l'urbanisme",
 	"LEGITEXT000006070667", "voirie_routiere", "C. voirie routière", "Code de la voirie routière",
 	"LEGITEXT000037701019", "commande_publique", "CCP", "Code de la commande publique",
 	"LEGITEXT000039086952", "justice_penale_mineurs", "C. just. pen. min.", "Code de la justice pénale des mineurs",
 	NULL
};

const char *constitutions[] = {
	"JORFTEXT000000697056", "1789", // CID
	"LEGITEXT000006071192", "1789", // ID
	"JORFTEXT000000868390", "1946", // CID
	"LEGITEXT000006071193", "1946", // ID
	"LEGITEXT000006071195", "1848", // CID
	"JORFTEXT000000571356", "1958", // CID
	"LEGITEXT000006071194", "1958", // ID
	NULL
};

const char *natures[] = {
	"JORFTEXT000000697056", "DECLARATION",
	"LEGITEXT000018942010", "DECRET",
	"LEGITEXT000018941992", "DECRET",
	"LEGITEXT000019111066", "DECRET",
	"LEGITEXT000019277543", "DECRET",
	"LEGITEXT000019027914", "DECRET",
	"LEGITEXT000020791358", "DECRET",
	NULL
};

void lower_string(char *s)
{
	while (*s != 0) {
		*s = (char)tolower((int)*s);
		s++;
	}
}

static const char *get_info(const char *cnames[], const char *cid, int step, int loc)
{
	int i = 0;

	while (cnames[i] != NULL) {
		if (strcmp(cnames[i], cid) == 0)
			return cnames[i + loc];
		i += step;
	}

	return NULL;
}

const char *get_code_name(const char *cid)
{
	const char *cname;

	cname = get_info(abrogated_codes, cid, 2, 1);
	if (cname != NULL)
		return cname;

	return get_info(codes_in_force, cid, 4, 1);
}

const char *get_constitution(const char *cid)
{
	return get_info(constitutions, cid, 2, 1);
}

const char *remove_zeros(const char *num, int l, int *zc) {
	const char *s = num;

	*zc = 0;
	while (*s != '\0' && *zc < l) {
		if (*s != '0')
			break;
		s++;
		(*zc)++;
	}
	if (*zc == l) {
		return NULL;
	}
	if (*s == '\0') {
		return NULL;
	}

	return s;
}

char *check_date(char *date)
{
	int l = 0;
	char *s = date;
	int fmd = 0;

	while (*s != '\0') {
		if (l == 0) {
			if (*s != '2' && *s != '1')
				return NULL;
		} else if (l >= 1 && l <=3) {
			if (*s < '0' || *s > '9')
				return NULL;
		} else if (l == 5) {
			if (*s != '0' && *s != '1')
				return NULL;
			fmd = *s - 48;
		} else if (l == 6) {
			if (fmd) {
				if (*s < '0' || *s > '2')
					return NULL;
			} else if (*s < '1' || *s > '9')
				return NULL;
		} else if (l == 8) {
			if (*s < '0' || *s > '3')
				return NULL;
			fmd = *s - 48;
		} else if (l == 9) {
			if (fmd == 0) {
				if (*s < '1' || *s > '9')
					return NULL;
			} else if (fmd == 1 || fmd == 2) {
				if (*s < '0' || *s > '9')
					return NULL;
			} else if (*s < '0' || *s > '1') // fmd == 3
				return NULL;
		} else if (*s != '-') {  // l == 4 || l == 7
			return NULL;
		}
		s++;
		l++;
	}

	if (l > 10)
		return NULL;
	return date;
}

int date_to_version(char *date, char *version)
{
	if (*date == 0)
		return -1;

	strncpy(version, date, 4);
	strncpy(version + 4, date + 5, 2);
	strncpy(version + 6, date + 8, 2);

	return 8;
}

int set_date_uri(struct metadata *mdata)
{
	int len;
	int zc;
	char *date_pub;
	char *date_sig;
	char *date;
	const char *val;

	date_pub = check_date(mdata->date_publi);
	if (date_pub == NULL) {
		fprintf(stderr, "ERROR:%s: bad date_publi '%s'\n", mdata->id, mdata->date_publi);
		return -1;
	}
	date_sig = check_date(mdata->date_texte);
	if (date_sig == NULL) {
		fprintf(stderr, "ERROR:%s: bad date_texte '%s'\n", mdata->id, mdata->date_texte);
		return -1;
	}
	if (*date_sig == 0 || (date_sig[0] == '2' && date_sig[1] == '9')) {
		mdata->uri_parts.datekind = PUB_DATEKIND;
		date = date_pub;
		if (*date_pub == 0 || (date_pub[0] == '2' && date_pub[1] == '9')) {
			// *(date_pub + 2432424) = 5;
			fprintf(stderr, "ERROR:%s: no usable date\n", mdata->id);
			return -1;
		}
	} else if (*date_sig != 0) {
		date = date_sig;
		mdata->uri_parts.datekind = SIG_DATEKIND;
		if (date_sig[0] == '2' && date_sig[1] == '9') {
			// *(date_sig + 2432424) = 5;
			fprintf(stderr, "ERROR:%s: no usable date\n", mdata->id);
			return -1;
		}
	}

	strncpy(mdata->uri_parts.year, date, 4);
	len = 5;
	val = remove_zeros(date + 5, 2, &zc);
	strncpy(mdata->uri_parts.month, val, 2 - zc);
	len += (2 - zc) + 1;
	val = remove_zeros(date + 8, 2, &zc);
	strncpy(mdata->uri_parts.day, val, 2 - zc);
	len += (2 - zc);

	return len;
}

int set_num_uri(struct metadata *mdata, char *target, enum numkind *numkind,
	const char *natural, const char *nor, const char *id, const char *idprefix)
{
	int len = 0;
	int plen;
	int preflen;
	int zc;
	const char *val;
	char *tmp;

	if (natural != NULL && (plen = strlen(natural)) > 0) {
		val = remove_zeros(natural, plen, &zc);
		if (val == NULL) {
			fprintf(stderr, "ERROR:%s: bad natural num '%s'\n", id, natural);
			return -1;
		}
		plen -= zc;
		if (plen > MAX_URI_NUMBER) {
			fprintf(stderr, "ERROR:%s: natural num too long '%s' (%d)\n",
				id, natural, plen - MAX_URI_NUMBER);
			return -1;
		}
		strcpy(mdata->uri_parts.num1, val);
		mdata->uri_parts.num1kind = NATURAL_NUMKIND;
		if (*numkind == EMPTY_NUMKIND) {
			strcpy(target, val);
			len += plen;
			*numkind = NATURAL_NUMKIND;
		}
	}
	if (nor != NULL && (plen = strlen(nor)) > 0) {
		val = remove_zeros(nor, plen, &zc);
		if (val == NULL) {
			fprintf(stderr, "ERROR:%s: bad nor num '%s'\n", id, nor);
			return -1;
		}
		if (strlen(val) > FIELD_LEN_NOR) {
			fprintf(stderr, "WARNING:%s: bad nor num '%s' (>%d)\n", id, nor, FIELD_LEN_NOR);
			//return -1;
		}
		plen -= zc;
		if (plen > MAX_URI_NUMBER) {
			fprintf(stderr, "ERROR:%s: nor num too long '%s' (%d)\n",
				id, nor, plen - MAX_URI_NUMBER);
			return -1;
		}
		strcpy(mdata->uri_parts.num2, val);
		mdata->uri_parts.num2kind = NOR_NUMKIND;
		if (*numkind == EMPTY_NUMKIND) {
			strcpy(target, val);
			len += plen;
			*numkind = NOR_NUMKIND;
		}
	}
	if (id != NULL && (plen = strlen(id)) > 0) {
		val = remove_zeros(id + 8, plen, &zc);
		if (val == NULL) {
			fprintf(stderr, "ERROR:%s: bad id num '%s'\n", id, id);
			return -1;
		}
		plen -= (zc + 8);
		if (idprefix != NULL) {
			plen += strlen(idprefix);
		}
		if (plen > MAX_URI_NUMBER) {
			fprintf(stderr, "ERROR:%s: id num too long '%s' (%d)\n",
				id, nor, plen - MAX_URI_NUMBER);
			return -1;
		}
		tmp = mdata->uri_parts.num3;
		if (idprefix != NULL) {
			tmp = stpcpy(tmp, idprefix);
		}
		strcpy(tmp, val);
		mdata->uri_parts.num3kind = ID_NUMKIND;
		if (*numkind == EMPTY_NUMKIND) {
			if (idprefix != NULL) {
				target = stpcpy(target, idprefix);
			}
			strcpy(target, val);
			len += plen;
			*numkind = ID_NUMKIND;
		}
	} else {
		fprintf(stderr, "ERROR:%s: cannot set num for URI\n", id);
		return -1;
	}

	return len;
}

enum uri_kind set_text_uri(struct metadata *mdata, char *tnum_target, enum numkind *tnumkind,
	const char *texte_num, const char *texte_nor, const char *texte_cid, const char *texte_nature,
	size_t *len)
{
    	size_t plen;
    	const char *cname;
	enum uri_kind kind;

	if (strcmp(texte_nature, "CODE") == 0) {
		strcpy(mdata->uri_parts.corpus, "code");
		*len += 5;  /* len(/code) */

		cname = get_code_name(texte_cid);
		if (cname == NULL) {
			fprintf(stderr, "WARNING:%s: cannot find code name for %s\n", mdata->id, texte_cid);
		}

		plen = set_num_uri(mdata, tnum_target,tnumkind,
				   cname, texte_nor, texte_cid,
				   "tid");
		if (plen < 0)
			return plen;
		*len += plen;
		kind = URI_CORPUS_TNUM;
	} else if (strcmp(texte_nature, "CONSTITUTION") == 0) {
		strcpy(mdata->uri_parts.corpus, "cons");
		*len += 5;  /* len(/cons) */
		//(1789|1958|1946) 2004?

		cname = get_constitution(texte_cid);
		if (cname == NULL) {
			fprintf(stderr, "WARNING:%s: cannot find constitution for %s\n", mdata->id, texte_cid);
		}

		plen = set_num_uri(mdata, tnum_target,tnumkind,
				   cname, texte_nor, texte_cid,
				   "tid");
		if (plen < 0)
			return plen;
		*len += plen;
		kind = URI_CORPUS_TNUM;
	} else {
		strcpy(mdata->uri_parts.corpus, "lr");
		*len += 3;  /* len(/lr) */

		plen = strlen(texte_nature);
		if (plen > MAX_URI_NATURE) {
			fprintf(stderr, "ERROR:%s: nature too small for text nature '%s' (%lu)\n",
				mdata->id, cname, plen - MAX_URI_NATURE);
			return EMPTY_URI_KIND;
		}
		strcpy(mdata->uri_parts.nature, texte_nature);
		lower_string(mdata->uri_parts.nature);
		*len += plen + 1;

		plen = set_date_uri(mdata);
		if (plen < 0)
			return plen;
		*len += plen + 1;

		plen = set_num_uri(mdata, tnum_target,
			tnumkind, texte_num, texte_nor, texte_cid,
			"tid");
		if (plen < 0)
			return plen;
		*len += plen + 1;
		kind = URI_CORPUS_NATURE_DATE_TNUM;
	}
	return kind;
}

int set_jorflegi_uri(struct metadata *mdata)
{
	size_t len = 0;
	size_t plen = 0;
	char *val;
	char *date_pub;
	char *date_sig;
	char *date;
	const char *cname;
	int zc;
	enum uri_kind kind;

	if (mdata->uri_parts.doctype == JORFCONT_DOCTYPE) {
		strcpy(mdata->rid+8, mdata->id+8);
		strcpy(mdata->uri_parts.country, "fr");
		strcpy(mdata->uri_parts.corpus, "jo");
		len = 5;
		plen = set_date_uri(mdata);
		if (plen < 0)
			return -1;
		len += plen + 1;
		plen = set_num_uri(mdata, mdata->uri_parts.cnum, &mdata->uri_parts.cnumkind,
			mdata->num, NULL, mdata->id, "cid");
		if (plen < 0)
			return -1;
		len += plen + 1;
		mdata->uri_parts.kind = URI_CORPUS_DATE_CNUM;
	} else if (mdata->uri_parts.doctype == JORFTEXT_DOCTYPE || mdata->uri_parts.doctype == LEGITEXT_DOCTYPE) {
		strcpy(mdata->rid+8, mdata->id+8);
		strcpy(mdata->uri_parts.country, "fr");
		len = 2;  /* fr */
		kind = set_text_uri(mdata, mdata->uri_parts.tnum, &mdata->uri_parts.tnumkind,
				    mdata->num, mdata->nor, mdata->cid,
				    mdata->nature, &len);
		if (kind == EMPTY_URI_KIND)
			return -1;
		mdata->uri_parts.kind = kind;
	} else if (mdata->uri_parts.doctype == JORFVERS_DOCTYPE) {
		strcpy(mdata->rid+8, mdata->id+8);
		plen = date_to_version(mdata->date_publi, mdata->uri_parts.tversion);
		if (plen < 0)
			return -1;
		len = plen + 1;
		mdata->uri_parts.tversion_datekind = PUB_DATEKIND;
		mdata->uri_parts.kind = PARTIAL_URI_VERSION;
	} else if (mdata->uri_parts.doctype == JORFSCTA_DOCTYPE) {
		strcpy(mdata->rid+8, mdata->id+8);
		plen = set_num_uri(mdata, mdata->uri_parts.snum, &mdata->uri_parts.snumkind,
				   NULL, NULL, mdata->id, "sid");
		if (plen < 0)
			return -1;
		len = plen + 1;
		plen = date_to_version(mdata->contexte.date_publi, mdata->uri_parts.sversion);
		if (plen < 0)
			return -1;
		len += plen;
		mdata->uri_parts.sversion_datekind = PUB_DATEKIND;
		mdata->uri_parts.kind = PARTIAL_URI_SNUM_VERSION;
	} else if (mdata->uri_parts.doctype == JORFARTI_DOCTYPE) {
		strcpy(mdata->rid+8, mdata->id+8);
		plen = set_num_uri(mdata, mdata->uri_parts.anum, &mdata->uri_parts.anumkind,
				   mdata->num, NULL, mdata->id, "aid");
		if (plen < 0)
			return -1;
		len = plen + 1;

		plen = date_to_version(mdata->contexte.date_publi, mdata->uri_parts.aversion);
		if (plen < 0)
			return -1;
		len += plen + 1;

		mdata->uri_parts.aversion_datekind = PUB_DATEKIND;
		mdata->uri_parts.kind = PARTIAL_URI_ANUM_VERSION;
	} else if (mdata->uri_parts.doctype == LEGIVERS_DOCTYPE) {
		strcpy(mdata->rid+8, mdata->id+8);
		plen = date_to_version(mdata->date_debut, mdata->uri_parts.tversion);
		if (plen < 0)
			return -1;
		len = plen;
		mdata->uri_parts.tversion_datekind = BEG_DATEKIND;
		mdata->uri_parts.kind = PARTIAL_URI_VERSION;
	} else if (mdata->uri_parts.doctype == LEGISCTA_DOCTYPE) {
		strcpy(mdata->rid+8, mdata->id+8);
		plen = set_num_uri(mdata, mdata->uri_parts.snum, &mdata->uri_parts.snumkind,
				   NULL, NULL, mdata->id, "sid");
		if (plen < 0)
			return -1;
		len = plen;
		mdata->uri_parts.kind = PARTIAL_URI_SNUM;
		/*
		 * Pas de date ici, car potentiellement il y a plusieurs dates de début (par ex,
		 * code rural et ses deux textversions...
		 * Ou alors il faudrait prendre la premiere?
		 */
	} else if (mdata->uri_parts.doctype == LEGIARTI_DOCTYPE) {
		strcpy(mdata->rid+8, mdata->id+8);
		plen = set_num_uri(mdata, mdata->uri_parts.anum, &mdata->uri_parts.anumkind,
				   mdata->num, NULL, mdata->id, "aid");
		if (plen < 0)
			return -1;
		len = plen + 1;

		plen = date_to_version(mdata->date_debut, mdata->uri_parts.aversion);
		if (plen < 0)
			return -1;
		len += plen + 1;
		mdata->uri_parts.aversion_datekind = BEG_DATEKIND;
		mdata->uri_parts.kind = PARTIAL_URI_ANUM_VERSION;
	}

	return len;
}