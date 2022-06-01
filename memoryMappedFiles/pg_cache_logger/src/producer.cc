// Copyright 2022 CSCE 311
//

#include <pg_cache_logger/inc/producer.h>
#include <string>

using namespace std;

char *log_file_addr;
long int log_size;

namespace logger {

Producer::Producer() : log_sig_(log_sig_name_), sig2(sig2_name) {
  // get shared memory log signal named semaphore
  //   created by and locked by consumer
  log_sig_.Open();
}

namespace {

void HandleError(const char msg[]) {
  std::cerr << "Source: " << msg << std::endl;
  std::cerr << '\t' << ::strerror(errno) << std::endl;
  ::exit(errno);
}


std::tuple<int, off_t> OpenFile(const char file_name[], int flags) {
  // open and capture file descriptor
  int fd = ::open(file_name, flags);
  if (fd < 0) {
    // (3) WRITES INVALID FILE IF UNABLE TO OPEN FILE
    std::cerr << "INVALID FILE" << std::endl;
    ::exit(1);
  }

  // get size of file
  struct ::stat f_stats;
  if (::fstat(fd, &f_stats) < 0)
    HandleError("Open File");

  return {fd, f_stats.st_size};
}

}  // anonymous namespace

void *upperCase(void *threadid) {
  long tid;
  tid = (long)threadid + 1;
  if (tid == 1) {
    for (int i = 0; i <= log_size/4; i++) {
      if (int(log_file_addr[i]) >= 97 && int(log_file_addr[i]) <= 122) {
        log_file_addr[i] = log_file_addr[i] - 32;
      }
    }
  }
  else if (tid == 2) {
    for (int i = log_size/4; i <= log_size/2; i++) {
      if (int(log_file_addr[i]) >= 97 && int(log_file_addr[i]) <= 122) {
        log_file_addr[i] = log_file_addr[i] - 32;
      }
    }
  }
  else if (tid == 3) {
    for (int i = log_size/2; i <= (3*log_size/4); i++) {
      if (int(log_file_addr[i]) >= 97 && int(log_file_addr[i]) <= 122) {
        log_file_addr[i] = log_file_addr[i] - 32;
      }
    }
  }
  else if (tid == 4) {
    for (int i = (3*log_size/4); i <= log_size; i++) {
      if (int(log_file_addr[i]) >= 97 && int(log_file_addr[i]) <= 122) {
        log_file_addr[i] = log_file_addr[i] - 32;
      }
    }
  }
  return threadid;
}

int Producer::Produce(const std::string& msg) {
  // (1) PASSES FILE NAME AND PATH TO SERVER VIA TRANSFER FILE
  // open buffer file and get stats
  int buf_fd;
  long int buf_size;
  std::tie(buf_fd, buf_size) = OpenFile(buf_file_name_, O_RDWR);

  // assure file empty and get map to page cache
  if (buf_size > 0) {
    std::cerr << "Transfer file is not empty" << std::endl;
    ::exit(1);
  }

  // make file big enough
  if (static_cast<unsigned long int>(buf_size) < msg.length() + 1)
    if (::fallocate(buf_fd, 0, 0, msg.length() + 1) < 0)  // need to add \n
      HandleError("fallocate");
  char *buf_file_addr = static_cast<char *>(::mmap(nullptr,
                                                   msg.length() + 1,
                                                   PROT_READ | PROT_WRITE,
                                                   MAP_SHARED,
                                                   buf_fd,
                                                   0));
  if (buf_file_addr == MAP_FAILED)
    HandleError("Buffer file map");
  if (::close(buf_fd) < 0)
    HandleError("Buffer file close");

  // copy string msg into memory mapped file in page cache
  for (size_t i = 0; i < msg.length(); ++i)
    buf_file_addr[i] = msg[i];
  buf_file_addr[msg.length()] = '\n';

  // update transfer file
  if (msync(buf_file_addr, buf_size, MS_SYNC) < 0)
    HandleError("Synchronizing transfer file map");

  // signal consumer
  log_sig_.Up();

  // release copy of mapped mem
  if (::munmap(buf_file_addr, msg.length() + 1))
    HandleError("Buffer file unmap");

  sig2.Create(0);
  sig2.Open();
  sig2.Down();

  string log_file_name = "";
  for (size_t i = 0; i < msg.length(); i++) {
    if (msg[i] != '\n') {
      log_file_name += msg[i];
    }
  }
  // open log file and get size
  int log_fd;
  std::tie(log_fd, log_size) = OpenFile(log_file_name.c_str(), O_RDWR);

  // map to log file pages in page cache and allocate additional pages
  log_file_addr = static_cast<char *>(::mmap(nullptr,
                                              log_size,
                                              PROT_READ | PROT_WRITE,
                                              MAP_SHARED,
                                              log_fd,
                                              0));

  // (2) CREATES FOUR THREADS AND CALLS FUNCTION TO PROCESS 1/4 LINES
  pthread_t threads[4];

  for (int i = 0; i < 4; i ++) {
    pthread_create(&threads[i], NULL, &upperCase, (void *)(intptr_t)i);
  }

  for (int i = 0; i < 4; i++) {
    pthread_join(threads[i], NULL);
  }

  if (msync(log_file_addr, log_size, MS_SYNC) < 0)
    HandleError("Synchronizing log file map");

  if (log_file_addr == MAP_FAILED)
    HandleError("Log file map");
  if (::close(log_fd) < 0)
    HandleError("Log file map close");


  // release copy of mapped mem
  if (::munmap(log_file_addr, log_size))
    HandleError("Log file unmap");
  sig2.Destroy();

  // (4) TERMINATES BY RETURNING ZERO
  return 0;
}

}  // namespace logger
