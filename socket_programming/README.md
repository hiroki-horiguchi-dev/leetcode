## ビルド実行方法
```shell
gcc -Wall -Wextra -o server server.c
gcc -Wall -Wextra -o client client.c

./server &  # サーバー起動

# curl でテスト
curl "http://localhost:8080/calc?query=2+10"  # → 12
curl "http://localhost:8080/calc?query=3*4"   # → 12
curl "http://localhost:8080/calc?query=10/0"  # → Bad Request

# 自作クライアントでテスト
➜  socket_programming git:(socket-programming) ✗ ./client "10+2"      
HTTP/1.1 200 OK
Content-Length: 2
Content-Type: text/plain
Connection: close

12
➜  socket_programming git:(socket-programming) ✗ ./client "10*2"
HTTP/1.1 200 OK
Content-Length: 2
Content-Type: text/plain
Connection: close

20
➜  socket_programming git:(socket-programming) ✗ ./client "10-2"
HTTP/1.1 200 OK
Content-Length: 1
Content-Type: text/plain
Connection: close

8
➜  socket_programming git:(socket-programming) ✗ ./client "10/0"
HTTP/1.1 400 Bad Request
Content-Length: 11
Content-Type: text/plain
Connection: close

Bad Request
➜  socket_programming git:(socket-programming) ✗ ./client "10/2"
HTTP/1.1 200 OK
Content-Length: 1
Content-Type: text/plain
Connection: close

5


## 対応OS
- Mac / Linux (Ubuntu 等)
- Windows は非対応 (unistd.h 等の POSIX ヘッダーが使用不可なため。対応するには Winsock に書き直す必要あり)