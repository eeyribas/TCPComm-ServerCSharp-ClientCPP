#include <QCoreApplication>
#include <QtNetwork>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>
#include <iostream>
#include <ctime>

#define HOST_NAME    "192.168.1.2"

void *TcpClientInit(void *param);
void *TcpClientReceiver(void *param);

pthread_t tcp_init_thread;
pthread_t tcp_receiver_thread;
int client_socket;
int port_no = 5120;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    printf("TCP Communication started.\n");
    QNetworkInterface eth0_ip = QNetworkInterface::interfaceFromName("eth0");
    QList<QNetworkAddressEntry> entries = eth0_ip.addressEntries();

    if (!entries.isEmpty()) {
        QNetworkAddressEntry entry = entries.first();
        QString client_ip = entry.ip().toString();

        printf("Client IP Adress  : %s\n", client_ip.toUtf8().constData());
        printf("Host IP Adress    : %s\n", HOST_NAME);

        if (pthread_create(&tcp_init_thread, nullptr, *TcpClientInit, nullptr) == -1)
            perror("Not create thread TCP client init.\n");
    } else {
        printf("TCP/IP detect error!\n");
    }

    return a.exec();
}

void *TcpClientInit(void *param)
{
    Q_UNUSED(param);

    printf("TCP Client initialized.\n");
    struct sockaddr_in socket_addr;
    char host_name[] = HOST_NAME;
    struct hostent *host;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        perror("Not Create Socket...\n");

    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons(port_no);
    if ((socket_addr.sin_addr.s_addr = inet_addr(host_name)) == INADDR_NONE) {
        if ((host = gethostbyname(host_name)) == nullptr)
             perror("Not create get host by name.\n");

        memcpy(&socket_addr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    }

    reconnect:

    usleep(1000);
    printf("Connecting...\n");
    if (connect(client_socket, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) == -1) {
        perror("Disconnected error.\n");
        goto reconnect;
    }

    if (pthread_create(&tcp_receiver_thread, nullptr, *TcpClientReceiver, nullptr) == -1)
        perror("Not create thread TCP Client receiver.\n");
    usleep(100);

    return param;
}

void *TcpClientReceiver(void *param)
{
    Q_UNUSED(param);

    printf("TCP Client connected.\n");
    int selected_core = 3;
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(selected_core, &cpu);
    if (sched_setaffinity(syscall(SYS_gettid), sizeof(cpu), &cpu) == -1)
        perror("Not selected core.\n");

    int recv_count = 0;
    int recv_buffer_size = 1;

    for (;;) {
        unsigned char recv_data[recv_buffer_size];

        if ((recv_count = recv(client_socket, &recv_data, recv_buffer_size, 0)) == -1) {

            if (errno == ECONNRESET) {
                shutdown(client_socket, SHUT_RDWR);
                close(client_socket);
            } else if(errno == EBADF) {
                sleep(1);
            }
        } else {
            if (recv_count == 0) {
                shutdown(client_socket, SHUT_RDWR);
                close(client_socket);
            }
        }

        if (recv_count == recv_buffer_size) {
            printf("Recv Data = %u\n", recv_data[0]);

            int send_count = 0;
            int left_count = 1;
            unsigned char buffer[1];
            buffer[0] = 13;

            while (left_count != 0) {

                if ((send_count = send(client_socket, &buffer, left_count, 0)) <= 0) {
                    perror("Send data error.");
                    if (errno == EBADF) {
                        sleep(1);
                        break;
                    }
                }

                if (send_count >= 0)
                    left_count -= send_count;
            }
        }
    }

    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);

    return param;
}
