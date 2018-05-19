#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/event.h>
#include <memory.h>

#define CONST_SIZE 1000
static struct kevent kevent_struct, event_list[CONST_SIZE];

int main(int argc, char **argv) {
    char message[256];
    char buf[sizeof(message)];
    int port;

    if (argc != 3) {
        printf("Wrong arguments: \"port, ip\" expected\n");
        exit(0);
    }

    int socket_descriptor;
    struct sockaddr_in addr;
    struct hostent *host;
    port = atoi(argv[1]);
    host = gethostbyname(argv[2]);
    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor < 0) {
        perror("socket");
        exit(0);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(socket_descriptor, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("Could not connect");
        exit(0);
    }

    int kqueue_descriptor = kqueue();
    if (kqueue_descriptor < 0) {
        perror("kqueue");
        exit(0);
    }
    // EVFILT_READ - тип фильтра. следим за EVFILT_READ
    // EV_ADD | EV_ENABLE - флаги. EV_ADD - добавить событие, EV_ENABLE - тут по идее не нужен, тк EV_ADD уже его вызывает
    // NULL - указатель который оно будет отдавать. мне там ничего не нужно
    EV_SET(&kevent_struct, STDIN_FILENO, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    // регистриция события
    if (kevent(kqueue_descriptor, &kevent_struct, 1, NULL, 0, NULL) < 0) {
        perror("STDIN_FILENO");
        exit(0);
    }

    // то же самое, но уже про сокет
    EV_SET(&kevent_struct, socket_descriptor, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kqueue_descriptor, &kevent_struct, 1, NULL, 0, NULL) < 0) {
        perror("socket_descriptor");
        exit(0);
    }

    int event_identifiers[CONST_SIZE];
    printf("Введите сообщение серверу (Для выхода - \"exit\"): ");
    while (1) {
        int events_count = kevent(kqueue_descriptor, NULL, 0, event_list, CONST_SIZE, NULL);
        if (events_count < 0) {
            return -1;
        }
        for (size_t i = 0; i < events_count; ++i) {
            event_identifiers[i] = event_list[i].ident;
        }

        for (size_t i = 0; i < events_count; ++i) {
            if (event_identifiers[i] == STDIN_FILENO) {
                gets(message);
                if (!strcmp(message, "exit")) {
                    printf("отправка сообщения о выходе на сервер...\n"); // нужно ли мне вообще это?
                    send(socket_descriptor, message, sizeof(message), 0);
                    close(socket_descriptor);
                    close(kqueue_descriptor);
                    return 0;
                }
                printf("отправка сообщения на сервер...\n");
                send(socket_descriptor, message, sizeof(message), 0);
                printf("Ожидание сообщения\n");
            } else if (event_identifiers[i] == socket_descriptor) {
                if (recv(socket_descriptor, buf, sizeof(message), 0) <= 0) {
                    printf("Сервер был закрыт\n");
                    close(socket_descriptor);
                    close(kqueue_descriptor);
                    return 0;
                }
                printf("Получено сообщение: %s\n", buf);
            } else {
                perror("unexpected event identifier");
                exit(0);
            }
        }
    }
}
