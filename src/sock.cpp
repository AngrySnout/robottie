#include <cstring>
#include <fcntl.h>
#include "sock.h"

#define MAXSIZE 4096

#define try(c,m) if(c) {std::cout << m << std::endl; return false;}
#define tryf(c,m,f) if(c) {std::cout << m << std::endl; f; return false;}
bool sock::Init() { 
  try((_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET, "Socket error.")

  int on = 1;
  try(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char const*)&on, sizeof(on)) == -1, "Invalid socket.")
  
  fcntl(_socket, F_SETFL, O_NONBLOCK);
  fcntl(_socket, F_SETFL, O_ASYNC);
  return true;
}

bool sock::Connect(char const* host, int port) {
  hostent* he;
  try(!(he = gethostbyname(host)), "Could not resolve host: ")

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *((const in_addr*)he->h_addr);
  memset(&(addr.sin_zero), '\0', 8);

  tryf(connect(_socket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR, "Could not connect to: ", closesocket(_socket);)

  _connected = true;
  return true;
}

void sock::Disconnect() {
  if (_connected) {
    closesocket(_socket);
    _connected = false;
  }
}

bool sock::SendData(char const* data) {
  if (_connected)
    if (send(_socket, data, strlen(data), 0) == -1)
      return false;

  return true;
}

std::string sock::ReceiveData() {
  char buffer[MAXSIZE];
  memset(buffer, 0, MAXSIZE);
  
  int bytes = recv(_socket, buffer, MAXSIZE - 1, 0);
  if (bytes > 0)
    return std::string(buffer);
  else
    Disconnect();

  return "";
}
