#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "error.h"

int32_t envelopeSeal(EVP_PKEY **pubKey, unsigned char *plainText, int32_t plainTextLength,
                     unsigned char *sessionKey, unsigned char *iv,
                     unsigned char *cipherText) {
  EVP_CIPHER_CTX *ctx;

  int32_t cipherTextLength;
  int32_t len;

  if(!(ctx = EVP_CIPHER_CTX_new())) errExit(34);

  int32_t k;
  if(EVP_SealInit(ctx, EVP_aes_256_cbc(), &sessionKey, &k, iv, pubKey, 1) == 0) errExit(35);

  if(EVP_SealUpdate(ctx, cipherText, &len, plainText, plainTextLength) == 0)
    errExit(36);
  cipherTextLength = len;

  if(EVP_SealFinal(ctx, cipherText + cipherTextLength, &len) == 0) errExit(37);
  cipherTextLength += len;

  EVP_CIPHER_CTX_free(ctx);
  return cipherTextLength;
}
