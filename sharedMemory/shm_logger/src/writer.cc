// Copyright 2022 Ana Boccanfuso
//

#include <shm_logger/inc/consumer.h>

#include <cstddef>
#include <iostream>


// Deletes kLogger memory (calls destructor)
void LoggerSigTermHandler(int sig);


logger::Consumer* log_writer;


int main() {

  // set SIGTERM signal handler to unlink shm
  ::signal(SIGTERM, LoggerSigTermHandler);

  // build Consumer and start writing to file
  log_writer = new logger::Consumer("project3", "signal");
  log_writer->Consume();

  return 0;
}


void LoggerSigTermHandler(int sig) {
  if (sig == SIGTERM || sig == SIGINT)
    delete log_writer;
}
