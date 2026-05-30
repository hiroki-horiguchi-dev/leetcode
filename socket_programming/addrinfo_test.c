// 確認用コード
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("localhost", "8080", &hints, &res);

    int count = 0;
    for (p = res; p != NULL; p = p->ai_next) {
        char ipstr[INET6_ADDRSTRLEN];
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
            printf("[%d] IPv4: %s\n", count, ipstr);
        } else {
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)p->ai_addr;
            inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
            printf("[%d] IPv6: %s\n", count, ipstr);
        }
        count++;
    }
    printf("合計: %d個\n", count);
    freeaddrinfo(res);
}