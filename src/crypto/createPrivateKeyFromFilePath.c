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
