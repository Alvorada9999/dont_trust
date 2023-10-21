#ifndef DTCOMMON
#define DTCOMMON

#include <stdint.h>
#include <stdbool.h>
#define DEFAULT_MESSAGE_OUTPUT_SIZE 140000

enum MessageStatus {
  PEER_NOT_READ = 1,
  PEER_READ = 2,
  RECEIVED = 3
};

struct Message {
  char *string;
  unsigned int size;
  struct Message *nextMessage;
  uint8_t status;
};

typedef struct Message Message;

typedef struct {
  Message *firstMessage;
  Message *LastMessage;
  int size;
} MessageQueue;

typedef struct {
  unsigned int sizeInChars;
  Message *currentStartingMessage;
  unsigned int currentStartingMessageCharPosition;
  unsigned int numberOfMessages;
  Message *messagesInOrder[];
} AllMessages;


void EnqueueMessage(MessageQueue *messageQueue, Message *message);
void DequeueMessage(MessageQueue *messageQueue);

int8_t renderMessages(AllMessages *allMessages);

#endif // !DTCOMMON
