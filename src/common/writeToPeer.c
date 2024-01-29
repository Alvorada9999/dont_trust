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

void writeToPeer(AllMessages *allMessages, MessageCodesToBeSentBackQueue *messageCodesToBeSentBackAsConfirmationQueue, int8_t *fd) {
  static uint8_t writingStatus = WRITING_NOTHING;
  if(writingStatus == WRITING_NOTHING) {
    if(messageCodesToBeSentBackAsConfirmationQueue->size > 0) {
      writingStatus = WRITING_MESSAGES_CONFIRMATIONS;
      allMessages->socketOutputStatus.isThereAnythingBeingSent = true;
    } else {
      if(allMessages->messagesByCode.length > allMessages->messagesByCode.numberOfSentMessages) {
        writingStatus = WRITING_MESSAGES;
        allMessages->socketOutputStatus.isThereAnythingBeingSent = true;
      }
    }
  }

  switch (writingStatus) {
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
      static int8_t lastWriteSize = 0;
      do {
        lastWriteSize = write(*fd, dataToBeSent+writtenSize, 9-writtenSize);
        if(lastWriteSize != -1) writtenSize += lastWriteSize;
      } while (lastWriteSize != -1 && writtenSize < 9);

      if(lastWriteSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        allMessages->socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = false;
        break;
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
      static bool gotFirstSegment = false;
      static char firstSegmentBuffer[7] = { BEGIN_OF_MESSAGE_TRANSMISSION };

      static Message *messageToBeSent;

      if(gotFirstSegment == false) {
        messageToBeSent = allMessages->messagesByCode.array[allMessages->messagesByCode.numberOfSentMessages];
        gotFirstSegment = true;
        static uint32_t messageCodeInNetworkByteOrder = 0;
        messageCodeInNetworkByteOrder = allMessages->messagesByCode.numberOfSentMessages;
        messageCodeInNetworkByteOrder = htonl(messageCodeInNetworkByteOrder);
        memcpy(firstSegmentBuffer+1, &messageCodeInNetworkByteOrder, 4);
        static uint16_t messageSizeInNetworkByteOrder = 0;
        messageSizeInNetworkByteOrder = htons(messageToBeSent->size);
        memcpy(firstSegmentBuffer+5, &messageSizeInNetworkByteOrder, 2);
      }

      static int8_t lastWriteSize = 0;

      static uint8_t firstSegmentWrittenSize = 0;
      if(firstSegmentWrittenSize < 7) {
        do {
          lastWriteSize = write(*fd, firstSegmentBuffer+firstSegmentWrittenSize, 7-firstSegmentWrittenSize);
          if(lastWriteSize != -1) firstSegmentWrittenSize += lastWriteSize;
        } while (lastWriteSize != -1 && firstSegmentWrittenSize < 7);
        if(lastWriteSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
          allMessages->socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = false;
          break;
        }
      }

      static uint16_t messageWrittenSize = 0;
      if(messageWrittenSize < messageToBeSent->size) {
        do {
          lastWriteSize = write(*fd, messageToBeSent->string+messageWrittenSize, messageToBeSent->size-messageWrittenSize);
          if(lastWriteSize != -1) messageWrittenSize += lastWriteSize;
        } while (lastWriteSize != -1 && messageWrittenSize < messageToBeSent->size);
        if(lastWriteSize == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
          allMessages->socketOutputStatus.isThereAnySpaceOnTheSocketSendBuffer = false;
          break;
        }
      }

      //if it arrived here, then it means all of the data
      //was sent, that being the case, the values should be reset
      writingStatus = WRITING_NOTHING;
      gotFirstSegment = false;
      firstSegmentWrittenSize = 0;
      messageWrittenSize = 0;
      allMessages->socketOutputStatus.isThereAnythingBeingSent = false;

      allMessages->messagesByCode.numberOfSentMessages += 1;

      break;
    }

  }
}
