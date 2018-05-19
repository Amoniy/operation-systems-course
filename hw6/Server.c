#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/event.h>

#define CONST_SIZE 1000
static struct kevent kevent_struct, event_list[CONST_SIZE];

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int socket_listener_descriptor, port_number, client_socket, message_size;
    struct sockaddr_in serv_addr;
    struct hostent *host;

    char buffer[256];
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    port_number = atoi(argv[1]);

    socket_listener_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_listener_descriptor < 0) {
        fprintf(stderr, "Error - could not open socket");
        exit(0);
    }

    host = gethostbyname(argv[2]);
    if (host == NULL) {
        fprintf(stderr, "Error - no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port_number);

    if (bind(socket_listener_descriptor, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error with connection");
    }

    listen(socket_listener_descriptor, CONST_SIZE);

    int kqueue_descriptor = kqueue();
    if (kqueue_descriptor < 0) {
        perror("kqueue");
        exit(0);
    }

    EV_SET(&kevent_struct, socket_listener_descriptor, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kqueue_descriptor, &kevent_struct, 1, NULL, 0, NULL) < 0) {
        perror("kevent(listener)");
        exit(0);
    }

    int event_identifiers[CONST_SIZE];

    while (1) {
        int events_count = kevent(kqueue_descriptor, NULL, 0, event_list, CONST_SIZE, NULL);
        if (events_count < 0) {
            return -1;
        }
        for (size_t i = 0; i < events_count; ++i) {
            event_identifiers[i] = event_list[i].ident;
        }

        for (size_t i = 0; i < events_count; ++i) {
            if (event_identifiers[i] == socket_listener_descriptor) {
                client_socket = accept(socket_listener_descriptor, NULL, NULL);
                if (client_socket < 0) {
                    perror("Something went wrong with connection");
                    exit(0);
                }
                EV_SET(&kevent_struct, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                if (kevent(kqueue_descriptor, &kevent_struct, 1, NULL, 0, NULL) < 0) {
                    perror("socket_descriptor");
                    exit(0);
                }
                printf("подключились к клиенту\n");
            } else { // event_identifiers[i] == client_socket, но он всегда разный, так что непонятно, как отлавливать какие-то неправильный айдентифаеры
                client_socket = event_identifiers[i];

                message_size = recv(client_socket, buffer, 256, 0);
                if (message_size <= 0) {
                    close(client_socket);
                    break;
                }
                if (strcmp(buffer, "close server") == 0) {
                    printf("Получено Сообщение: %s\n", buffer);
                    printf("Закрываем сервер\n");
                    close(kqueue_descriptor);
                    close(socket_listener_descriptor);
                    close(client_socket);
                    return 0;
                }
                if (strcmp(buffer, "exit") == 0) {
                    printf("Получено Сообщение: %s\n", buffer);
                    printf("Закрываем подключение к клиенту\n");
                    close(client_socket);
                    continue;
                }
                printf("Получено Сообщение: %s\n", buffer);
                printf("Отправляю принятое сообщение клиенту\n");
                send(client_socket, buffer, message_size, 0);

                if (message_size <= 0) {
                    printf("Закрываем подключение к клиенту\n");
                    close(client_socket);
                }
            }
        }
    }
    return 0;
}
