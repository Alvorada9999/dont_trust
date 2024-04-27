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

#ifndef DT_CRYPTO

#include <openssl/evp.h>

//should be updated if MAX_MESSAGE_SIZE is changed
//shoud not be bigger than 2,147,483,647
//MAX_CIPHER_TEXT_LENGTH = (MAX_MESSAGE_SIZE+16)/16)*16
#define MAX_CIPHER_TEXT_LENGTH 65552
#define INITIALIZATION_VECTOR_LENGTH 16
#define SESSION_KEY_LENGTH 256

int32_t envelopeSeal(EVP_PKEY **pubKey, unsigned char *plainText, int32_t plainTextLength,
                 unsigned char *sessionKey, unsigned char *iv,
                 unsigned char *cipherText);

int32_t envelopeOpen(EVP_PKEY *pKey, unsigned char *cipherText, int32_t cipherTextLength,
                 unsigned char *sessionKey, unsigned char *iv,
                 unsigned char *plainText);

EVP_PKEY *createPrivateKeyFromFilePath(const char *filePath);
EVP_PKEY *createPublicKeyFromFilePath(const char *filePath);

#endif // !DT_CRYPTO
