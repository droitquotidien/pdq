#include <string.h>
#include "sig.h"

const EVP_MD *init_signature_system()
{
	OpenSSL_add_all_digests();
	return EVP_sha256();
}

void cleanup_signature_system()
{
	EVP_cleanup();
}

int compute_signature(const EVP_MD *md,
                      const char **values, int nb_values,
                      unsigned char *sig_value, int *sig_len,
                      struct timings *tt)
{
	int i;
	EVP_MD_CTX *mctx;
	clock_t ct;

	tt->sig_comp = clock();
	mctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mctx, md, NULL);
	for (i = 0; i < nb_values; i++) {
		if (values[i] == NULL || *values[i] == 0)
			continue;
		EVP_DigestUpdate(mctx, values[i], strlen(values[i]));
	}
	EVP_DigestFinal_ex(mctx, sig_value, sig_len);
	EVP_MD_CTX_destroy(mctx);
	tt->sig_comp = clock() - tt->sig_comp;
	tt->sig_comp_tm = ((double) tt->sig_comp) / CLOCKS_PER_SEC;

	return 0;
}

void hex_signature(unsigned char *sig_value, unsigned int sig_len, char *target)
{
	int i;
	char *buf = target;

	for (i = 0; i < sig_len; i++) {
		sprintf(buf, "%02x", sig_value[i]);
		buf += 2;
	}
}