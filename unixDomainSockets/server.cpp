// Copyright 2022 Ana Boccanfuso
//
#include "socket.h"
#include <sys/socket.h>  // Unix header for sockets, using socket
#include <sys/un.h>  // defns for Unix domain sockets, using struct sockaddr_un
#include <sys/sysinfo.h>
#include <unistd.h>  // Unix standard header, using close
#include <stdlib.h>
#include <stdio.h>

#include <cassert>  // using assert
#include <cerrno>  // using errno
#include <cstddef>  // using size_t
#include <cstdlib>  // exit
#include <cstring>  // using strncpy, strerror

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

UnixDomainSocket::UnixDomainSocket(const char *socket_path) {
    socket_path_ = std::string(socket_path);  // std::string manages char *

    sock_addr_ = {};  // init struct (replaces memset)
    sock_addr_.sun_family = AF_UNIX;  // set to Unix domain socket (e.g. instead
                                      //   of internet domain socket)
    // leaving leading null char sets abstract socket
    strncpy(sock_addr_.sun_path + 1,  // use strncpy to limit copy for
            socket_path,              //   portability
            sizeof(sock_addr_.sun_path) - 2);  // -2 for leading/trailing \0s
  }

string finder(string path, string search) {
  ifstream in(path.c_str());
  string found = "";
  size_t begin;
  if (in.is_open()) {
    string line;
    while (getline(in, line)) {
      if (line.find(search) != string::npos) {
        begin = line.find_first_not_of(' ');
        found += line.substr(begin, line.size());
        found += "\n";
      }
    }
  }
  else {
    cerr << "INVALID FILE" << endl;
    exit(1);
  }
  return found;
}

// Domain Socket Server C++ Interface Class
//
class DomainSocketServer : public UnixDomainSocket{
 public:
  using ::UnixDomainSocket::UnixDomainSocket;

  void RunServer() const {
    int sock_fd;  // unnamed socket file descriptor
    int client_req_sock_fd;  // client connect request socket file descriptor

    // (1) create a socket
    //       AF_UNIX -> file system pathnames
    //       SOCK_STREAM -> sequenced bytestream
    //       0 -> default protocol (let OS decide correct protocol)
    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if ( sock_fd < 0 ) {
      cerr << strerror(errno) << endl;
      exit(-1);
    }

    // (2) bind socket to address for the server
    unlink(socket_path_.c_str());  // sys call to delete file if it exists
                                   // already using Unix system calls for
                                   //   sockets, no reason to be non-Unix
                                   //   portable now.  :-/
    int success = bind(sock_fd,
                       // sockaddr_un is a Unix sockaddr and so may be cast "up"
                       //   to that pointer type (think of it as C polymorphism)
                       reinterpret_cast<const sockaddr*>(&sock_addr_),
                       // size needs be known due to underlying data layout,
                       //   i.e., there may be a size difference between parent
                       //   and child
                       sizeof(sock_addr_));
    if (success < 0) {
      cerr << strerror(errno) << endl;
      exit(-1);
    }

    clog << "SERVER STARTED" << endl;

    // (3) Listen for connections from clients
    size_t kMax_client_conns = get_nprocs_conf() - 1;
    //size_t kMax_client_conns = 7;
    clog << "\tMAX CLIENTS: " << kMax_client_conns << endl;
    success = listen(sock_fd, kMax_client_conns);
    if (success < 0) {
      cerr << strerror(errno) << endl;
      exit(-1);
    }

    int bytes_read;
    const size_t kRead_buffer_size = 64;
    char read_buffer[kRead_buffer_size];

    while (true) {
      // (4) Accept connection from a client
      client_req_sock_fd = accept(sock_fd, nullptr, nullptr);
      if (client_req_sock_fd  < 0) {
        cerr << strerror(errno) << endl;
        continue;
      }

      clog << "CLIENT CONNECTED" << endl;

      // (5) Receive data from client(s)
      int index;
      bytes_read = read(client_req_sock_fd, read_buffer, kRead_buffer_size);
      for (int i = 0; i < bytes_read; i++) {
        if (read_buffer[i] == '+') {
          index = i;
        }
      }
      read_buffer[bytes_read] = '\0';
      string total = read_buffer;
      string path = total.substr(0, index);
      string seek = total.substr(index + 1);

      clog << "PATH: " << "\"" << path.c_str() << "\"" << endl;
      clog << "SEEKING: " << "\"" << seek.c_str() << "\"" << endl;

      memset(read_buffer, 0, sizeof(read_buffer));

      string message = finder(path, seek);
      string otherwise = "No occurences found.";
      if (message.compare("") == 0) {
        int none = send(client_req_sock_fd, otherwise.c_str(), strlen(otherwise.c_str()), 0);
        clog << "BYTES SENT: " << none << endl;
      }
      else {
        int bytes_wrote = send(client_req_sock_fd, message.c_str(), strlen(message.c_str()), 0);
        clog << "BYTES SENT: " << bytes_wrote << endl;

      }
      }
    }
};


int main(int argc, char *argv[]) {
  if (argc != 2)
    return 1;

  DomainSocketServer dss(argv[1]);
  dss.RunServer();

  return 0;
}
