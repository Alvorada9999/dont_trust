#ifndef DTCOMMON
#define DTCOMMON

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

int renderMessages(AllMessages *allMessages);

#endif // !DTCOMMON
