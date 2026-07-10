#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// サーバーのIPアドレスとPORT番号
// ドメイン名（例: google.com）が指定された場合:
// 1. まず ISP（ソフトバンク、NTTなど）の DNS サーバーに問い合わせる
// 2. ISP が知らなければルート DNS サーバー（世界13台）に問い合わせる
// 3. さらに権威 DNS サーバーに問い合わせて IP アドレスを取得
// 4. 返ってきた IP アドレスの HOST:PORT に connect() しに行く
// ※ DNSはサーバーのポートは返さないが、PORTは http: 80, https: 443 なので特別指定しなくても問題ない

// 自分のPCが契約しているプロバイダのDNSをちゃんと使っているのか調べてみた
// ➜  / cat etc/resolv.conf : 自分のルーターが使っているIPアドレスがわかる
// ➜  / scutil --dns:
// →  / nslookup xxxx:xxxx:xx:xxx....:xxx
// →  / whois xxxx:xxxx:xx:xxx....:xxx: descr: BBIX IPv6 Network SoftbankBB ABUSE e-mail: abuse@e.softbank.co.jp
// うん、ちゃんと使ってた
#define HOST "127.0.0.1"
#define PORT "8080"

int main(int argc, char *argv[]) {
    const char *expr = (argc > 1) ? argv[1] : "2+10";

    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof hints);
    //  hints を全て0埋めしてくれる
    //  addrinfo の構造体は以下
    //  struct addrinfo {
    //       int ai_flags;              /* input flags */
    //       int ai_family;             /* protocol family for socket */        --> AF-INET(IPv4) or AF_INET6(IPv6)
    //       int ai_socktype;           /* socket type */                       --> TCP or UDP
    //       int ai_protocol;           /* protocol for socket */               --> IPPROTO_TCP or IPPROTO_UDP
    //       socklen_t ai_addrlen;      /* length of socket-address */          --> ai_addr のサイズ
    //       struct sockaddr *ai_addr;  /* socket-address for socket */         --> 実際のアドレス+ポート番号
    //       char *ai_canonname;        /* canonical name for service location */ --> ホスト名
    //       struct addrinfo *ai_next;  /* pointer to next in list */             --> 次の addrinfo ノードへのポインタ
    //   };
    //  *ai_next が LinkedList みたいにポインタで次の要素を指してくれるので、server.c 側で for (p = res; p != NULL; p->ai_next)とかけた
    //  なぜLinkedListのような構造なのか？
    //  1つのホストに対して、複数アドレスが返ってくる可能性があるため
    //  検証コードを addrinfo_text.c において、実行結果を見ると
    //  ➜  socket_programming git:(socket-programming) ✗ gcc -o addrinfo_test addrinfo_test.c && ./addrinfo_test
    //    [0] IPv6: ::1
    //    [1] IPv4: 127.0.0.1
    //    合計: 2個
    //  IPv4, IPv6 の2つのアドレスが返ってくる、server.c では、この2つのアドレスに対して bind を試みる

    hints.ai_family   = AF_UNSPEC;　// IPv4, IPv6 どちらも対応
    hints.ai_socktype = SOCK_STREAM;
    // man 2 socket で確認。
    // TCP のこと、順序保証、信頼性、双方向接続
    // A SOCK_STREAM type provides sequenced, reliable, two-way connection based byte streams.
    // SOCK_DGRAM: UDP, connectionless, unreliable message
    // SOCK_RAW: 内部ネットワークプロトコルへの直接アクセス、root権限が必要

    int rv = getaddrinfo(HOST, PORT, &hints, &res); // HOST: 接続先のIPアドレス, PORT: 接続先のポート
    if (rv != 0) { fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); return 1; }

    int sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    // アロー演算子、ポインタ経由でアクセスしている
    // Java で書くところの res.ai_family と同じ
    // man 2 socket
    // RETURN VALUES
    // A -1 is returned if an error occurs, otherwise the return value is a descriptor referencing the socket.

    if (sfd < 0) { perror("socket"); freeaddrinfo(res); return 1; }

    if (connect(sfd, res->ai_addr, res->ai_addrlen) < 0) { perror("connect"); close(sfd); freeaddrinfo(res); return 1; }
    freeaddrinfo(res);

    /* リクエスト送信 */
    char req[512]; // スタック領域に512byte確保
    int reqlen = snprintf(req, sizeof req,
        "GET /calc?query=%s HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n",
        expr);
    write(sfd, req, reqlen);

    /* レスポンス受信・表示 */
    char buf[4096]; // スタック領域に4096byte確保、スタックはメモリ領域の名前で、取り出されると自動でその分のメモリは解放される特徴がある。free()不要
    ssize_t n;
    while ((n = read(sfd, buf, sizeof buf - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
    }
    printf("\n");
    close(sfd);
    return 0;
}
