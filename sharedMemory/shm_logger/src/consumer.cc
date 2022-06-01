// Copyright 2022 Ana Boccanfuso
//

#include <shm_logger/inc/consumer.h>
#include <iostream>
#include <string>

using namespace std;

namespace logger {

Consumer::Consumer(const char* shm_name, const char* shm_log_signal_name)
    : shm_name_(shm_name),
      shm_log_signal_(shm_log_signal_name),
      shm_log_signal_2("two"),
      shm_log_signal_3("three") {

  // (1) SERVER STARTED
  cout << "SERVER STARTED" << endl;

  // create signal mux (unlocked by producer(s))
  shm_log_signal_.Create(0);
  shm_log_signal_.Open();

}


Consumer::~Consumer() {
  // delete named semaphore
  shm_log_signal_.Destroy();

  ::exit(errno);
}


void Consumer::Consume() {
  // write any logs to file
  string msg;
  while (true) {
    shm_log_signal_.Down();  // block until occupied signal
    // (2) receives file name and path from client
    clog << "CLIENT REQUEST RECEIVED" << endl;

    // (3) OPEN SHARED MEMORY
    int shm_fd = ::shm_open(shm_name_, O_RDWR, 0);
    if (shm_fd < 0) {
      std::cerr << ::strerror(errno) << std::endl;

      ::exit(errno);
    }
    // get copy of mapped mem
    store_ = static_cast<SharedMemoryStore*>(
      ::mmap(nullptr,
             sizeof(SharedMemoryStore),
             PROT_READ | PROT_WRITE,
             MAP_SHARED,
             shm_fd,
             0));
    if (store_ == MAP_FAILED) {
      cerr << ::strerror(errno) << std::endl;
      ::exit(errno);
    }

    clog << "\tMEMORY OPEN" << endl;

    // (4) Using the path ...
    string filePath = store_->buffer;

    // writes path name to terminal's STDLOG
    clog << "\tOPENING: " << filePath << endl;

    shm_log_signal_2.Open();
    shm_log_signal_3.Open();

    // opens and reads the file
    ifstream in(filePath.c_str());
    if (in.is_open()) {
       string line;
       while (getline(in, line)) {
         if (line == "")
            continue;
           // writes contents of the file to shared memory
         strncpy(store_->buffer, line.c_str(), store_->buffer_size);
         shm_log_signal_2.Up();
         shm_log_signal_3.Down();
      }
      string end = "\0";
      strncpy(store_->buffer, end.c_str(), store_->buffer_size);
      shm_log_signal_2.Up();
    }
    else {
      cerr << "\tINVALID FILE" << endl;
      // notify client
      string invalid = "INVALID FILE";
      strncpy(store_->buffer, invalid.c_str(), store_->buffer_size);
      shm_log_signal_2.Up();
      // exit(1);
    }
    // closes the file
    in.close();
    clog << "\tFILE CLOSED" << endl;

    // release copy of mapped mem
    int result = ::munmap(store_, sizeof(SharedMemoryStore));
    if (result < 0) {
      std::cerr << ::strerror(errno) << std::endl;

      ::exit(errno);
    }

    // memory is CLOSED
    clog << "\tMEMORY CLOSED" << endl;
  }
}


}  // namespace logger
