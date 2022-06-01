// Copyright 2022 Ana Boccanfuso
//

#include <shm_logger/inc/producer.h>

#include <cstddef>
#include <iostream>
#include <string>

using namespace std;

// Deletes kLogger memory (calls destructor)
void LoggerSigTermHandler(int sig);

logger::Producer log_writer("project3", "signal");

int main(int argc, char* argv[]) {
  assert(argc == 3 && "producer <file_path> <search_str>");

  // set SIGTERM signal handler to unlink shm
  ::signal(SIGTERM, LoggerSigTermHandler);

  log_writer.Produce(string(argv[1]), string(argv[2]));

  // (7) TERMINATES BY RETURNING 0
  return 0;
}

void LoggerSigTermHandler(int sig) {
  if (sig == SIGTERM || sig == SIGINT)
    // (6) DESTROYS SHARED MEMORY LOCATION
    log_writer.~Producer();
}
