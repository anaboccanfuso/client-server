// Copyright 2022 Ana Boccanfuso
//

#include <shm_logger/inc/producer.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

using namespace std;

namespace logger {

vector<string> lines;
string searchString;
vector<string> found1;
vector<string> found2;
vector<string> found3;
vector<string> found4;

Producer::Producer(const char shm_name[], const char mutex_name[])
    : shm_name_(shm_name),
      shm_log_signal_(mutex_name),
      shm_log_signal_2("two"),
      shm_log_signal_3("three") {
  // (1) CREATE SHARED MEMORY
  // open shared memory and capture file descriptor
  int shm_fd = ::shm_open(shm_name_, O_CREAT | O_EXCL | O_RDWR, 0660);
  if (shm_fd < 0) {
    cerr << ::strerror(errno) << endl;
    ::exit(errno);
  }
  // set size of shared memory with file descriptor
  const size_t kBuffer_size = 4096 - sizeof(size_t);  // page - buffer_size size
  const size_t kSHM_size = sizeof(SharedMemoryStore) + kBuffer_size;
  if (::ftruncate(shm_fd, kSHM_size) < 0) {
    std::cerr << ::strerror(errno) << std::endl;
    ::exit(errno);
  }

  // get copy of mapped mem
  const int kProt = PROT_READ | PROT_WRITE;
  store_ = static_cast<SharedMemoryStore*>(
  ::mmap(nullptr, sizeof(SharedMemoryStore), kProt, MAP_SHARED, shm_fd, 0));

  if (store_ == MAP_FAILED) {
    cerr << ::strerror(errno) << endl;
    ::exit(errno);
  }

  // init memory map
  *store_ = {};
  store_->buffer_size = kBuffer_size;  // set store's buffer size

  // get shared memory log signal named semaphore
  //   created by and locked by consumer
  shm_log_signal_.Open();
}

Producer::~Producer() {
  // return copy of mapped mem, capture any error/exit code
  int exit_code = ::munmap(store_, sizeof(SharedMemoryStore));

  // alert for error in ::munmap
  if (exit_code < 0)
    std::cerr << ::strerror(errno) << std::endl;

  // delete shared memory map
  if (::shm_unlink(shm_name_) < 0)
    std::cerr << ::strerror(errno) << std::endl;

  // // delete named semaphore
  shm_log_signal_2.Destroy();
  shm_log_signal_3.Destroy();
  ::exit(errno);
}

void *findMatch(void *threadid) {
   long tid;
   tid = (long)threadid + 1;
   size_t begin;
   if (tid == 1) {
     for (size_t i = 0; i <= lines.size()/4; i++) {
       if(lines[i].find(searchString) != string::npos) {
         begin = lines[i].find_first_not_of(' ');
         found1.push_back(lines[i].substr(begin, lines.size()));
       }
     }
  }
  else if (tid == 2) {
    for (size_t i = lines.size()/4; i <= lines.size()/2; i++) {
      if(lines[i].find(searchString) != string::npos) {
        begin = lines[i].find_first_not_of(' ');
        found2.push_back(lines[i].substr(begin, lines.size()));
      }
    }
  }
  else if (tid == 3) {
      for (size_t i = lines.size()/2; i <= lines.size()*3/4; i++) {
        if(lines[i].find(searchString) != string::npos) {
          begin = lines[i].find_first_not_of(' ');
          found3.push_back(lines[i].substr(begin, lines.size()));
        }
      }
  }
  else if (tid == 4) {
    for (size_t i = lines.size()*3/4; i <= lines.size(); i++) {
      if(lines[i].find(searchString) != string::npos) {
        begin = lines[i].find_first_not_of(' ');
        found4.push_back(lines[i].substr(begin, lines.size()));
      }
    }
 }
 return threadid;
}

int Producer::Produce(const string& file_path, const string& search_str) {

  searchString = search_str;
  // (2) SEND FILE PATH TO SERVER
  // copy string msg into shared memory via strncpy ('n' for bounded copy)
  strncpy(store_->buffer, file_path.c_str(), store_->buffer_size);

  shm_log_signal_2.Create(0);
  shm_log_signal_2.Open();
  shm_log_signal_3.Create(0);
  shm_log_signal_3.Open();
  // signal consumer to write to shared memory
  shm_log_signal_.Up();

  // (3) COPIES CONTENTS OF FILE TO LOCAL STORAGE
  while(true) {
    shm_log_signal_2.Down();
    string fileLine = store_->buffer;
    if (fileLine.compare("\0") == 0) {
      break;
    }
    // INVALID FILE
    else if (fileLine.compare("INVALID FILE") == 0) {
      cerr << "INVALID FILE" << endl;
      exit(1);
    }
    lines.push_back(fileLine);
    shm_log_signal_3.Up();
  }

  // (4) CREATES FOUR THREADS AND CALLS FUNCTION TO PROCESS 1/4 LINES
  pthread_t threads[4];

  for (int i = 0; i < 4; i++) {
    pthread_create(&threads[i], NULL, &findMatch, (void *)(intptr_t)i);
  }

  for (int i = 0; i < 4; i++) {
    pthread_join(threads[i], NULL);
  }

  // (5) WRITE ALL LINES OF TEXT FOUND TO STDOUT
  int count = 1;
  for (string line : found1) {
    cout << count << "\t" << line << endl;
    count++;
  }
  for (string line : found2) {
    cout << count << "\t" << line << endl;
    count++;
  }
  for (string line : found3) {
    cout << count << "\t" << line << endl;
    count++;
  }
  for (string line : found4) {
    cout << count << "\t" << line << endl;
    count++;
  }
  // release copy of mapped mem
  int result = ::munmap(store_, sizeof(SharedMemoryStore));
  if (result < 0) {
    std::cerr << ::strerror(errno) << std::endl;

    return errno;
  }
  return 0;
}

}  // namespace logger
