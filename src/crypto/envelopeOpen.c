#include <openssl/bn.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "error.h"

int32_t envelopeOpen(EVP_PKEY *pKey, unsigned char *cipherText, int cipherTextLength,
                     unsigned char *sessionKey, unsigned char *iv,
                     unsigned char *plainText) {
  EVP_CIPHER_CTX *ctx;

  int32_t len = 0;
  int32_t plaintextLength = 0;

  if(!(ctx = EVP_CIPHER_CTX_new())) errExit(30);

  if(EVP_OpenInit(ctx, EVP_aes_256_cbc(), sessionKey, 256, iv, pKey) == 0) 
    errExit(31);

  if(EVP_OpenUpdate(ctx, plainText, &len, cipherText, cipherTextLength) == 0)
    errExit(32);
  plaintextLength = len;

  if(EVP_OpenFinal(ctx, plainText + plaintextLength, &len) == 0) errExit(33);
  plaintextLength += len;

  EVP_CIPHER_CTX_free(ctx);
  return plaintextLength;
}
