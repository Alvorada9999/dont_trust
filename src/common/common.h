#ifndef DTCOMMON
#define DTCOMMON

#include <stdint.h>
#define DEFAULT_MESSAGE_OUTPUT_SIZE 140000

struct Message {
  char *string;
  unsigned int size;
  struct Message *nextMessage;
};

typedef struct Message Message;

typedef struct {
  Message *firstMessage;
  Message *LastMessage;
  int size;
} MessageQueue;

typedef struct {
  unsigned int sizeInChars;
  Message *startingMessage;
  unsigned int startingMessageCharPosition;
  unsigned int numberOfMessages;
  Message *messagesInOrder[];
} AllMessages;


void EnqueueMessage(MessageQueue *messageQueue, Message *message);
void DequeueMessage(MessageQueue *messageQueue);

int8_t renderMessages(AllMessages *allMessages);

#endif // !DTCOMMON
