#include <openssl/pem.h>

#include "error.h"

EVP_PKEY *createPrivateKeyFromFilePath(const char *filePath) {
  FILE *fp = NULL;
  EVP_PKEY *pKey = NULL;

  if ((fp = fopen(filePath, "r")) == NULL) {
    errExit(26);
  }

  if ((pKey = PEM_read_PrivateKey(fp, NULL, NULL, NULL)) == NULL) {
    errExit(27);
  }

  fclose(fp);
  return pKey;
}
