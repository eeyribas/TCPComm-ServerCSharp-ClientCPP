#include "tcpcomm.h"

bool TCPComm::Setup(const char* host_name , uint16_t port_no)
{
    struct hostent *host;

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        return false;

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port_no);
    if ((sock_addr.sin_addr.s_addr = inet_addr(host_name)) == INADDR_NONE) {
        if ((host = gethostbyname(host_name)) == nullptr)
            return false;

        memcpy(&sock_addr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    }

    return true;
}

bool TCPComm::Connection()
{
    if (connect(sock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1)
        return false;

    return true;
}

bool TCPComm::Send(vector<unsigned char> data)
{
    unsigned char send_data[data.size()];
    int send_count = 0;
    int left_count = static_cast<int>(data.size());

    for (unsigned int i = 0; i < data.size(); i++) {
        send_data[i] = data.at(i);
        printf("%u ", send_data[i]);
    }
    printf("\n");

    while (left_count != 0) {
        if ((send_count = send(sock, &send_data, left_count, false)) <= 0) {

            if (errno == EBADF) {
                this_thread::sleep_for(chrono::seconds(1));
                return false;
            }
        }

        if (send_count >= 0)
            left_count -= send_count;
    }

    return true;
}

vector<unsigned char> TCPComm::Receive(unsigned int receive_size)
{
    struct timeval tv;
    tv.tv_sec=0;
    tv.tv_usec=10000;
    setsockopt(sock, SOL_SOCKET,SO_RCVTIMEO,(const char *)&tv,sizeof(timeval));

    vector<unsigned char> vector_data;
    unsigned char receive_data[sizeof(receive_size)];
    int recv_count = 0;
    if ((recv_count = recv(sock, &receive_data, receive_size, 0)) == 0) {
        if (errno == ECONNRESET) {
            this_thread::sleep_for(chrono::seconds(1));
            Close();
            vector_data = ErrorStatus(receive_size);
        } else if (errno == EBADF) {
            this_thread::sleep_for(chrono::seconds(1));
            Close();
            vector_data = ErrorStatus(receive_size);
        }
    } else {
        if (recv_count == 0) {
            this_thread::sleep_for(chrono::seconds(1));
            Close();
            vector_data = ErrorStatus(receive_size);
        }
    }

    if (recv_count == static_cast<int>(receive_size)) {
        for (unsigned int i = 0; i < receive_size; i++)
            vector_data.push_back(receive_data[i]);
    }

    return vector_data;
}

void TCPComm::Close()
{
    shutdown(sock, SHUT_RDWR);
    close(sock);
}

bool TCPComm::ErrorControl(vector<unsigned char> data)
{
    unsigned int count = 0;

    for (unsigned int i = 0; i < data.size(); i++) {
        if (data.at(i) == 0)
            count++;
    }

    if (count == data.size())
        return true;

    return false;
}

vector<unsigned char> TCPComm::ErrorStatus(unsigned int receive_size)
{
    vector<unsigned char> error_data;
    for (unsigned int i = 0; i < receive_size; i++)
        error_data.push_back(0);

    return error_data;
}
