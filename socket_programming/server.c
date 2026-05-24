#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT "8080"
#define BACKLOG 10
#define BUF_SIZE 4096

/* -------------------------------------------------------
 * /calc?query=<expression> を評価する
 * 対応演算子: + - * /
 * 返り値: 動的確保した結果文字列（呼び出し元が free する）
 *         エラー時は NULL
 * ------------------------------------------------------- */
static char *calc(const char *query_string) {
    /* query_string は "query=2+10" など */
    const char *eq = strchr(query_string, '=');
    if (!eq) return NULL;

    const char *expr = eq + 1;   // const char*
    printf("DEBUG expr: [%s]\n", expr);

    double left, right;
    char op;
    if (sscanf(expr, " %lf %c %lf", &left, &op, &right) != 3) {
        return NULL;  // free(expr) を削除
    }

    double result;
    switch (op) {
        case '+': result = left + right; break;
        case '-': result = left - right; break;
        case '*': result = left * right; break;
        case '/':
            if (right == 0) return NULL;
            result = left / right;
            break;
        default: return NULL;
    }

    /* 整数なら整数表示、そうでなければ小数表示 */
    char *buf = malloc(64);
    if (!buf) { perror("malloc"); exit(1); }
    if (result == (long long)result)
        snprintf(buf, 64, "%lld", (long long)result);
    else
        snprintf(buf, 64, "%g", result);
    return buf;
}

/* -------------------------------------------------------
 * 1リクエストの処理
 * ------------------------------------------------------- */
static void handle_request(int fd) {
    /* リクエスト全体を読む */
    char *req = malloc(BUF_SIZE);
    if (!req) { perror("malloc"); exit(1); }
    ssize_t total = 0;
    while (total < BUF_SIZE - 1) {
        ssize_t n = read(fd, req + total, BUF_SIZE - 1 - total);
        if (n < 0) { perror("read"); free(req); return; }
        if (n == 0) break;
        total += n;
        /* ヘッダー終端 \r\n\r\n が来たら終了 */
        req[total] = '\0';
        if (strstr(req, "\r\n\r\n")) break;
    }
    req[total] = '\0';
    printf("--- request ---\n%s\n", req);

    /* リクエストライン解析: "GET /path?query HTTP/1.x" */
    char method[16], path[1024], version[16];
    if (sscanf(req, "%15s %1023s %15s", method, path, version) != 3) {
        free(req);
        return;
    }
    free(req);

    /* パスとクエリ文字列に分割 */
    char *qmark = strchr(path, '?');
    char *query_string = NULL;
    if (qmark) {
        *qmark = '\0';
        query_string = qmark + 1;
    }

    /* レスポンスボディ生成 */
    char *body = NULL;
    int status = 200;
    const char *status_msg = "OK";

    if (strcmp(method, "GET") == 0 && strcmp(path, "/calc") == 0 && query_string) {
        body = calc(query_string);
    }

    if (!body) {
        status = 400;
        status_msg = "Bad Request";
        body = strdup("Bad Request");
        if (!body) { perror("strdup"); exit(1); }
    }

    /* レスポンス送信 */
    size_t body_len = strlen(body);
    char *header = malloc(256);
    if (!header) { perror("malloc"); exit(1); }
    int hlen = snprintf(header, 256,
        "HTTP/1.1 %d %s\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, status_msg, body_len);

    /* ヘッダー */
    ssize_t written = 0;
    while (written < hlen) {
        ssize_t n = write(fd, header + written, hlen - written);
        if (n < 0) { perror("write"); break; }
        written += n;
    }
    /* ボディ */
    written = 0;
    while ((size_t)written < body_len) {
        ssize_t n = write(fd, body + written, body_len - written);
        if (n < 0) { perror("write"); break; }
        written += n;
    }

    free(header);
    free(body);
}

/* -------------------------------------------------------
 * サーバーソケット作成 (IPv4/IPv6 両対応)
 * ------------------------------------------------------- */
static int create_server_socket(void) {
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;    /* IPv4 / IPv6 どちらでも */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;   /* INADDR_ANY */

    int rv = getaddrinfo(NULL, PORT, &hints, &res);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    int sfd = -1;
    for (p = res; p != NULL; p = p->ai_next) {
        sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sfd < 0) { perror("socket"); continue; }

        int yes = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0) {
            perror("setsockopt"); close(sfd); continue;
        }

        if (bind(sfd, p->ai_addr, p->ai_addrlen) < 0) {
            perror("bind"); close(sfd); continue;
        }
        break; /* 成功 */
    }
    freeaddrinfo(res);

    if (sfd < 0) {
        fprintf(stderr, "failed to bind\n");
        exit(1);
    }

    if (listen(sfd, BACKLOG) < 0) {
        perror("listen"); exit(1);
    }
    return sfd;
}

/* -------------------------------------------------------
 * main
 * ------------------------------------------------------- */
int main(void) {
    int sfd = create_server_socket();
    printf("Listening on port %s ...\n", PORT);

    while (1) {
        struct sockaddr_storage client_addr;
        socklen_t addrlen = sizeof client_addr;
        int cfd = accept(sfd, (struct sockaddr *)&client_addr, &addrlen);
        if (cfd < 0) { perror("accept"); continue; }

        /* クライアントの IP アドレスを表示 */
        char ipstr[INET6_ADDRSTRLEN];
        if (client_addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&client_addr;
            inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        } else {
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&client_addr;
            inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        }
        printf("Connection from %s\n", ipstr);

        handle_request(cfd);
        close(cfd);
    }

    close(sfd);
    return 0;
}
