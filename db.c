#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "db.h"
#include "sig.h"

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
	if (*val == 0)
		params[n] = NULL;
	else
		params[n] = val;
}

int db_import(PGconn *conn, const EVP_MD *md, struct tm *tag,
		struct parsed_data *pdata,
			struct write_buffer *dbbuf1,
		struct write_buffer *dbbuf2,
	      struct write_buffer *dbbuf3,
	      struct write_buffer *dbbuf4,
	      struct write_buffer *dbbuf5,
	      struct write_buffer *dbbuf6,
			struct timings *tt)
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
	unsigned char digest[EVP_MAX_MD_SIZE];
	int digest_len;

	sprintf(stag, "%04d-%02d-%02d %02d:%02d:%02d",
		tag->tm_year, tag->tm_mon, tag->tm_mday,
		tag->tm_hour, tag->tm_min, tag->tm_sec);

	switch (mdata->uri_parts.doctype) {
	case JORFARTI_DOCTYPE:
		/*fprintf(stderr, "INFO:%s: insert in DB\n",
			mdata->id);*/
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
		r = write_uri_parts_json(-1, &mdata->uri_parts,
					 dbbuf6, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf6->buffer[dbbuf6->current_size] = 0;
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
		param_values[11] = mdata->id;
		param_values[12] = mdata->uri;
		param_values[13] = dbbuf6->buffer;  // uri_parts
		param_values[14] = stag;
		param_values[15] = signature;
		param_values[16] = NULL;
		tt->db_insert = clock();
		/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
		res = PQexecParams(conn,
				   "INSERT INTO raw_jorflegi.import_jorf_arti"
				   " (cid, rid, num,"
				   "  date_debut, date_fin, type,"
				   "  mcs, liens, contexte, contenu, versions,"
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
			fprintf(stderr, "INSERT failed: %s\n", PQresultErrorMessage(res));
			PQclear(res);
			exit_nicely(conn);
		}
		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		buffer_reset(dbbuf3);
		buffer_reset(dbbuf4);
		buffer_reset(dbbuf5);
		buffer_reset(dbbuf6);
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
		r = write_uri_parts_json(-1, &mdata->uri_parts,
					 dbbuf3, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf3->buffer[dbbuf3->current_size] = 0;
		param_values[0] = mdata->rid;
		param_values[1] = mdata->titrefull;
		param_values[2] = mdata->commentaire;
		param_values[3] = dbbuf1->buffer;  // contexte
		param_values[4] = dbbuf2->buffer;  // toc
		compute_signature(md, param_values, 5,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);
		param_values[5] = mdata->id;
		param_values[6] = mdata->uri;
		param_values[7] = dbbuf3->buffer;  // uri_parts
		param_values[8] = stag;
		param_values[9] = signature;
		param_values[10] = NULL;
		tt->db_insert = clock();
		/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
		res = PQexecParams(conn,
				   "INSERT INTO raw_jorflegi.import_jorf_scta"
				   " (rid, titrefull, commentaire,"
				   "  contexte, toc,"
				   "  id, uri, uri_parts, tag, sig, mod_tm)"
				   " VALUES"
				   " ($1, $2, $3, $4, $5,"
				   "  $6, $7, $8, $9, $10, NOW()::timestamp)"
				   " ON CONFLICT DO NOTHING",
				   10,
				   NULL,
				   param_values,
				   NULL,
				   NULL,
				   0);
		if (PQresultStatus(res) != PGRES_COMMAND_OK) {
			fprintf(stderr, "INSERT failed: %s\n", PQresultErrorMessage(res));
			PQclear(res);
			exit_nicely(conn);
		}
		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		buffer_reset(dbbuf3);
		PQclear(res);
		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;
	case JORFVERS_DOCTYPE:
		/*fprintf(stderr, "INFO:%s: insert in DB\n",
			mdata->rid);*/
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
		r = write_uri_parts_json(-1, &mdata->uri_parts,
			 dbbuf5, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf5->buffer[dbbuf5->current_size] = 0;

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
		param_values[12] = mdata->rid;
		param_values[13] = mdata->uri;
		param_values[14] = dbbuf5->buffer;  // uri_parts
		param_values[15] = stag;
		param_values[16] = signature;
		param_values[17] = NULL;
		tt->db_insert = clock();
		/* https://linuxfr.org/users/n_e/journaux/upsert-dans-postgresql-ca-dechire */
		res = PQexecParams(conn,
				   "INSERT INTO raw_jorflegi.import_jorf_vers"
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
			fprintf(stderr, "INSERT failed: %s\n", PQresultErrorMessage(res));
			PQclear(res);
			exit_nicely(conn);
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
	case JORFTEXT_DOCTYPE:
		/*fprintf(stderr, "INFO:%s: insert in DB\n",
			mdata->id);*/
			/*
			 * mdata->num
			 * mdata->num_parution
			 * mdata->num_sequence
			 * mdata->nor
			 * mdata->date_publi (date)
			 * mdata->date_texte (date)
			 * mdata->derniere_modification (date)
			 * mdata->origine_publi
			 * mdata->page_deb_publi
			 * mdata->page_fin_publi
			 * toc (json)
			 * versions (json)
			 */
		r = write_toc_json(-1, toc, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;
		r = write_versions_json(-1, versions, dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;
		r = write_uri_parts_json(-1, &mdata->uri_parts,
					 dbbuf3, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf3->buffer[dbbuf3->current_size] = 0;

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
		param_values[10] = mdata->date_publi;
		set_param(11, param_values, mdata->date_texte);
		set_param(12, param_values, mdata->derniere_modification);
		param_values[13] = dbbuf1->buffer;  // toc
		param_values[14] = dbbuf2->buffer;  // versions

		compute_signature(md, param_values, 15,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);
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
			fprintf(stderr, "INSERT failed: %s\n", PQresultErrorMessage(res));
			PQclear(res);
			exit_nicely(conn);
		}
		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
		buffer_reset(dbbuf3);
		PQclear(res);
		tt->db_insert = clock() - tt->db_insert;
		tt->db_insert_tm = ((double) tt->db_insert) / CLOCKS_PER_SEC;
		return 1;
		break;
	case JORFCONT_DOCTYPE:
		/*
		fprintf(stderr, "INFO:%s: insert in DB\n",
			mdata->id);
		*/
		r = write_toc_json(-1, toc, dbbuf1, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf1->buffer[dbbuf1->current_size] = 0;

		r = write_uri_parts_json(-1, &mdata->uri_parts,
					 dbbuf2, 1);
		if (r < 0) exit_nicely(conn);
		dbbuf2->buffer[dbbuf2->current_size] = 0;

		param_values[0] = mdata->titre;
		param_values[1] = mdata->num;
		param_values[2] = mdata->date_publi;
		param_values[3] = dbbuf1->buffer; // toc
		compute_signature(md, param_values, 4,
				  digest, &digest_len, tt);
		hex_signature(digest, digest_len, signature);
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
			fprintf(stderr, "INSERT failed: %s\n", PQresultErrorMessage(res));
			PQclear(res);
			exit_nicely(conn);
		}
		buffer_reset(dbbuf1);
		buffer_reset(dbbuf2);
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