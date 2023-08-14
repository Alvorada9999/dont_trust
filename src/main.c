#include <unistd.h>

int main(int argc, char *argv[]) {
  write(STDOUT_FILENO, &"test", sizeof("test"));
  return 0;
}
