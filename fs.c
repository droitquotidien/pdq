/* Filesystem backend: création de fichiers avec les données juridiques
 * sur un filesystem
 */
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include "parse.h"
#include "fs.h"
#include "html.h"
#include "json.h"

/* Returns -1 in case of error, 0 if it creates the directory, 1 if the directory exists.
 */
int create_dir(const char *dirname)
{
	int r = 0;

	r = mkdir(dirname, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
	if (r == -1) {
		if (errno == EEXIST) {
			return 1;
		}
		perror("ERROR: cannot create directory");
		fprintf(stderr, "ERROR: '%s' not created\n", dirname);
		return -1;
	}

	return r;
}

/*
 * with rootname='/a/b/c' and cid = 'JORFARTI000045175220', creates:
 * /a/b/c/JORF/ARTI/0000/4517/5220 folders.
 */
int create_cid_dirs(const char *rootname, const char *cid, char *pathbuf)
{
	int r = 0;
	char *loc;
	const char *cidpos;
	int i;

	/*
	 * - pathbuf contient une zone mémoire permettant de contenir les chemins *complets*
	 *
	 */
	loc = stpcpy(pathbuf, rootname);
	for (i = 0, cidpos = cid; i < 5; i++, cidpos += 4) {
		*loc++ = '/';
		loc = stpncpy(loc, cidpos, 4);
		*loc = 0;
		r = create_dir(pathbuf);
		if (r == 0) fprintf(stderr, "dir: %s\n", pathbuf);
		if (r == -1) return -1;
	}

	return 0;
}

int create_file(const char *fname)
{
	int f;

	f = open(fname, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR);
	if (f < 0) {
		if (errno == EEXIST) {
			return -2;
		} else {
			perror("ERROR: creating file");
			fprintf(stderr, "ERROR: cannot create file '%s'\n", fname);
			return -1;
		}
	}

	return f;
}

ssize_t write_fs(struct fs_backend *fs, struct parsed_data *pdata, struct write_buffer *wbuf, char force)
{
	ssize_t r = 0;
	int i;
	struct metadata *mdata = pdata->metadata;
	struct versions *versions = pdata->versions;
	struct liens *liens = pdata->liens;
	struct toc *toc = pdata->toc;
	struct contenu *contenu = pdata->contenu;
	struct mcs *mcs = pdata->mcs;
	struct entreprise *entreprise = pdata->entreprise;
	struct document_version *docversion;
	struct lien *lien;
	struct tocitem *tocitem;
	char *buf;
	char *rid;
	char *loc;
	int f;

	/*
	 * Création d'un répertoire .tmp
	 * Article: création dans .tmp
	 * Section: création dans
	 */

	/* Create cid directory '/tmp/JORF/TEXT/0000/0045/4567' */
	rid = mdata->rid;
	assert(*rid != '\0');
	r = create_cid_dirs(fs->rootdir, rid, fs->pathbuf);
	if (r == -1) {
		return -1;
	}

	/* Create JSON file for each resource */
	loc = fs->pathbuf + strlen(fs->pathbuf);
	*loc++ = '/';
	loc = stpcpy(loc, mdata->rid);
	strcpy(loc, ".json");
	f = create_file(fs->pathbuf);
	if (f == -1) {
		return -1;
	} else if (f == -2 && !force) {
		/* file exists */
		return 0;
	}

	r = write_json(pdata, f, wbuf);
	if (r < 0) {
		close(f);
		return -1;
	}
	r = buffer_flush(f, wbuf);
	if (r < 0) {
		close(f);
		return -1;
	}
	close(f);
	printf("%s\n", fs->pathbuf);

	/*
	if (*contenu->notice.text != 0) {
		fprintf(f, "\tcontenu.notice;\"\"\"%s\"\"\"\n", contenu->notice.text);
	}
	*/
	/*
	if (*contenu->visas.text != 0) {
		fprintf(f, "\tcontenu.visas;\"\"\"%s\"\"\"\n", contenu->visas.text);
	}
	*/
	/*
	if (*contenu->signataires != 0) {
		fprintf(f, "\tcontenu.signataires;\"\"\"%s\"\"\"\n", contenu->signataires);
	}
	 */
	/*
	if (*contenu->nota != 0) {
		fprintf(f, "\tcontenu.nota;\"\"\"%s\"\"\"\n", contenu->nota);
	}*/
	/*
	for (i = 0; i < versions->nb_versions; i++) {
		docversion = &versions->versions[i];
		fprintf(f, "\tversions.version;%s;%s;%s;%s;%s\n",
			docversion->id, docversion->date_debut, docversion->date_fin,
			docversion->etat, docversion->num);
	}
	for (i = 0; i < liens->nb_liens; i++) {
		lien = &liens->liens[i];
		fprintf(f, "\tliens.lien;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
			lien->cid_texte, lien->date_signature_texte, lien->nature_texte,
			lien->nor_texte, lien->num_texte,
			lien->id, lien->num, lien->sens, lien->typelien, lien->titre);
	}
	 */
	/*
	for (i = 0; i < toc->nb_tocitems; i++) {
		tocitem = &toc->tocitems[i];
		fprintf(f, "\ttoc.tocitem;%d;%d;%s;%s;%s;%s;%s;%s\n",
			tocitem->niv, tocitem->kind, tocitem->id, tocitem->cid,
			tocitem->date_debut, tocitem->date_fin,
			tocitem->etat, tocitem->titrefull);
	}
	*/
	/*
	for (i = 0; i < mcs->nb_mcs; i++) {
		buf = mcs->mc[i];
		fprintf(f, "\tmc;%s\n", buf);
	}
	 */
	/*
	if (*entreprise->texte_entreprise != 0 && strcmp(entreprise->texte_entreprise, "oui") == 0) {
		fprintf(f, "\tentreprise.texte_entreprise;%s\n", entreprise->texte_entreprise);
		for (i = 0; i < entreprise->nb_dates_effet; i++) {
			buf = entreprise->dates_effet[i];
			fprintf(f, "\tentreprise.date_effet;%s\n", buf);
		}
		for (i = 0; i < entreprise->nb_domaines; i++) {
			buf = entreprise->domaines[i];
			fprintf(f, "\tentreprise.domaine;%s\n", buf);
		}
	}
	 */
	return r;
}
