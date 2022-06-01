// Copyright 2022 Ana Boccanfuso
//

#ifndef SHM_LOGGER_INC_PRODUCER_H_
#define SHM_LOGGER_INC_PRODUCER_H_

#include <shm_logger/inc/named_semaphore.h>
#include <shm_logger/inc/shm_store.h>
#include <pthread.h>

#include <sys/mman.h>  // POSIX shared memory map
#include <sys/unistd.h>  // UNIX standard header (sleep)

#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstddef>  // size_t

#include <iostream>
#include <string>
#include <vector>

using namespace std;


namespace logger {

class Producer {
 public:
  Producer(const char shm_name[], const char mutex_name[]);

  ~Producer();

  int Produce(const std::string& file_path, const std::string& search_str);


 private:
  const char* shm_name_;  // shared memory name
  SharedMemoryStore *store_;
  wrappers::NamedSemaphore shm_log_signal_;  // shared memory log mutex
  wrappers::NamedSemaphore shm_log_signal_2;
  wrappers::NamedSemaphore shm_log_signal_3;
};

}  // namespace logger


#endif  // SHM_LOGGER_INC_PRODUCER_H_
