// This file is part of dont_trust.
// Copyright (C) 2023 Kenedy Henrique Bueno Silva

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

#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <arpa/inet.h>

#include "common.h"
#include "dt_crypto.h"
#include "net.h"
#include "terminal.h"

void writeToPeer(AllMessages *allMessages, MessageCodesToBeSentBackQueue *messageCodesToBeSentBackAsConfirmationQueue, int8_t *fd, EVP_PKEY *pubKey) {

  while(messageCodesToBeSentBackAsConfirmationQueue->size > 0 || allMessages->messagesByCode.length > allMessages->messagesByCode.numberOfSentMessages) {

    static uint8_t writingStatus = WRITING_NOTHING;
    switch (writingStatus) {
      case WRITING_NOTHING:
        if(messageCodesToBeSentBackAsConfirmationQueue->size > 0) {
          writingStatus = WRITING_MESSAGES_CONFIRMATIONS;
          allMessages->socketOutputStatus.isThereAnythingBeingSent = true;
        } else {
          if(allMessages->messagesByCode.length > allMessages->messagesByCode.numberOfSentMessages) {
            writingStatus = WRITING_MESSAGES;
            allMessages->socketOutputStatus.isThereAnythingBeingSent = true;
          }
        }
        break;
      case WRITING_MESSAGES_CONFIRMATIONS: {
        bool startedSending = false;
        static char dataToBeSent[9] = { BEGIN_OF_MESSAGES_CONFIRMATIONS };

        if(!startedSending) {
          startedSending = true;
          //first byte = protocol code
          //2-5 bytes = number of confirmation codes (Always one in this version of the code)
          //6-9 bytes = confirmation code as unsigned 32 bit number in network byte order
          static uint32_t numberOfConfirmationCodesToSend;
          numberOfConfirmationCodesToSend = htonl(1);
          memcpy(dataToBeSent+1, &numberOfConfirmationCodesToSend, sizeof(uint32_t));

          static uint32_t messageCodeInNetworkByteOrder = 0;
          messageCodeInNetworkByteOrder = dequeueMessageCode(messageCodesToBeSentBackAsConfirmationQueue);
          memcpy(dataToBeSent+5, &messageCodeInNetworkByteOrder, sizeof(uint32_t));
        }

        static uint8_t writtenSize = 0;
        static int32_t lastWriteSize = 0;
        do {
          lastWriteSize = write(*fd, dataToBeSent+writtenSize, 9-writtenSize);
          if(lastWriteSize != -1) writtenSize += lastWriteSize;
        } while (lastWriteSize != -1 && writtenSize < 9);

        if(lastWriteSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
          allMessages->socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = false;
          return;
        }

        //if it arrived here, then it means all of the data
        //was sent, that being the case, the values should be reset
        writingStatus = WRITING_NOTHING;
        startedSending = false;
        writtenSize = 0;
        lastWriteSize = 0;
        allMessages->socketOutputStatus.isThereAnythingBeingSent = false;

        break;
      }
      case WRITING_MESSAGES: {
        static uint8_t buffer[TCP_STREAM_COMMAND_INFO_LENGTH+
          TCP_STREAM_MESSAGE_CODE_INFO_LENGTH+
          TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH+
          INITIALIZATION_VECTOR_LENGTH+
          SESSION_KEY_LENGTH+
          MAX_CIPHER_TEXT_LENGTH] = { BEGIN_OF_MESSAGE_TRANSMISSION };
        static const uint16_t messageCodeInfoIndex = TCP_STREAM_COMMAND_INFO_LENGTH;
        static const uint16_t cipherTextSizeInfoIndex = messageCodeInfoIndex + TCP_STREAM_MESSAGE_CODE_INFO_LENGTH/8;
        static const uint16_t initializationVectorIndex = cipherTextSizeInfoIndex + TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH/8;
        static const uint16_t sessionKeyIndex = initializationVectorIndex + INITIALIZATION_VECTOR_LENGTH;
        static const uint16_t cipherTextIndex = sessionKeyIndex + SESSION_KEY_LENGTH;

        //1+32+32+16+256
        //1+4+4+2+32
        //43
        static int32_t numberOfBytesToSend = TCP_STREAM_COMMAND_INFO_LENGTH+
          TCP_STREAM_MESSAGE_CODE_INFO_LENGTH/8+
          TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH/8+
          INITIALIZATION_VECTOR_LENGTH+
          SESSION_KEY_LENGTH;
        static bool startedWriting = false;
        if(!startedWriting) {
          startedWriting = true;
          static Message *messageToBeSent;
          messageToBeSent = allMessages->messagesByCode.array[allMessages->messagesByCode.numberOfSentMessages];

          static uint32_t messageCodeInNetworkByteOrder = 0;
          messageCodeInNetworkByteOrder = htonl(allMessages->messagesByCode.numberOfSentMessages);
          memcpy(buffer+messageCodeInfoIndex, &messageCodeInNetworkByteOrder, TCP_STREAM_MESSAGE_CODE_INFO_LENGTH/8);

          static uint32_t cipherTextSizeInNetworkByteOrder = 0;
          cipherTextSizeInNetworkByteOrder = (uint32_t)envelopeSeal(&pubKey, (unsigned char *)messageToBeSent -> string, (int32_t)messageToBeSent -> size, buffer+sessionKeyIndex, buffer+initializationVectorIndex, buffer+cipherTextIndex);
          numberOfBytesToSend += (int32_t)cipherTextSizeInNetworkByteOrder;
          cipherTextSizeInNetworkByteOrder = htonl(cipherTextSizeInNetworkByteOrder);
          memcpy(buffer+cipherTextSizeInfoIndex, &cipherTextSizeInNetworkByteOrder, TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH/8);
        }

        static int32_t dataWrittenSize = 0;
        static int32_t lastWriteSize;
        if(dataWrittenSize < numberOfBytesToSend) {
          do {
            lastWriteSize = write(*fd, buffer+dataWrittenSize, (uint32_t)(numberOfBytesToSend-dataWrittenSize));
            if(lastWriteSize != -1) dataWrittenSize += lastWriteSize;
          } while (lastWriteSize != -1 && dataWrittenSize < numberOfBytesToSend);
          if(lastWriteSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            allMessages->socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = false;
            return;
          }
        }

        //if it arrived here, then it means all of the data
        //was sent, that being the case, the values should be reset
        writingStatus = WRITING_NOTHING;
        numberOfBytesToSend = TCP_STREAM_COMMAND_INFO_LENGTH+
          TCP_STREAM_MESSAGE_CODE_INFO_LENGTH/8+
          TCP_STREAM_CIPHER_TEXT_SIZE_INFO_LENGTH/8+
          INITIALIZATION_VECTOR_LENGTH+
          SESSION_KEY_LENGTH;
        startedWriting = false;
        dataWrittenSize = 0;
        allMessages->socketOutputStatus.isThereAnythingBeingSent = false;

        allMessages->messagesByCode.numberOfSentMessages += 1;

        renderStatus(MESSAGE_SENT, &allMessages->winSize);

        break;
      }

    }
  }

}
