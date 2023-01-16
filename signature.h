#ifndef PDQ_SIGNATURE_H
#define PDQ_SIGNATURE_H
#include <openssl/evp.h>
#include "timings.h"
const EVP_MD *init_signature_system();
void cleanup_signature_system();
int compute_signature(const EVP_MD *md,
                      const char **values, int nb_values,
                      unsigned char *sig_value, int *sig_len,
                      struct timings *tt);
void hex_signature(unsigned char *sig_value, unsigned int sig_len, char *target);
#define SIG_HEX_MAX (EVP_MAX_MD_SIZE*2+1)
#endif //PDQ_DB_H
