#ifndef _IRCSOCKET_H
#define _IRCSOCKET_H

#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket(s) close(s)
#define close(s)
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif

class sock {
public:
    bool Init();
    bool Connect(char const* host, int port);
    void Disconnect();
    bool Connected() { return _connected; };
    bool SendData(char const* data);
    std::string ReceiveData();

private:
    int _socket;
    bool _connected;
};

#endif
