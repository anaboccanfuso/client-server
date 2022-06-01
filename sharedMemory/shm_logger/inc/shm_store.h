// Copyright 2022 Ana Boccanfuso
//
#ifndef SHM_LOGGER_INC_SHM_STORE_H_
#define SHM_LOGGER_INC_SHM_STORE_H_


#include <cstddef>  // size_t


struct SharedMemoryStore {
  size_t buffer_size;
  char buffer[4096-sizeof(size_t)];  // this will be extended by external code
};


#endif  // SHM_LOGGER_INC_SHM_STORE_H_
