#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define HOST "127.0.0.1"
#define PORT "8080"

int main(int argc, char *argv[]) {
    const char *expr = (argc > 1) ? argv[1] : "2+10";

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rv = getaddrinfo(HOST, PORT, &hints, &res);
    if (rv != 0) { fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); return 1; }

    int sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sfd < 0) { perror("socket"); return 1; }

    if (connect(sfd, res->ai_addr, res->ai_addrlen) < 0) { perror("connect"); return 1; }
    freeaddrinfo(res);

    /* リクエスト送信 */
    char req[512];
    int reqlen = snprintf(req, sizeof req,
        "GET /calc?query=%s HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n",
        expr);
    write(sfd, req, reqlen);

    /* レスポンス受信・表示 */
    char buf[4096];
    ssize_t n;
    while ((n = read(sfd, buf, sizeof buf - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
    }
    printf("\n");
    close(sfd);
    return 0;
}
