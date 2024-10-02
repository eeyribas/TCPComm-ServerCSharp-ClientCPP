#ifndef TCPCOMM_H
#define TCPCOMM_H

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

using namespace  std;

class TCPComm
{
public:
    bool Setup(const char *host_name, uint16_t port_no);
    bool Connection();
    bool Send(vector<unsigned char> data);
    vector<unsigned char> Receive(unsigned int receive_size);
    void Close();
    bool ErrorControl(vector<unsigned char> data);

private:
    vector<unsigned char> ErrorStatus(unsigned int receive_size);

    struct sockaddr_in sock_addr;
    int sock;
};

#endif // TCPCOMM_H
