// Copyright 2022 CSCE 311
//

#include <pg_cache_logger/inc/consumer.h>
#include <string>

using namespace std;

namespace logger {

Consumer::Consumer() : log_sig_(log_sig_name_), sig2(sig2_name) {
  // create signal mux (unlocked by producer(s))
  log_sig_.Create(0);
  log_sig_.Open();

  // (1) SERVER STARTED
  cout << "SERVER STARTED" << endl;
}


Consumer::~Consumer() {
  // delete named semaphore
  log_sig_.Destroy();

  ::exit(0);
}


std::tuple<int, off_t> Consumer::OpenFile(const char file_name[], int flags) {
  // open and capture file descriptor
  int fd = ::open(file_name, flags);
  if (fd < 0) {
    std::cerr << "\tINVALID FILE" << std::endl;
    // allows client to see that file cannot be opened
    sig2.Open();
    sig2.Up();
    log_sig_.Destroy();
    ::exit(errno);
  }

  // get size of file
  struct ::stat f_stats;
  if (::fstat(fd, &f_stats) < 0)
    HandleError("OpenFile");

  return {fd, f_stats.st_size};
}


void Consumer::HandleError(const char msg[]) {
  std::cerr << "Source: " << msg << std::endl;
  std::cerr << '\t' << ::strerror(errno) << std::endl;
  log_sig_.Destroy();
  ::exit(errno);
}


void Consumer::Consume() {
  // (3) RESUME WAITING FOR FUTURE CLIENT CONTACT
  while (true) {
    log_sig_.Down();  // block until client ready

    // (2A) WRITES CLIENT REQUEST RECEIVED
    clog << "CLIENT REQUEST RECEIVED" << endl;

    // open transfer file and get size
    int buf_fd;
    long int buf_size;
    std::tie(buf_fd, buf_size) = OpenFile(buf_file_name_, O_RDWR);

    // map to transfer file pages in page cache
    char *buf_file_addr = static_cast<char *>(::mmap(nullptr, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, buf_fd, 0));
    if (buf_file_addr == MAP_FAILED)
      HandleError("Transfer file map");
    if (::close(buf_fd) < 0)
      HandleError("Transfer file close");

    string log_file_name = "";

    // get file name from transfer file
    for (long int i = 0; i < buf_size; i++) {
      if (buf_file_addr[i] != '\n')
        log_file_name += buf_file_addr[i];
    }

    if (::truncate(buf_file_name_, 0) < 0)
      HandleError("Emptying transfer file");

    // (2B) WRITES OPENING FILE AND FILE PATH TO STDLOG
    clog << "\tOPENING: " << log_file_name << endl;

    // (2B) OPENS THE FILE TO RECEIVE A FILE DESCRIPTOR
    // open log file and get size
    int log_fd;
    long int log_size;  // off_t is a long int
    std::tie(log_fd, log_size) = OpenFile(log_file_name.c_str(), O_RDWR);

    // (2B) USES FILE DESCRIPTOR TO MAP THE FILE TO SHARED MEMORY
    // map to log file pages in page cache and allocate additional pages
    char *log_file_addr = static_cast<char *>(::mmap(nullptr,
                                                     log_size,
                                                     PROT_READ | PROT_WRITE,
                                                     MAP_SHARED,
                                                     log_fd,
                                                     0));
    if (log_file_addr == MAP_FAILED)
      HandleError("Log file map");
    // (2B) AFTER MAPPING, WRITES TO STDLOG
    clog << "\tFILE MAPPED TO SHARED MEMORY" << endl;
    // (2B) CLOSES THE FILE USING FILE DESCRIPTOR
    if (::close(log_fd) < 0)
      HandleError("Log file map close");
    // (2B) WRITES TO STDLOG
    clog << "\tFILE CLOSED" << endl;

    sig2.Open();
    sig2.Up();

    // release copy of mapped mem
    if (::munmap(buf_file_addr, buf_size))
      HandleError("Buffer file unmap");
    if (::munmap(log_file_addr, log_size))
      HandleError("Log file unmap");
  }
}


}  // namespace logger
