#ifndef init

typedef struct {
  char *ipV4;
} CommandLineOptions;

void blockAllSignalsExceptSitTerm(void);
void getCommandLineOptions(int argc, char *argv[], CommandLineOptions *commandLineOptions);

#endif // !init
