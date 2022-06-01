#ifndef SOCKET_H
#define SOCKET_H

#include <sys/un.h>  // defns for Unix domain sockets, using struct sockaddr_un

#include <string>

using namespace std;

class UnixDomainSocket {
  public:
    explicit UnixDomainSocket(const char *socket_path);

protected:
  ::sockaddr_un sock_addr_; // socket address from sys/un.h
  string socket_path_; // let std::string manage char*
};

#endif // SOCKET_H
