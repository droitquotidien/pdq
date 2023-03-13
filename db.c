#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "db.h"
#include "signature.h"

/*
 * https://gist.github.com/ictlyh/12fe787ec265b33fd7e4b0bd08bc27cb
 * https://www.postgresql.org/docs/current/libpq-exec.html
 */

void exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}

PGconn *db_connect(char *conninfo)
{
    PGconn *conn;
    PGresult *res;

    conn = PQconnectdb(conninfo);

    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "%s", PQerrorMessage(conn));
        exit_nicely(conn);
    }

    /* Set always-secure search path, so malicious users can't take control. */
    res = PQexec(conn,
                 "SELECT pg_catalog.set_config('search_path', '', false)");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
        PQclear(res);
        exit_nicely(conn);
    }

    /*
     * Should PQclear PGresult whenever it is no longer needed to avoid memory
     * leaks
     */
    PQclear(res);

    return conn;
}

void set_param(int n, const char *params[], const char *val)
{
	if (val == NULL || *val == 0)
		params[n] = NULL;
	else
		params[n] = val;
}

void update_ltag(PGconn *conn, enum doctype doctype, char id[], char tag[])
{
	int nb_tuples;
	PGresult *res;
	const char *param_values[3];
	static char *requests[] = {
		NULL,
		NULL,
		("UPDATE raw_jorflegi.import_jorf_cont SET ltag = $2"
		 " WHERE id = $1"), /*JORFCONT_DOCTYPE*/
		("UPDATE raw_jorflegi.import_jorf_text SET ltag = $2"
		 " WHERE id = $1"), /*JORFTEXT_DOCTYPE*/
		("UPDATE raw_jorflegi.import_jorf_vers SET ltag = $2"
		 " WHERE id = $1"), /*JORFVERS_DOCTYPE*/
		("UPDATE raw_jorflegi.import_jorf_scta SET ltag = $2"
		 " WHERE id = $1"), /*JORFSCTA_DOCTYPE*/
		("UPDATE raw_jorflegi.import_jorf_arti SET ltag = $2"
		 " WHERE id = $1"), /*JORFARTI_DOCTYPE*/
		("UPDATE raw_jorflegi.import_legi_text SET ltag = $2"
		 " WHERE id = $1"), /*LEGITEXT_DOCTYPE*/
		("UPDATE raw_jorflegi.import_legi_vers SET ltag = $2"
		 " WHERE id = $1"), /*LEGIVERS_DOCTYPE*/
		("UPDATE raw_jorflegi.import_legi_scta SET ltag = $2"
		 " WHERE id = $1"), /*LEGISCTA_DOCTYPE*/
		("UPDATE raw_jorflegi.import_legi_arti SET ltag = $2"
		 " WHERE id = $1"), /*LEGIARTI_DOCTYPE*/
	};
	param_values[0] = id;
	param_values[1] = tag;
	param_values[2] = NULL;

	res = PQexecParams(conn, requests[doctype],
			   2, NULL,
			   param_values, NULL,
			   NULL, 0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr,
			"UPDATE ltag failed %d: %s\n",
			PQresultStatus(res),
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
}

int get_document_signature(PGconn *conn, enum doctype doctype, char id[],
	char signature[], char tag[])
{
	int nb_tuples;
	PGresult *res;
	const char *param_values[2];
	static char *requests[] = {
		NULL,
		NULL,
		("SELECT sig, tag FROM raw_jorflegi.import_jorf_cont"
		 " WHERE id = $1"), /*JORFCONT_DOCTYPE*/
		("SELECT sig, tag FROM raw_jorflegi.import_jorf_text"
		 " WHERE id = $1"), /*JORFTEXT_DOCTYPE*/
		("SELECT sig, tag FROM raw_jorflegi.import_jorf_vers"
		 " WHERE id = $1"), /*JORFVERS_DOCTYPE*/
		("SELECT sig, tag FROM raw_jorflegi.import_jorf_scta"
		 " WHERE id = $1"), /*JORFSCTA_DOCTYPE*/
		("SELECT sig, tag FROM raw_jorflegi.import_jorf_arti"
		 " WHERE id = $1"), /*JORFARTI_DOCTYPE*/
		("SELECT sig, tag FROM raw_jorflegi.import_legi_text"
		 " WHERE id = $1"), /*LEGITEXT_DOCTYPE*/
		("SELECT sig, tag FROM raw_jorflegi.import_legi_vers"
		 " WHERE id = $1"), /*LEGIVERS_DOCTYPE*/
		("SELECT sig, tag FROM raw_jorflegi.import_legi_scta"
		 " WHERE id = $1"), /*LEGISCTA_DOCTYPE*/
		("SELECT sig, tag FROM raw_jorflegi.import_legi_arti"
		 " WHERE id = $1"), /*LEGIARTI_DOCTYPE*/
	};

	param_values[0] = id;
	param_values[1] = NULL;

	res = PQexecParams(conn, requests[doctype],
			   1, NULL,
			   param_values, NULL,
			   NULL, 0);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(res),
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(res);
	if (nb_tuples > 0) {
		strcpy(signature,
		       PQgetvalue(res, 0, 0));
		strcpy(tag,
		       PQgetvalue(res, 0, 1));
	}
	PQclear(res);
	return nb_tuples;
}

int get_document_tags(PGconn *conn, enum doctype doctype, char id[],
		      char tag[], char ltag[])
{
	int nb_tuples;
	PGresult *res;
	const char *param_values[2];
	static char *requests[] = {
		NULL,
		NULL,
		("SELECT tag, ltag FROM raw_jorflegi.import_jorf_cont"
		 " WHERE id = $1"), /*JORFCONT_DOCTYPE*/
		("SELECT tag, ltag FROM raw_jorflegi.import_jorf_text"
		 " WHERE id = $1"), /*JORFTEXT_DOCTYPE*/
		("SELECT tag, ltag FROM raw_jorflegi.import_jorf_vers"
		 " WHERE id = $1"), /*JORFVERS_DOCTYPE*/
		("SELECT tag, ltag FROM raw_jorflegi.import_jorf_scta"
		 " WHERE id = $1"), /*JORFSCTA_DOCTYPE*/
		("SELECT tag, ltag FROM raw_jorflegi.import_jorf_arti"
		 " WHERE id = $1"), /*JORFARTI_DOCTYPE*/
		("SELECT tag, ltag FROM raw_jorflegi.import_legi_text"
		 " WHERE id = $1"), /*LEGITEXT_DOCTYPE*/
		("SELECT tag, ltag FROM raw_jorflegi.import_legi_vers"
		 " WHERE id = $1"), /*LEGIVERS_DOCTYPE*/
		("SELECT tag, ltag FROM raw_jorflegi.import_legi_scta"
		 " WHERE id = $1"), /*LEGISCTA_DOCTYPE*/
		("SELECT tag, ltag FROM raw_jorflegi.import_legi_arti"
		 " WHERE id = $1"), /*LEGIARTI_DOCTYPE*/
	};

	param_values[0] = id;
	param_values[1] = NULL;

	res = PQexecParams(conn, requests[doctype],
			   1, NULL,
			   param_values, NULL,
			   NULL, 0);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(res),
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(res);
	if (nb_tuples > 0) {
		strcpy(tag,
		       PQgetvalue(res, 0, 0));
		strcpy(ltag,
		       PQgetvalue(res, 0, 1));
	}
	PQclear(res);
	return nb_tuples;
}

int delete(PGconn *conn, enum doctype doctype, char id[], char tag[])
{
	int nb_tuples;
	PGresult *res;
	const char *param_values[4];
	char c_tag[64];
	char c_ltag[64];
	static char *upd_requests[] = {
		NULL,
		NULL,
		("INSERT INTO raw_jorflegi.updated_jorf_cont"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"),
		 /*JORFCONT_DOCTYPE*/
		("INSERT INTO raw_jorflegi.updated_jorf_text"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"), /*JORFTEXT_DOCTYPE*/
		("INSERT INTO raw_jorflegi.updated_jorf_vers"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"), /*JORFVERS_DOCTYPE*/
		("INSERT INTO raw_jorflegi.updated_jorf_scta"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"), /*JORFSCTA_DOCTYPE*/
		("INSERT INTO raw_jorflegi.updated_jorf_arti"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"), /*JORFARTI_DOCTYPE*/
		("INSERT INTO raw_jorflegi.updated_legi_text"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"), /*LEGITEXT_DOCTYPE*/
		("INSERT INTO raw_jorflegi.updated_legi_vers"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"), /*LEGIVERS_DOCTYPE*/
		("INSERT INTO raw_jorflegi.updated_legi_scta"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"), /*LEGISCTA_DOCTYPE*/
		("INSERT INTO raw_jorflegi.updated_legi_arti"
		 "(id, tag, ltag, updates, mod_tm)"
		 "VALUES"
		 "($1, $2, $3, 'DEL', NOW()::timestamp)"), /*LEGIARTI_DOCTYPE*/
	};
	static char *del_requests[] = {
		NULL,
		NULL,
		("DELETE FROM raw_jorflegi.import_jorf_cont"
		 " WHERE id = $1"), /*JORFCONT_DOCTYPE*/
		("DELETE FROM raw_jorflegi.import_jorf_text"
		 " WHERE id = $1"), /*JORFTEXT_DOCTYPE*/
		("DELETE FROM raw_jorflegi.import_jorf_vers"
		 " WHERE id = $1"), /*JORFVERS_DOCTYPE*/
		("DELETE FROM raw_jorflegi.import_jorf_scta"
		 " WHERE id = $1"), /*JORFSCTA_DOCTYPE*/
		("DELETE FROM raw_jorflegi.import_jorf_arti"
		 " WHERE id = $1"), /*JORFARTI_DOCTYPE*/
		("DELETE FROM raw_jorflegi.import_legi_text"
		 " WHERE id = $1"), /*LEGITEXT_DOCTYPE*/
		("DELETE FROM raw_jorflegi.import_legi_vers"
		 " WHERE id = $1"), /*LEGIVERS_DOCTYPE*/
		("DELETE FROM raw_jorflegi.import_legi_scta"
		 " WHERE id = $1"), /*LEGISCTA_DOCTYPE*/
		("DELETE FROM raw_jorflegi.import_legi_arti"
		 " WHERE id = $1"), /*LEGIARTI_DOCTYPE*/
	};

	if (get_document_tags(conn, doctype, id,c_tag,c_ltag) == 0) {
		/* Document does not exists */
		return 0;
	}

	/* Create an updated doc with DEL as the "updates" field */
	param_values[0] = id;
	param_values[1] = tag; /* tag */
	param_values[2] = c_ltag; /* ltag */
	if (c_ltag[0] == '\0')
		param_values[2] = NULL;
	param_values[3] = NULL;
	res = PQexecParams(conn, upd_requests[doctype],
			   3, NULL,
			   param_values, NULL,
			   NULL, 0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr,
			"UPDATE with DEL failed %d: %s\n",
			PQresultStatus(res),
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);

	/* Delete the document */
	param_values[0] = id;
	param_values[1] = NULL;
	res = PQexecParams(conn, del_requests[doctype],
			   1, NULL,
			   param_values, NULL,
			   NULL, 0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr,
			"UPDATE with DEL failed %d: %s\n",
			PQresultStatus(res),
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);

	return 1;
}

char *has_been_updated(PGresult *res, int field, char *new, char *updated)
{
	char *old = PQgetvalue(res, 0, field);
	*updated = ' ';
	if (old == NULL && new != NULL) {
		*updated = '+';
		return new;
	}
	if (old != NULL && new == NULL) {
		*updated = '-';
		return NULL;
	}
	if (strcmp(old, new) == 0)
		return NULL;
	*updated = '*';
	return new;
}

void create_updated_jorf_arti(PGconn *conn,
			      char *id, char *rid, char *num,
			      char *date_debut, char *date_fin,
			      char *type, char *mcs, char* liens,
			      char *contexte, char *contenu, char *versions)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[15];
	char updates[11];
	static char request[] = (
		"SELECT id, rid, num, date_debut, date_fin, type,"
		"       mcs, liens, contexte, contenu, versions,"
		"       tag, ltag FROM raw_jorflegi.import_jorf_arti"
		"  WHERE id = $1"
		);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			   1, NULL, sparam_values,
			   NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	/*
	fprintf(stderr, "SELECT OK\n");

	fprintf(stderr, "%s %s %s %s %s %s\n",
		id, rid, num, date_debut, date_fin, type);
	fprintf(stderr, "%s %s\n",
		PQgetvalue(sres, 0, 11),
		PQgetvalue(sres, 0, 12));
	*/
	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, rid, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, num, &updates[1]);
	set_param(3, param_values, has_been_updated(sres, 3, date_debut, &updates[2]));
	set_param(4, param_values, has_been_updated(sres, 4, date_fin, &updates[3]));
	param_values[5] = has_been_updated(sres, 5, type, &updates[4]);
	param_values[6] = has_been_updated(sres, 6, mcs, &updates[5]);
	param_values[7] = has_been_updated(sres, 7, liens, &updates[6]);
	param_values[8] = has_been_updated(sres, 8, contexte, &updates[7]);
	param_values[9] = has_been_updated(sres, 9, contenu, &updates[8]);
	param_values[10] = has_been_updated(sres, 10, versions, &updates[9]);
	updates[10] = '\0';
	set_param(11, param_values, PQgetvalue(sres, 0, 11));
	set_param(12, param_values, PQgetvalue(sres, 0, 12));
	param_values[13] = updates;
	param_values[14] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO raw_jorflegi.updated_jorf_arti"
			   " (id, rid, num,"
			   "  date_debut, date_fin, type,"
			   "  mcs, liens, contexte, contenu, versions,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3,"
			   "  $4, $5, $6,"
			   "  $7, $8, $9, $10, $11,"
			   "  $12, $13, NOW()::timestamp, $14)"
			   " ON CONFLICT DO NOTHING",
			   14,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}


void create_updated_jorf_scta(PGconn *conn,
			      char *id, char *rid, char *titrefull,
			      char *commentaire, char *contexte, char *toc)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[10];
	char updates[6];
	static char request[] = (
		"SELECT id, rid, titrefull, commentaire,"
		"       contexte, toc,"
		"       tag, ltag FROM raw_jorflegi.import_jorf_scta"
		"  WHERE id = $1"
	);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			    1, NULL, sparam_values,
			    NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	/*
	fprintf(stderr, "SELECT OK\n");

	fprintf(stderr, "%s %s %s %s %s %s\n",
		id, rid, num, date_debut, date_fin, type);
	fprintf(stderr, "%s %s\n",
		PQgetvalue(sres, 0, 11),
		PQgetvalue(sres, 0, 12));
	*/
	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, rid, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, titrefull, &updates[1]);
	param_values[3] = has_been_updated(sres, 3, commentaire, &updates[2]);
	param_values[4] = has_been_updated(sres, 4, contexte, &updates[3]);
	param_values[5] = has_been_updated(sres, 5, toc, &updates[4]);
	updates[5] = '\0';
	set_param(6, param_values, PQgetvalue(sres, 0, 6));
	set_param(7, param_values, PQgetvalue(sres, 0, 7));
	param_values[8] = updates;
	param_values[9] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO raw_jorflegi.updated_jorf_scta"
			   " (id, rid, titrefull, commentaire,"
			   "  contexte, toc,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3, $4,"
			   "  $5, $6,"
			   "  $7, $8, NOW()::timestamp, $9)"
			   " ON CONFLICT DO NOTHING",
			   9,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}


void create_updated_jorf_cont(PGconn *conn,
			      char *id, char *titre, char *num,
			      char *date_publi, char *toc)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[9];
	char updates[5];
	static char request[] = (
		"SELECT id, titre, num, date_publi, toc,"
		"       tag, ltag FROM raw_jorflegi.import_jorf_cont"
		"  WHERE id = $1"
	);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			    1, NULL, sparam_values,
			    NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	/*
	fprintf(stderr, "SELECT OK\n");

	fprintf(stderr, "%s %s %s %s %s %s\n",
		id, rid, num, date_debut, date_fin, type);
	fprintf(stderr, "%s %s\n",
		PQgetvalue(sres, 0, 11),
		PQgetvalue(sres, 0, 12));
	*/
	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, titre, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, num, &updates[1]);
	set_param(3, param_values, has_been_updated(sres, 3, date_publi, &updates[2]));
	param_values[4] = has_been_updated(sres, 4, toc, &updates[3]);
	updates[4] = '\0';
	set_param(5, param_values, PQgetvalue(sres, 0, 5));
	set_param(6, param_values, PQgetvalue(sres, 0, 6));
	param_values[7] = updates;
	param_values[8] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO raw_jorflegi.updated_jorf_cont"
			   " (id, titre, num, date_publi,"
			   "  toc,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3, $4,"
			   "  $5,"
			   "  $6, $7, NOW()::timestamp, $8)"
			   " ON CONFLICT DO NOTHING",
			   8,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}

void create_updated_jorf_text(PGconn *conn,
			      char *id, char *cid, char *rid, char *nature,
			      char *num, char *nor, char *num_parution,
			      char *num_sequence, char *origine_publi,
			      char *page_deb_publi, char *page_fin_publi,
			      char *date_publi, char *date_texte,
			      char *derniere_modification,
			      char *toc, char *versions)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[20];
	char updates[16];
	static char request[] = (
		"SELECT id, cid, rid, nature, num, nor, num_parution,"
		"       num_sequence, origine_publi, page_deb_publi,"
		"       page_fin_publi, date_publi, date_texte,"
		"       derniere_modification,"
		"       toc, versions,"
		"       tag, ltag FROM raw_jorflegi.import_jorf_text"
		"  WHERE id = $1"
	);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			    1, NULL, sparam_values,
			    NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, cid, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, rid, &updates[1]);
	param_values[3] = has_been_updated(sres, 3, nature, &updates[2]);
	param_values[4] = has_been_updated(sres, 4, num, &updates[3]);
	param_values[5] = has_been_updated(sres, 5, nor, &updates[4]);
	param_values[6] = has_been_updated(sres, 6, num_parution, &updates[5]);
	param_values[7] = has_been_updated(sres, 7, num_sequence, &updates[6]);
	param_values[8] = has_been_updated(sres, 8, origine_publi, &updates[7]);
	param_values[9] = has_been_updated(sres, 9, page_deb_publi, &updates[8]);
	param_values[10] = has_been_updated(sres, 10, page_fin_publi, &updates[9]);
	set_param(11, param_values, has_been_updated(sres, 11, date_publi, &updates[10]));
	set_param(12, param_values, has_been_updated(sres, 12, date_texte, &updates[11]));
	set_param(13, param_values, has_been_updated(sres, 13, derniere_modification, &updates[12]));
	param_values[14] = has_been_updated(sres, 14, toc, &updates[13]);
	param_values[15] = has_been_updated(sres, 15, versions, &updates[14]);
	updates[15] = '\0';
	set_param(16, param_values, PQgetvalue(sres, 0, 16));
	set_param(17, param_values, PQgetvalue(sres, 0, 17));
	param_values[18] = updates;
	param_values[19] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO"
			   " raw_jorflegi.updated_jorf_text"
			   " (id, cid, rid, nature, num, nor,"
			   "  num_parution, num_sequence,"
			   "  origine_publi, page_deb_publi, page_fin_publi,"
			   "  date_publi,"
			   "  date_texte, derniere_modification, toc, versions,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3, $4, $5, $6,"
			   "  $7, $8,"
			   "  $9, $10, $11,"
			   "  $12,"
			   "  $13, $14, $15, $16,"
			   "  $17, $18, NOW()::timestamp, $19)"
			   " ON CONFLICT DO NOTHING",
			   19,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}


void create_updated_jorf_vers(PGconn *conn,
			      char *id, char *cid, char *rid,
			      char *titre, char *titrefull,
			      char *autorite, char *ministere,
			      char *date_debut, char *date_fin,
			      char *mcs, char *liens, char *contexte,
			      char *contenu)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[17];
	char updates[13];
	static char request[] = (
		"SELECT id, cid, rid, titre, titrefull, autorite, ministere,"
		"       date_debut, date_fin, mcs, liens, contexte, contenu,"
		"       tag, ltag FROM raw_jorflegi.import_jorf_vers"
		"  WHERE id = $1"
	);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			    1, NULL, sparam_values,
			    NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, cid, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, rid, &updates[1]);
	param_values[3] = has_been_updated(sres, 3, titre, &updates[2]);
	param_values[4] = has_been_updated(sres, 4, titrefull, &updates[3]);
	param_values[5] = has_been_updated(sres, 5, autorite, &updates[4]);
	param_values[6] = has_been_updated(sres, 6, ministere, &updates[5]);
	set_param(7, param_values, has_been_updated(sres, 7, date_debut, &updates[6]));
	set_param(8, param_values, has_been_updated(sres, 8, date_fin, &updates[7]));
	param_values[9] = has_been_updated(sres, 9, mcs, &updates[8]);
	param_values[10] = has_been_updated(sres, 10, liens, &updates[9]);
	param_values[11] = has_been_updated(sres, 11, contexte, &updates[10]);
	param_values[12] = has_been_updated(sres, 12, contenu, &updates[11]);
	updates[12] = '\0';
	set_param(13, param_values, PQgetvalue(sres, 0, 13));
	set_param(14, param_values, PQgetvalue(sres, 0, 14));
	param_values[15] = updates;
	param_values[16] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO"
			   " raw_jorflegi.updated_jorf_vers"
			   " (id, cid, rid,"
			   "  titre, titrefull, autorite, ministere,"
			   "  date_debut, date_fin, mcs, liens, contexte, contenu,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3, $4, $5, $6, $7,"
			   "  $8, $9, $10, $11, $12, $13,"
			   "  $14, $15, NOW()::timestamp, $16)"
			   " ON CONFLICT DO NOTHING",
			   16,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}

void create_updated_legi_arti(PGconn *conn,
			      char *id, char *rid, char *num,
			      char *etat,
			      char *date_debut, char *date_fin,
			      char *type, char* liens,
			      char *contexte, char *contenu, char *versions)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[15];
	char updates[11];
	static char request[] = (
		"SELECT id, rid, num, etat, date_debut, date_fin, type,"
		"       liens, contexte, contenu, versions,"
		"       tag, ltag FROM raw_jorflegi.import_legi_arti"
		"  WHERE id = $1"
	);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			    1, NULL, sparam_values,
			    NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	/*
	fprintf(stderr, "SELECT OK\n");

	fprintf(stderr, "%s %s %s %s %s %s\n",
		id, rid, num, date_debut, date_fin, type);
	fprintf(stderr, "%s %s\n",
		PQgetvalue(sres, 0, 11),
		PQgetvalue(sres, 0, 12));
	*/
	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, rid, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, num, &updates[1]);
	param_values[3] = has_been_updated(sres, 3, etat, &updates[2]);

	set_param(4, param_values,
		  has_been_updated(sres, 4, date_debut, &updates[3]));
	set_param(5, param_values,
		  has_been_updated(sres, 5, date_fin, &updates[4]));
	param_values[6] = has_been_updated(sres, 6, type, &updates[5]);
	param_values[7] = has_been_updated(sres, 7, liens, &updates[6]);
	param_values[8] = has_been_updated(sres, 8, contexte, &updates[7]);
	param_values[9] = has_been_updated(sres, 9, contenu, &updates[8]);
	param_values[10] = has_been_updated(sres, 10, versions, &updates[9]);
	updates[10] = '\0';
	set_param(11, param_values, PQgetvalue(sres, 0, 11));
	set_param(12, param_values, PQgetvalue(sres, 0, 12));
	param_values[13] = updates;
	param_values[14] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO raw_jorflegi.updated_legi_arti"
			   " (id, rid, num, etat,"
			   "  date_debut, date_fin, type,"
			   "  liens, contexte, contenu, versions,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3,"
			   "  $4, $5, $6,"
			   "  $7, $8, $9, $10, $11,"
			   "  $12, $13, NOW()::timestamp, $14)"
			   " ON CONFLICT DO NOTHING",
			   14,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}


void create_updated_legi_scta(PGconn *conn,
			      char *id, char *rid, char *titrefull,
			      char *commentaire, char *contexte, char *toc)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[10];
	char updates[6];
	static char request[] = (
		"SELECT id, rid, titrefull, commentaire,"
		"       contexte, toc,"
		"       tag, ltag FROM raw_jorflegi.import_legi_scta"
		"  WHERE id = $1"
	);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			    1, NULL, sparam_values,
			    NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	/*
	fprintf(stderr, "SELECT OK\n");

	fprintf(stderr, "%s %s %s %s %s %s\n",
		id, rid, num, date_debut, date_fin, type);
	fprintf(stderr, "%s %s\n",
		PQgetvalue(sres, 0, 11),
		PQgetvalue(sres, 0, 12));
	*/
	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, rid, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, titrefull, &updates[1]);
	param_values[3] = has_been_updated(sres, 3, commentaire, &updates[2]);
	param_values[4] = has_been_updated(sres, 4, contexte, &updates[3]);
	param_values[5] = has_been_updated(sres, 5, toc, &updates[4]);
	updates[5] = '\0';
	set_param(6, param_values, PQgetvalue(sres, 0, 6));
	set_param(7, param_values, PQgetvalue(sres, 0, 7));
	param_values[8] = updates;
	param_values[9] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO raw_jorflegi.updated_legi_scta"
			   " (id, rid, titrefull, commentaire,"
			   "  contexte, toc,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3, $4,"
			   "  $5, $6,"
			   "  $7, $8, NOW()::timestamp, $9)"
			   " ON CONFLICT DO NOTHING",
			   9,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}


void create_updated_legi_text(PGconn *conn,
			      char *id, char *cid, char *rid, char *nature,
			      char *num, char *nor, char *num_parution,
			      char *num_sequence, char *origine_publi,
			      char *page_deb_publi, char *page_fin_publi,
			      char *date_publi, char *date_texte,
			      char *derniere_modification,
			      char *toc, char *versions)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[20];
	char updates[16];
	static char request[] = (
		"SELECT id, cid, rid, nature, num, nor, num_parution,"
		"       num_sequence, origine_publi, page_deb_publi,"
		"       page_fin_publi, date_publi, date_texte,"
		"       derniere_modification,"
		"       toc, versions,"
		"       tag, ltag FROM raw_jorflegi.import_legi_text"
		"  WHERE id = $1"
	);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			    1, NULL, sparam_values,
			    NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, cid, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, rid, &updates[1]);
	param_values[3] = has_been_updated(sres, 3, nature, &updates[2]);
	param_values[4] = has_been_updated(sres, 4, num, &updates[3]);
	param_values[5] = has_been_updated(sres, 5, nor, &updates[4]);
	param_values[6] = has_been_updated(sres, 6, num_parution, &updates[5]);
	param_values[7] = has_been_updated(sres, 7, num_sequence, &updates[6]);
	param_values[8] = has_been_updated(sres, 8, origine_publi, &updates[7]);
	param_values[9] = has_been_updated(sres, 9, page_deb_publi, &updates[8]);
	param_values[10] = has_been_updated(sres, 10, page_fin_publi, &updates[9]);
	set_param(11, param_values, has_been_updated(sres, 11, date_publi, &updates[10]));
	set_param(12, param_values, has_been_updated(sres, 12, date_texte, &updates[11]));
	set_param(13, param_values, has_been_updated(sres, 13, derniere_modification, &updates[12]));
	param_values[14] = has_been_updated(sres, 14, toc, &updates[13]);
	param_values[15] = has_been_updated(sres, 15, versions, &updates[14]);
	updates[15] = '\0';
	set_param(16, param_values, PQgetvalue(sres, 0, 16));
	set_param(17, param_values, PQgetvalue(sres, 0, 17));
	param_values[18] = updates;
	param_values[19] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO"
			   " raw_jorflegi.updated_legi_text"
			   " (id, cid, rid, nature, num, nor,"
			   "  num_parution, num_sequence,"
			   "  origine_publi, page_deb_publi, page_fin_publi,"
			   "  date_publi,"
			   "  date_texte, derniere_modification, toc, versions,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3, $4, $5, $6,"
			   "  $7, $8,"
			   "  $9, $10, $11,"
			   "  $12,"
			   "  $13, $14, $15, $16,"
			   "  $17, $18, NOW()::timestamp, $19)"
			   " ON CONFLICT DO NOTHING",
			   19,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}


void create_updated_legi_vers(PGconn *conn,
			      char *id, char *cid, char *rid,
			      char *titre, char *titrefull,
			      char *autorite, char *ministere, char *etat,
			      char *date_debut, char *date_fin,
			      char *liens, char *contexte,
			      char *contenu)
{
	int nb_tuples;
	PGresult *sres;
	PGresult *res;
	const char *sparam_values[2];
	const char *param_values[17];
	char updates[13];
	static char request[] = (
		"SELECT id, cid, rid, titre, titrefull, autorite, ministere,"
		"       etat, date_debut, date_fin, liens, contexte, contenu,"
		"       tag, ltag FROM raw_jorflegi.import_legi_vers"
		"  WHERE id = $1"
	);

	sparam_values[0] = id;
	sparam_values[1] = NULL;

	sres = PQexecParams(conn, request,
			    1, NULL, sparam_values,
			    NULL, NULL, 0);
	if (PQresultStatus(sres) != PGRES_TUPLES_OK) {
		fprintf(stderr, "SELECT failed %d: %s\n",
			PQresultStatus(sres),
			PQresultErrorMessage(sres));
		PQclear(sres);
		exit_nicely(conn);
	}
	nb_tuples = PQntuples(sres);
	assert (nb_tuples == 1);

	param_values[0] = id;
	param_values[1] = has_been_updated(sres, 1, cid, &updates[0]);
	param_values[2] = has_been_updated(sres, 2, rid, &updates[1]);
	param_values[3] = has_been_updated(sres, 3, titre, &updates[2]);
	param_values[4] = has_been_updated(sres, 4, titrefull, &updates[3]);
	param_values[5] = has_been_updated(sres, 5, autorite, &updates[4]);
	param_values[6] = has_been_updated(sres, 6, ministere, &updates[5]);
	param_values[7] = has_been_updated(sres, 7, etat, &updates[6]);
	set_param(8, param_values, has_been_updated(sres, 8, date_debut, &updates[7]));
	set_param(9, param_values, has_been_updated(sres, 9, date_fin, &updates[8]));
	param_values[10] = has_been_updated(sres, 10, liens, &updates[9]);
	param_values[11] = has_been_updated(sres, 11, contexte, &updates[10]);
	param_values[12] = has_been_updated(sres, 12, contenu, &updates[11]);
	updates[12] = '\0';
	set_param(13, param_values, PQgetvalue(sres, 0, 13));
	set_param(14, param_values, PQgetvalue(sres, 0, 14));
	param_values[15] = updates;
	param_values[16] = NULL;

	res = PQexecParams(conn,
			   "INSERT INTO"
			   " raw_jorflegi.updated_legi_vers"
			   " (id, cid, rid,"
			   "  titre, titrefull, autorite, ministere,"
			   "  etat, date_debut, date_fin, liens, contexte, contenu,"
			   "  tag, ltag, mod_tm, updates)"
			   " VALUES"
			   " ($1, $2, $3, $4, $5, $6, $7,"
			   "  $8, $9, $10, $11, $12, $13,"
			   "  $14, $15, NOW()::timestamp, $16)"
			   " ON CONFLICT DO NOTHING",
			   16,
			   NULL,
			   param_values,
			   NULL,
			   NULL,
			   0);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "INSERT failed: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	PQclear(sres);
}


int db_import(PGconn *conn, const EVP_MD *md, struct tm *tag,
	struct parsed_data *pdata,
	struct write_buffer *dbbuf1,
	struct write_buffer *dbbuf2,
	struct write_buffer *dbbuf3,
	struct write_buffer *dbbuf4,
	struct write_buffer *dbbuf5,
	struct write_buffer *dbbuf6,
	struct timings *tt,
	char bootstrap,
	FILE *log_file)
{
	ssize_t r;
	struct metadata *mdata = pdata->metadata;
	struct toc *toc = pdata->toc;
	struct mcs *mcs = pdata->mcs;
	struct liens *liens = pdata->liens;
	struct contexte *contexte = &mdata->contexte;
	struct contenu *contenu = pdata->contenu;
	struct versions *versions = pdata->versions;
	PGresult *res;
	const char *param_values[21];
	clock_t ct;
	char stag[20];
	double sig_tt; // Time taken by signature
	char signature[SIG_HEX_MAX];
	char c_signature[SIG_HEX_MAX];
	char c_tag[64];
	unsigned char digest[EVP_MAX_MD_SIZE];
	unsigned int digest_len;
	char to_create = 1;

	sprintf(stag, "%04d-%02d-%02d %02d:%02d:%02d",
		tag->tm_year, tag->tm_mon, tag->tm_mday,
		tag->tm_hour, tag->tm_min, tag->tm_sec);

	switch (mdata->uri_parts.doctype) {

	case JORFARTI_DOCTYPE:
		r = write_mcs_json(-1, mcs, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_liens_json(-1, liens, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;
		r = write_contexte_json(-1, contexte, dbbuf3, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf3->buffer[dbbuf3->current_size] = 0;
		r = write_contenu_json(-1, contenu, dbbuf4, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf4->buffer[dbbuf4->current_size] = 0;
		r = write_versions_json(-1, versions, dbbuf5, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf5->buffer[dbbuf5->current_size] = 0;
		param_values[0] = mdata->id;
		param_values[1] = mdata->rid;
		param_values[2] = mdata->num;
		set_param(3, param_values, mdata->date_debut);
		set_param(4, param_values, mdata->date_fin);
		param_values[5] = mdata->type;
		param_values[6] = dbbuf1->buffer;  // mcs
		param_values[7] = dbbuf2->buffer;  // liens
		param_values[8] = dbbuf3->buffer;  // contexte
		param_values[9] = dbbuf4->buffer;  // contenu
		param_values[10] = dbbuf5->buffer;  // versions
		compute_signature(md, param_values, 11,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			/* Get existing document signature and tag */
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->id, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->id, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->id, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->id,
							stag);
					}
					buffer_reset(dbbuf1);
					buffer_reset(dbbuf2);
					buffer_reset(dbbuf3);
					buffer_reset(dbbuf4);
					buffer_reset(dbbuf5);
					return 0;
				} else {
					/* Content is different */
					/*
					 * COMPARE new WITH previous
					 * STORE differences IN updated_xxxx
					 * UPDATE doc ATTRIBUTES
		 			 * UPDATE doc.tag AND doc.ltag
					 * UPDATE doc.mod_tm AND doc.sig
		 			 * DO NOT CHANGE URI/URI PARTS (?)
					 */
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->id, c_tag);
					create_updated_jorf_arti(
						conn, mdata->id,
						mdata->rid,
						mdata->num,
						mdata->date_debut,
						mdata->date_fin,
						mdata->type,
						dbbuf1->buffer,
						dbbuf2->buffer,
						dbbuf3->buffer,
						dbbuf4->buffer,
						dbbuf5->buffer);
					to_create = 0;
				}
			}
		}

		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->id, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf6, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf6->buffer[dbbuf6->current_size] = 0;
			param_values[11] = mdata->id;
			param_values[12] = mdata->uri;
			param_values[13] = dbbuf6->buffer;  // uri_parts
			param_values[14] = stag;
			/* TODO: ltag */
			param_values[15] = signature;
			param_values[16] = NULL;
			tt->db_insert = clock();
			/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
			res = PQexecParams(conn,
					   "INSERT INTO "
					   "raw_jorflegi.import_jorf_arti"
					   " (cid, rid, num,"
					   "  date_debut, date_fin, type,"
					   "  mcs, liens, contexte, contenu,"
					   "  versions,"
					   "  id, uri, uri_parts, tag, sig,"
					   "  mod_tm)"
					   " VALUES"
					   " ($1, $2, $3,"
					   "  $4, $5, $6,"
					   "  $7, $8, $9, $10,"
					   "  $11,"
					   "  $12, $13, $14, $15, $16,"
					   "  NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   16,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf6);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->id, stag);
			param_values[11] = stag; /* tag */
			param_values[12] = stag; /* ltag */
			param_values[13] = signature; /* sig */
			param_values[14] = mdata->id;
			param_values[15] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_jorf_arti"
					   " SET (cid, rid, num,"
					   "  date_debut, date_fin, type,"
					   "  mcs, liens, contexte, contenu,"
					   "  versions,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3,"
					   "  $4, $5, $6,"
					   "  $7, $8, $9, $10,"
					   "  $11,"
					   "  $12, $13, $14, NOW()::timestamp)"
					   " WHERE id = $15",
					   15,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}

		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		buffer_reset(dbbuf3);
		buffer_reset(dbbuf4);
		buffer_reset(dbbuf5);
		PQclear(res);

		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;

	case JORFSCTA_DOCTYPE:
		/*fprintf(stderr, "INFO:%s: insert in DB\n",
			mdata->rid);*/
		r = write_contexte_json(-1, contexte, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_toc_json(-1, toc, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;
		param_values[0] = mdata->rid;
		param_values[1] = mdata->titrefull;
		param_values[2] = mdata->commentaire;
		param_values[3] = dbbuf1->buffer;  // contexte
		param_values[4] = dbbuf2->buffer;  // toc
		compute_signature(md, param_values, 5,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->id, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->id, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->id, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->id,
							stag);
					}
					buffer_reset(dbbuf1);
					buffer_reset(dbbuf2);
					return 0;
				} else {
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->id, c_tag);
					create_updated_jorf_scta(
						conn, mdata->id,
						mdata->rid,
						mdata->titrefull,
						mdata->commentaire,
						dbbuf1->buffer,
						dbbuf2->buffer);
					to_create = 0;
				}
			}
		}

		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->id, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf3, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf3->buffer[dbbuf3->current_size] = 0;
			param_values[5] = mdata->id;
			param_values[6] = mdata->uri;
			param_values[7] = dbbuf3->buffer;  // uri_parts
			param_values[8] = stag;
			param_values[9] = signature;
			param_values[10] = NULL;
			tt->db_insert = clock();
			res = PQexecParams(conn,
					   "INSERT INTO"
					   " raw_jorflegi.import_jorf_scta"
					   " (rid, titrefull, commentaire,"
					   "  contexte, toc,"
					   "  id, uri, uri_parts, tag, sig,"
					   "  mod_tm)"
					   " VALUES"
					   " ($1, $2, $3, $4, $5,"
					   "  $6, $7, $8, $9, $10,"
					   "  NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   10,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf3);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->id, stag);
			param_values[5] = stag; /* tag */
			param_values[6] = stag; /* ltag */
			param_values[7] = signature; /* sig */
			param_values[8] = mdata->id;
			param_values[9] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_jorf_scta"
					   " SET ("
					   "  rid, titrefull, commentaire,"
					   "  contexte, toc,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3,"
					   "  $4, $5,"
					   "  $6, $7, $8,"
					   "  NOW()::timestamp)"
					   " WHERE id = $9",
					   9,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}

		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		PQclear(res);

		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;

	case JORFVERS_DOCTYPE:
		r = write_mcs_json(-1, mcs, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_liens_json(-1, liens, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;
		r = write_contexte_json(-1, contexte, dbbuf3, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf3->buffer[dbbuf3->current_size] = 0;
		r = write_contenu_json(-1, contenu, dbbuf4, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf4->buffer[dbbuf4->current_size] = 0;

		param_values[0] = mdata->cid;
		param_values[1] = mdata->id;
		param_values[2] = mdata->titre;
		param_values[3] = mdata->titrefull;
		param_values[4] = mdata->autorite;
		param_values[5] = mdata->ministere;
		set_param(6, param_values, mdata->date_debut);
		set_param(7, param_values, mdata->date_fin);
		param_values[8] = dbbuf1->buffer;  // mcs
		param_values[9] = dbbuf2->buffer;  // liens
		param_values[10] = dbbuf3->buffer;  // contexte
		param_values[11] = dbbuf4->buffer;  // contenu
		// FIXME: entreprise
		compute_signature(md, param_values, 12,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->rid, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->rid, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->rid, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->rid,
							stag);
					}
					buffer_reset(dbbuf1);
					buffer_reset(dbbuf2);
					buffer_reset(dbbuf3);
					buffer_reset(dbbuf4);
					return 0;
				} else {
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->rid, c_tag);
					create_updated_jorf_vers(
						conn, mdata->rid,
						mdata->cid,
						mdata->id,
						mdata->titre,
						mdata->titrefull,
						mdata->autorite,
						mdata->ministere,
						mdata->date_debut,
						mdata->date_fin,
						dbbuf1->buffer,
						dbbuf2->buffer,
						dbbuf3->buffer,
						dbbuf4->buffer);
					to_create = 0;
				}
			}
		}

		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->rid, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf5, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf5->buffer[dbbuf5->current_size] = 0;
			param_values[12] = mdata->rid;
			param_values[13] = mdata->uri;
			param_values[14] = dbbuf5->buffer;  // uri_parts
			param_values[15] = stag;
			param_values[16] = signature;
			param_values[17] = NULL;
			tt->db_insert = clock();
			res = PQexecParams(conn,
					   "INSERT INTO "
					   " raw_jorflegi.import_jorf_vers"
					   " (cid, rid, titre, titrefull, autorite,"
					   "  ministere,"
					   "  date_debut, date_fin, mcs, liens,"
					   "  contexte, contenu,"
					   "  id, uri, uri_parts, tag, sig, mod_tm)"
					   " VALUES"
					   " ($1, $2, $3, $4, $5,"
					   "  $6,"
					   "  $7, $8, $9, $10,"
					   "  $11, $12,"
					   "  $13, $14, $15, $16, $17, NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   17,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf5);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->rid, stag);
			param_values[12] = stag; /* tag */
			param_values[13] = stag; /* ltag */
			param_values[14] = signature; /* sig */
			param_values[15] = mdata->rid;
			param_values[16] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_jorf_vers"
					   " SET ("
					   "  cid, rid, titre, titrefull, autorite,"
					   "  ministere,"
					   "  date_debut, date_fin, mcs, liens,"
					   "  contexte, contenu,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3, $4, $5,"
					   "  $6, "
					   "  $7, $8, $9, $10,"
					   "  $11, $12,"
					   "  $13, $14, $15, NOW()::timestamp)"
					   " WHERE id = $16",
					   16,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}
		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		buffer_reset(dbbuf3);
		buffer_reset(dbbuf4);
		PQclear(res);
		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;

	case JORFTEXT_DOCTYPE:
		r = write_toc_json(-1, toc, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_versions_json(-1, versions, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;

		param_values[0] = mdata->cid;
		param_values[1] = mdata->rid;
		param_values[2] = mdata->nature;
		param_values[3] = mdata->num;
		param_values[4] = mdata->nor;
		param_values[5] = mdata->num_parution;
		param_values[6] = mdata->num_sequence;
		param_values[7] = mdata->origine_publi;
		param_values[8] = mdata->page_deb_publi;
		param_values[9] = mdata->page_fin_publi;
		set_param(10, param_values, mdata->date_publi);
		set_param(11, param_values, mdata->date_texte);
		set_param(12, param_values, mdata->derniere_modification);
		param_values[13] = dbbuf1->buffer;  // toc
		param_values[14] = dbbuf2->buffer;  // versions
		compute_signature(md, param_values, 15,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->id, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->id, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->id, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->id,
							stag);
					}
					buffer_reset(dbbuf1);
					buffer_reset(dbbuf2);
					return 0;
				} else {
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->id, c_tag);
					create_updated_jorf_text(
						conn, mdata->id,
						mdata->cid,
						mdata->rid,
						mdata->nature,
						mdata->num,
						mdata->nor,
						mdata->num_parution,
						mdata->num_sequence,
						mdata->origine_publi,
						mdata->page_deb_publi,
						mdata->page_fin_publi,
						mdata->date_publi,
						mdata->date_texte,
						mdata->derniere_modification,
						dbbuf1->buffer,
						dbbuf2->buffer);
					to_create = 0;
				}
			}
		}

		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->id, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf3, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf3->buffer[dbbuf3->current_size] = 0;
			param_values[15] = mdata->id;
			param_values[16] = mdata->uri;
			param_values[17] = dbbuf3->buffer;  // uri_parts
			param_values[18] = stag;
			param_values[19] = signature;
			param_values[20] = NULL;

			tt->db_insert = clock();
			/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
			res = PQexecParams(conn,
					   "INSERT INTO raw_jorflegi.import_jorf_text"
					   " (cid, rid, nature, num, nor, num_parution, num_sequence,"
					   "  origine_publi, page_deb_publi, page_fin_publi, date_publi,"
					   "  date_texte, derniere_modification, toc, versions,"
					   "  id, uri, uri_parts, tag, sig, mod_tm)"
					   " VALUES"
					   " ($1, $2, $3, $4, $5, $6, $7,"
					   "  $8, $9, $10, $11,"
					   "  $12, $13, $14, $15,"
					   "  $16, $17, $18, $19, $20, NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   20,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf3);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->id, stag);
			param_values[15] = stag; /* tag */
			param_values[16] = stag; /* ltag */
			param_values[17] = signature; /* sig */
			param_values[18] = mdata->id;
			param_values[19] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_jorf_text"
					   " SET ("
					   "  cid, rid, nature, num, nor,"
					   "  num_parution, num_sequence,"
					   "  origine_publi, page_deb_publi, "
					   "  page_fin_publi,"
					   "  date_publi, date_texte, "
					   "  derniere_modification, "
					   "  toc, versions,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3, $4, $5,"
					   "  $6, $7,"
					   "  $8, $9, "
					   "  $10,"
					   "  $11, $12,"
					   "  $13,"
					   "  $14, $15,"
					   "  $16, $17, $18,"
					   "  NOW()::timestamp)"
					   " WHERE id = $19",
					   19,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}
		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		PQclear(res);
		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;

	case JORFCONT_DOCTYPE:
		r = write_toc_json(-1, toc, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;

		param_values[0] = mdata->titre;
		param_values[1] = mdata->num;
		param_values[2] = mdata->date_publi;
		param_values[3] = dbbuf1->buffer; // toc
		compute_signature(md, param_values, 4,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->id, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->id, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->id, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->id,
							stag);
					}
					buffer_reset(dbbuf1);
					return 0;
				} else {
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->id, c_tag);
					create_updated_jorf_cont(
						conn, mdata->id,
						mdata->titre,
						mdata->num,
						mdata->date_publi,
						dbbuf1->buffer);
					to_create = 0;
				}
			}
		}

		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->id, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf2, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf2->buffer[dbbuf2->current_size] = 0;
			param_values[4] = mdata->id;
			param_values[5] = mdata->uri;
			param_values[6] = dbbuf2->buffer; // uri_parts
			param_values[7] = stag;
			param_values[8] = signature;
			param_values[9] = NULL;

			tt->db_insert = clock();
			/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
			res = PQexecParams(conn,
					   "INSERT INTO raw_jorflegi.import_jorf_cont"
					   " (titre, num, date_publi, toc, id, uri, uri_parts, tag, sig, mod_tm)"
					   " VALUES"
					   " ($1, $2, $3, $4, "
					   "  $5, $6, $7, $8, $9, NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   9,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf2);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->id, stag);
			param_values[4] = stag; /* tag */
			param_values[5] = stag; /* ltag */
			param_values[6] = signature; /* sig */
			param_values[7] = mdata->id;
			param_values[8] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_jorf_cont"
					   " SET ("
					   "  titre, num, date_publi,"
					   "  toc,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3,"
					   "  $4,"
					   "  $5, $6, $7,"
					   "  NOW()::timestamp)"
					   " WHERE id = $8",
					   8,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}
		buffer_reset(dbbuf1);
		PQclear(res);
		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;

	case LEGIARTI_DOCTYPE:
		r = write_liens_json(-1, liens, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_contexte_json(-1, contexte, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;
		r = write_contenu_json(-1, contenu, dbbuf3, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf3->buffer[dbbuf3->current_size] = 0;
		r = write_versions_json(-1, versions, dbbuf4, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf4->buffer[dbbuf4->current_size] = 0;
		param_values[0] = mdata->id; // cid
		param_values[1] = mdata->rid;
		param_values[2] = mdata->num;
		param_values[3] = mdata->etat;
		set_param(4, param_values, mdata->date_debut);
		set_param(5, param_values, mdata->date_fin);
		param_values[6] = mdata->type;
		param_values[7] = dbbuf1->buffer;  // liens
		param_values[8] = dbbuf2->buffer;  // contexte
		param_values[9] = dbbuf3->buffer;  // contenu
		param_values[10] = dbbuf4->buffer;  // versions
		compute_signature(md, param_values, 11,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->id, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->id, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->id, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->id,
							stag);
					}
					buffer_reset(dbbuf1);
					buffer_reset(dbbuf2);
					buffer_reset(dbbuf3);
					buffer_reset(dbbuf4);
					return 0;
				} else {
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->id, c_tag);
					create_updated_legi_arti(
						conn, mdata->id,
						mdata->rid,
						mdata->num,
						mdata->etat,
						mdata->date_debut,
						mdata->date_fin,
						mdata->type,
						dbbuf1->buffer,
						dbbuf2->buffer,
						dbbuf3->buffer,
						dbbuf4->buffer);
					to_create = 0;
				}
			}
		}

		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->id, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf5, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf5->buffer[dbbuf5->current_size] = 0;
			param_values[11] = mdata->id;
			param_values[12] = mdata->uri;
			param_values[13] = dbbuf5->buffer;  // uri_parts
			param_values[14] = stag;
			param_values[15] = signature;
			param_values[16] = NULL;
			tt->db_insert = clock();
			/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
			res = PQexecParams(conn,
					   "INSERT INTO raw_jorflegi.import_legi_arti"
					   " (cid, rid, num, etat,"
					   "  date_debut, date_fin, type,"
					   "  liens, contexte, contenu, versions,"
					   "  id, uri, uri_parts, tag, sig, mod_tm)"
					   " VALUES"
					   " ($1, $2, $3,"
					   "  $4, $5, $6,"
					   "  $7, $8, $9, $10, $11,"
					   "  $12, $13, $14, $15, $16, NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   16,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf5);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->id, stag);
			param_values[11] = stag; /* tag */
			param_values[12] = stag; /* ltag */
			param_values[13] = signature; /* sig */
			param_values[14] = mdata->id;
			param_values[15] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_legi_arti"
					   " SET (cid, rid, num, etat,"
					   "  date_debut, date_fin, type,"
					   "  liens, contexte, contenu,"
					   "  versions,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3,"
					   "  $4, $5, $6,"
					   "  $7, $8, $9, $10,"
					   "  $11,"
					   "  $12, $13, $14, NOW()::timestamp)"
					   " WHERE id = $15",
					   15,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}

		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		buffer_reset(dbbuf3);
		buffer_reset(dbbuf4);
		PQclear(res);

		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;

	case LEGISCTA_DOCTYPE:
		r = write_contexte_json(-1, contexte, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_toc_json(-1, toc, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;
		param_values[0] = mdata->rid;
		param_values[1] = mdata->titrefull;
		param_values[2] = mdata->commentaire;
		param_values[3] = dbbuf1->buffer;  // contexte
		param_values[4] = dbbuf2->buffer;  // toc
		compute_signature(md, param_values, 5,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->id, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->id, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->id, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->id,
							stag);
					}
					buffer_reset(dbbuf1);
					buffer_reset(dbbuf2);
					return 0;
				} else {
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->id, c_tag);
					create_updated_legi_scta(
						conn, mdata->id,
						mdata->rid,
						mdata->titrefull,
						mdata->commentaire,
						dbbuf1->buffer,
						dbbuf2->buffer);
					to_create = 0;
				}
			}
		}

		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->id, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf3, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf3->buffer[dbbuf3->current_size] = 0;
			param_values[5] = mdata->id;
			param_values[6] = mdata->uri;
			param_values[7] = dbbuf3->buffer;  // uri_parts
			param_values[8] = stag;
			param_values[9] = signature;
			param_values[10] = NULL;
			tt->db_insert = clock();
			res = PQexecParams(conn,
					   "INSERT INTO"
					   " raw_jorflegi.import_legi_scta"
					   " (rid, titrefull, commentaire,"
					   "  contexte, toc,"
					   "  id, uri, uri_parts, tag, sig,"
					   "  mod_tm)"
					   " VALUES"
					   " ($1, $2, $3, $4, $5,"
					   "  $6, $7, $8, $9, $10,"
					   "  NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   10,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf3);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->id, stag);
			param_values[5] = stag; /* tag */
			param_values[6] = stag; /* ltag */
			param_values[7] = signature; /* sig */
			param_values[8] = mdata->id;
			param_values[9] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_legi_scta"
					   " SET ("
					   "  rid, titrefull, commentaire,"
					   "  contexte, toc,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3,"
					   "  $4, $5,"
					   "  $6, $7, $8,"
					   "  NOW()::timestamp)"
					   " WHERE id = $9",
					   9,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}

		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		PQclear(res);

		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;

	case LEGITEXT_DOCTYPE:
		r = write_toc_json(-1, toc, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_versions_json(-1, versions, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;

		param_values[0] = mdata->cid;
		param_values[1] = mdata->rid;
		param_values[2] = mdata->nature;
		param_values[3] = mdata->num;
		param_values[4] = mdata->nor;
		param_values[5] = mdata->num_parution;
		param_values[6] = mdata->num_sequence;
		param_values[7] = mdata->origine_publi;
		param_values[8] = mdata->page_deb_publi;
		param_values[9] = mdata->page_fin_publi;
		set_param(10, param_values, mdata->date_publi);
		set_param(11, param_values, mdata->date_texte);
		set_param(12, param_values, mdata->derniere_modification);
		param_values[13] = dbbuf1->buffer;  // toc
		param_values[14] = dbbuf2->buffer;  // versions
		compute_signature(md, param_values, 15,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->id, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->id, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->id, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->id,
							stag);
					}
					buffer_reset(dbbuf1);
					buffer_reset(dbbuf2);
					return 0;
				} else {
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->id, c_tag);
					create_updated_legi_text(
						conn, mdata->id,
						mdata->cid,
						mdata->rid,
						mdata->nature,
						mdata->num,
						mdata->nor,
						mdata->num_parution,
						mdata->num_sequence,
						mdata->origine_publi,
						mdata->page_deb_publi,
						mdata->page_fin_publi,
						mdata->date_publi,
						mdata->date_texte,
						mdata->derniere_modification,
						dbbuf1->buffer,
						dbbuf2->buffer);
					to_create = 0;
				}
			}
		}

		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->id, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf3, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf3->buffer[dbbuf3->current_size] = 0;
			param_values[15] = mdata->id;
			param_values[16] = mdata->uri;
			param_values[17] = dbbuf3->buffer;  // uri_parts
			param_values[18] = stag;
			param_values[19] = signature;
			param_values[20] = NULL;

			tt->db_insert = clock();
			/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
			res = PQexecParams(conn,
					   "INSERT INTO raw_jorflegi.import_legi_text"
					   " (cid, rid, nature, num, nor, num_parution, num_sequence,"
					   "  origine_publi, page_deb_publi, page_fin_publi, date_publi,"
					   "  date_texte, derniere_modification, toc, versions,"
					   "  id, uri, uri_parts, tag, sig, mod_tm)"
					   " VALUES"
					   " ($1, $2, $3, $4, $5, $6, $7,"
					   "  $8, $9, $10, $11,"
					   "  $12, $13, $14, $15,"
					   "  $16, $17, $18, $19, $20, NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   20,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf3);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->id, stag);
			param_values[15] = stag; /* tag */
			param_values[16] = stag; /* ltag */
			param_values[17] = signature; /* sig */
			param_values[18] = mdata->id;
			param_values[19] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_legi_text"
					   " SET ("
					   "  cid, rid, nature, num, nor,"
					   "  num_parution, num_sequence,"
					   "  origine_publi, page_deb_publi, "
					   "  page_fin_publi,"
					   "  date_publi, date_texte, "
					   "  derniere_modification, "
					   "  toc, versions,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3, $4, $5,"
					   "  $6, $7,"
					   "  $8, $9, "
					   "  $10,"
					   "  $11, $12,"
					   "  $13,"
					   "  $14, $15,"
					   "  $16, $17, $18,"
					   "  NOW()::timestamp)"
					   " WHERE id = $19",
					   19,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}
		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		PQclear(res);
		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;

	case LEGIVERS_DOCTYPE:
		r = write_liens_json(-1, liens, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_contexte_json(-1, contexte, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;
		r = write_contenu_json(-1, contenu, dbbuf3, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf3->buffer[dbbuf3->current_size] = 0;

		param_values[0] = mdata->cid;
		param_values[1] = mdata->id;
		param_values[2] = mdata->titre;
		param_values[3] = mdata->titrefull;
		param_values[4] = mdata->autorite;
		param_values[5] = mdata->ministere;
		param_values[6] = mdata->etat;
		set_param(7, param_values, mdata->date_debut);
		set_param(8, param_values, mdata->date_fin);
		param_values[9] = dbbuf1->buffer;  // liens
		param_values[10] = dbbuf2->buffer;  // contexte
		param_values[11] = dbbuf3->buffer;  // contenu
		// FIXME: entreprise
		compute_signature(md, param_values, 12,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);

		if (!bootstrap) {
			int exists = get_document_signature(
				conn,
				mdata->uri_parts.doctype,
				mdata->rid, c_signature, c_tag);
			if (exists) {
				if (strcmp(signature, c_signature) == 0) {
					/* Same content */
					if (strcmp(c_tag, stag) == 0) {
						/* Same tag */
						fprintf(log_file,
							"IGNORE:%s:%s\n",
							mdata->rid, stag);
					} else {
						fprintf(log_file,
							"IDENTICAL:%s:%s\n",
							mdata->rid, stag);
						update_ltag(
							conn,
							mdata->uri_parts.doctype,
							mdata->rid,
							stag);
					}
					buffer_reset(dbbuf1);
					buffer_reset(dbbuf2);
					buffer_reset(dbbuf3);
					return 0;
				} else {
					fprintf(log_file,
						"CREATE_UPDATED:%s:%s\n",
						mdata->rid, c_tag);
					create_updated_legi_vers(
						conn, mdata->rid,
						mdata->cid,
						mdata->id,
						mdata->titre,
						mdata->titrefull,
						mdata->autorite,
						mdata->ministere,
						mdata->etat,
						mdata->date_debut,
						mdata->date_fin,
						dbbuf1->buffer,
						dbbuf2->buffer,
						dbbuf3->buffer);
					to_create = 0;
				}
			}
		}
		if (to_create) {
			fprintf(log_file, "CREATE:%s:%s\n",
				mdata->rid, stag);
			r = write_uri_parts_json(-1, &mdata->uri_parts,
						 dbbuf4, 1);
			if (r < 0) exit_nicely(conn);
			dbbuf4->buffer[dbbuf4->current_size] = 0;
			param_values[12] = mdata->rid;
			param_values[13] = mdata->uri;
			param_values[14] = dbbuf4->buffer;  // uri_parts
			param_values[15] = stag;
			param_values[16] = signature;
			param_values[17] = NULL;
			tt->db_insert = clock();
			/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
			res = PQexecParams(conn,
					   "INSERT INTO raw_jorflegi.import_legi_vers"
					   " (cid, rid, titre, titrefull, autorite,"
					   "  ministere, etat,"
					   "  date_debut, date_fin, liens,"
					   "  contexte, contenu,"
					   "  id, uri, uri_parts, tag, sig, mod_tm)"
					   " VALUES"
					   " ($1, $2, $3, $4, $5,"
					   "  $6,"
					   "  $7, $8, $9, $10,"
					   "  $11, $12,"
					   "  $13, $14, $15, $16, $17, NOW()::timestamp)"
					   " ON CONFLICT DO NOTHING",
					   17,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "INSERT failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
			buffer_reset(dbbuf4);
		} else {
			fprintf(log_file, "UPDATE:%s:%s\n",
				mdata->rid, stag);
			param_values[12] = stag; /* tag */
			param_values[13] = stag; /* ltag */
			param_values[14] = signature; /* sig */
			param_values[15] = mdata->rid;
			param_values[16] = NULL;
			res = PQexecParams(conn,
					   "UPDATE"
					   " raw_jorflegi.import_legi_vers"
					   " SET ("
					   "  cid, rid, titre, titrefull, autorite,"
					   "  ministere, etat,"
					   "  date_debut, date_fin, liens,"
					   "  contexte, contenu,"
					   "  tag, ltag, sig, mod_tm)"
					   " = "
					   " ($1, $2, $3, $4, $5,"
					   "  $6, "
					   "  $7, $8, $9, $10,"
					   "  $11, $12,"
					   "  $13, $14, $15, NOW()::timestamp)"
					   " WHERE id = $16",
					   16,
					   NULL,
					   param_values,
					   NULL,
					   NULL,
					   0);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr,
					"UPDATE failed: %s\n",
					PQresultErrorMessage(res));
				PQclear(res);
				exit_nicely(conn);
			}
		}
		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		buffer_reset(dbbuf3);
		PQclear(res);
		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;
	default:
		break;
	}

	return 0;
}
