// This file is part of dont_trust.
// Copyright (C) 2024 Kenedy Henrique Bueno Silva

// dont_trust is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
