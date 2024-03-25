#include <openssl/pem.h>

#include "error.h"

EVP_PKEY *createPublicKeyFromFilePath(const char *filePath) {
  FILE *fp = NULL;
  EVP_PKEY *pubKey = NULL;

  if ((fp = fopen(filePath, "r")) == NULL) {
    errExit(28);
  }

  if ((pubKey = PEM_read_PUBKEY(fp, NULL, NULL, NULL)) == NULL) {
    errExit(29);
  }

  fclose(fp);
  return pubKey;
}
