#ifndef init

typedef struct {
  char *ipV4;
} CommandLineOptions;

void blockAllSignalsExceptSigTerm(void);
void getCommandLineOptions(int argc, char *argv[], CommandLineOptions *commandLineOptions);

#endif // !init
