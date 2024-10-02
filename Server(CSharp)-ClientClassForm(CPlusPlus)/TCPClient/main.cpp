#include <QCoreApplication>
#include <iostream>
#include <vector>
#include "tcpcomm.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TCPComm tcpComm;

    bool tcp_connection_status = tcpComm.Setup("192.168.1.2", 5120);
    std::cout << "TCP Setup Status = " << tcp_connection_status << std::endl;

    bool connection_status = tcpComm.Connection();
    std::cout << "TCP Connect Status = " << connection_status << std::endl;

    for (;;) {
        std::vector<unsigned char> receive = tcpComm.Receive(1);
        if (receive.size() > 0) {
            for (uint  i = 0; i < receive.size(); i++)
                std::cout << "Data = " << receive.at(i) << std::endl;
        }
        usleep(100000);
    }

    std::cout << "Finish." << std::endl;

    return a.exec();
}
