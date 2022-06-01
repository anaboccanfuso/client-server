// Copyright 2022 Ana Boccanfuso
//
#include "socket.h"
#include <sys/socket.h>  // Unix header for sockets, using socket
#include <sys/un.h>  // defns for Unix domain sockets, using struct sockaddr_un
#include <unistd.h>  // Unix standard header, using close

#include <cassert>  // using assert
#include <cerrno>  // using errno
#include <cstddef>  // using size_t
#include <cstdlib>  // exit
#include <cstring>  // using strncpy, strerror

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

UnixDomainSocket::UnixDomainSocket(const char *socket_path) {
  socket_path_ = string(socket_path);  // std::string manages char *

  sock_addr_ = {};  // init struct (replaces memset)
  sock_addr_.sun_family = AF_UNIX;  // set to Unix domain socket (e.g. instead
                                      //   of internet domain socket)
    // leaving leading null char sets abstract socket
  strncpy(sock_addr_.sun_path + 1,  // use strncpy to limit copy for
          socket_path,              //   portability
          sizeof(sock_addr_.sun_path) - 2);  // -2 for leading/trailing \0s
}

// Domain Socket Client C++ Interface Class
//
class DomainSocketClient : public UnixDomainSocket {
 public:
  using UnixDomainSocket::UnixDomainSocket;

  void RunClient(const char *filename, const char *searchstr) {
    // (1) open nameless Unix socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {
      cerr << strerror(errno) << endl;
      exit(-1);
    }

    // (2) connect to an existing socket
    int success = connect(socket_fd,
                          // sockaddr_un is a Unix sockaddr
                          reinterpret_cast<const sockaddr*>(&sock_addr_),
                          sizeof(sock_addr_));
    if (success < 0) {
      cerr << strerror(errno) << endl;
      exit(-1);
    }
    clog << "SERVER CONNECTION ACCEPTED" << endl;

    //  (3) write to socket
    string file = filename;
    string search = searchstr;
    string fileSearch = file + "+" + search;
    // ssize_t kWrite_buffer_size = file.size();
    // char write_buffer[kWrite_buffer_size];
    send(socket_fd, fileSearch.c_str(), strlen(fileSearch.c_str()), 0);

    // (4) Receive data from Server
    const size_t kRead_buffer_size = 10000;
    char read_buffer[kRead_buffer_size];
    int bytes_read = read(socket_fd, read_buffer, kRead_buffer_size);
    string message = read_buffer;
    if (message.compare("No occurences found.") == 0) {
      cout << message << endl;
      clog << "BYTES RECEIVED: " << bytes_read << endl;
      exit(1);
    }
    istringstream ss(message);
    vector<string> result;
    string copy;
    while(getline(ss, copy, '\n')) {
      if (!copy.empty()) {
        result.push_back(copy);
      }
    }
    for (int i = 0; i < (int)result.size(); i++) {
      cout << i + 1 << '\t' << result[i] << "\n";
    }
    clog << "BYTES RECEIVED: " << bytes_read << endl;

  }
};


int main(int argc, char *argv[]) {
  if (argc != 4)
    return 1;

  DomainSocketClient dsc(argv[1]);
    dsc.RunClient(argv[2], argv[3]);

  return 0;
}
