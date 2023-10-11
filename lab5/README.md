# lab 5 - TCP Socket Programming

## `Test #1` 
### TCP Socket Descriptor
```c++
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int s1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cout << "UDP Socket ID: " << s1 << endl;
    
    int s2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << "TCP Socket ID: " << s2 << endl;
    close(s1);

    int s3 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << "TCP Socket ID: " << s3 << endl;

    close(s2);
    close(s3);
    return 0;
}
// result 
// UDP Socket ID: 3
// TCP Socket ID: 4
// TCP Socket ID: 5
```
* IP 위의 `DGRAM` 은 UDP 밖에 없으므로 IPPROTO_UDP 대신 0을 써도 됩니다.
* IP 위의 `STREAM` 은 TCP 밖에 없으므로 IPPROTO_TCP 대신 0을 써도 됩니다.
* TCP socket descriptor, UDP socket descriptor는 동일하게 숫자 기반입니다.
* `비어있는 숫자가 할당`됩니다.
    * 0, 1, 2 는 각각 표준 입력, 표준 출력, 표준 에러 입니다.
    * 그 다음 비어있는 숫자인 3이 s1 에 할당 됩니다.  
    * 그 다음 비어있는 숫자인 4가 s2 에 할당 됩니다.
    * s1 을 닫았으므로, 3이 비게 되어 3을 s3에 할당 합니다. <br>
    &rarr; close() 후에는 socket descriptor 번호는 재사용됩니다.
* 파일 입출력 같은 I/O 역시 socket descriptor 와 같은 공간의 숫자로 표시됩니다. <br>
(따라서 file 을 open 하면 할당되는 숫자가 변경됩니다.)

## `Test #2` 
### Process 가 동시에 열 수 있는 Descriptor 수
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include <unistd.h>
#include <iostream>

using namespace std;

int main() {
    for(int i=0 ; i < 1024 ; i++){
        int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s < 0){
            cerr << "socket() failed" << i << " " << strerror(errno) << endl;
            break;
        }
    }
    return 0;
}
``` 
![ulimit_result1](/asset/img/ulimit1.png)
<br>

* TCP는 `연결 기반`이므로 대상 하나하나에 대응되는 별도의 소켓(= active socket)을 갖게 됩니다.
* 즉, TCP 서버는 클라이언트 개수 별로 socket descriptor 가 증가합니다. <br>
&rarr; TCP 서버에서는 별도 조치를 취하지 않는 한 일정 숫자 이상의 클라이언트 서비스를 못합니다. (보통 상용 서비스 오픈 후 많이 겪음)

## `Test #3` 
### 연결이 되지 않은 소켓에 데이터 쓰기
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s < 0){
        cerr << "socket() failed: " << strerror(errno) << endl;
        return 1;
    }

    char buf[1024];
    int r = send(s, buf, sizeof(buf), MSG_NOSIGNAL);
    if(r < 0){
        cerr << "send() failed: " << strerror(errno) << endl;
    } else {
        cout << "Sent: " << r << "bytes" << endl;
    }

    close(s);
    return 0;
}
// return 
// send() failed: Broken pipe
```
* UDP는 TCP와 다르게 데이터 전송 전 `connect()` 함수가 호출 되어야 합니다.
* 다음과 같이 연결되지 않은 소켓에 데이터를 쓸 경우, <br>
`send()` 함수는 Broken Pipe (= EPIPE 에러 상수)로 실패합니다.
* `MSG_NOSIGNAL` Flag 가 아닌 0을 넣을 경우 아무 것도 출력이 되지 않습니다. 그 이유는 `echo $?` 결과가 `141`이 나오는 것으로 확인할 수 있습니다. <br> 
&rarr; OS 가 해당 프로세스 강제 종료 <br>
* 만약 0 대신 `MSG_NOSIGNAL`을 사용할 경우 연결이 되지 않은 소켓에 쓰기 작업을 하여 조용히 죽는 것을 막을 수 있습니다. <br>
&rarr; 해당 작업을 통해서 연결이 되지 않은 소켓에 쓰기 작업을 하면 프로세스가 조용히 죽을 수 있다는 것을 아는 것이 더 중요합니다. <br>
**`즉, 서버가 갑자기 아무 메세지도 없이 죽는 일이 발생할 수 있습니다.`** <br>
* `EPIPE` 대신 `ECONNRESET` 이 발생할 수도 있습니다.
* `send()` 대신 `write()`를 사용할 수 있습니다. <br>
&rarr; `Socket Descriptor`는 `File Descriptor`의 일종입니다.

## `Test #4` 
### 연결을 닫은 소켓에 데이터 쓰기
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s < 0){
        cerr << "socket() failed : " << strerror(errno) << endl;
        return 1;
    }
    close(s);
    
    char buf[1024];
    int r = send(s, buf, sizeof(buf), MSG_NOSIGNAL);
    if(r < 0){
        cerr << "send() failed: " << strerror(errno) << endl;
    } else {
        cout << "Sent : " << r << "bytes" << endl;
    }

    close(s);
    return 0;
}
// result
// send() failed: Bad file descriptor
```
**`즉, 아예 없는 descriptor 인 경우와 연결이 안된 socket descriptor는 구분됩니다.`**

## `Test #5` 
### 특정 오류에 대한 처리
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s < 0){
        cerr << "socket() failed : " << strerror(errno) << endl;
        return 1;
    }

    close(s);
    char buf[1024];
    int r = send(s, buf, sizeof(buf), MSG_NOSIGNAL);
    if(r < 0){
        cerr << "send() failed: " << strerror(errno) << endl;
        if(errno == EBADF){
            cout << "Not a Socket: " << s << endl;
        }
    } else {
        cout << "Sent: " << r << "bytes" << endl;
    }
    close(s);
    return 0;
}
// result
// send() failed: Bad file descriptor
// Not a Socket: 3
```
**`함수가 실패하면 `errno` 라는 전역 변수에 에러 코드가 저장되므로 이 값을 통해 특정 에러 코드와 비교할 수 있습니다.`**

## `Test #6` 
### TCP 연결 후 데이터 전송하기
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s < 0) {
        cerr << "socket() failed: " << strerror(errno) << endl;
        return 1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(10001);
    if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        cerr << "connect() failed: " << strerror(errno) << endl;
        return 1;
    }

    char buf[1024];
    int r = send(s, buf, sizeof(buf), 0);
    if(r < 0){
        cerr << "send() failed: " << strerror(errno) << endl;
    } else {
        cout << "Sent: " << r << "bytes" << endl;
    }

    close(s);
    return 0;
}
// result 
// Sent: 1024bytes
```
## `Test #7` 
### TCP 데이터 수신하기
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s < 0){
        cerr << "socket() failed: " << strerror(errno) << endl;
        return 1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(10001);
    if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        cerr << "connect() failed : " << strerror(errno) << endl;
        return 1; 
    }

    char buf[1024];
    int r = send(s, buf, sizeof(buf), 0);
    if(r < 0){
        cerr << "send() failed: " << strerror(errno) << endl;
    } else {
        cout << "Sent: " << r << "bytes" << endl;
    }

    r = recv(s, buf, sizeof(buf), 0);
    if(r < 0){
        cerr << "recv() failed: " << strerror(errno) << endl;
    } else {
        cout << "Received: " << r << "bytes" << endl;
    }

    close(s);
    return 0;
}
// result
// Send: 1024bytes
// Received: 1024bytes
```
**`recv() 대신 read() 함수를 사용할 수도 있습니다.`**

## `Test #8` 
### 연결이 끊겼을 때 recv() 함수의 동작
**`recv() 는 연결이 끊길 경우 `0` 을 반환합니다.`** <br>
다음의 10001 TCP 포트의 서버는 3초 동안 새로운 데이터가 없으면 연결을 끊습니다. <br>
&rarr; 따라서 `send()` 없이 다시 `recv()` 하면 3초 뒤 끊기게 됩니다.
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s < 0){
        cerr << "socket() failed: " << strerror(errno) << endl;
        return 1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(10001);
    if(connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0){
        cerr << "connect() failed: " << strerror(errno) << endl;
        return 1;
    }

    char buf[1024];
    int r = send(s, buf, sizeof(buf), 0);
    if(r < 0){
        cerr << "send() failed: " << strerror(errno) << endl;
    } else {
        cout << "Sent: " << r << "bytes" << endl;
    }

    r = recv(s, buf, sizeof(buf), 0);
    if(r < 0){
        cerr << strerror(errno) << endl;
    } else {
        cout << "Received: " << r << "bytes" << endl;
    }

    r = recv(s, buf, sizeof(buf), 0);
    if(r < 0){
        cerr << "recv() failed: " << strerror(errno) << endl;
    } else if (r == 0) {
        cout << "Socket closed" << endl;
    } else {
        cout << "Received: " << r << "bytes" << endl;
    }

    close(s);
    return 0;
}
// result
// Sent: 1024bytes
// Received: 1024bytes
// Socket closed
```

## `Test #9` 
### recv() 가 일부 데이터만 반환할 때
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main() {
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(s < 0){
        cerr << "socket() failed: " << strerror(errno) << endl;
        return 1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(10001);
    if(connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0){
        cerr << "connect() failed: " << strerror(errno) << endl;
        return 1;
    }

    char buf[60 * 1024];
    int r = send(s, buf, sizeof(buf), 0);
    if(r < 0) {
        cerr << "send() failed: " << strerror(errno) << endl;
    } else {
        cout << "Sent: " << r << "bytes" << endl;
    }

    r = recv(s, buf, sizeof(buf), 0);
    if(r < 0){
        cerr << strerror(errno) << endl;
    } else {
        cout << "Received: " << r << "bytes" << endl;
    }

    r = recv(s, buf, sizeof(buf), 0);
    if(r < 0){
        cerr << "recv() failed: " << strerror(errno) << endl;
    } else if (r == 0){
        cout << "Socket closed" << endl;
    } else {
        cout << "Received: " << r << "bytes" << endl;
    } 

    close(s);
    return 0;
}
// result
// Sent: 61440bytes
// Received: 32768bytes
// Received: 28672bytes
```
* UDP의 `sendto()`는 하나의 datagram 에 데이터를 실어 보냅니다. <br>
그 때문에 상대방이 `recvfrom()` 호출 시, 이 `datagram` 이 전송이 안됐으면 모르되 <br>
전송이 되었다면 recvfrom() 은 전체 data를 반환합니다.
* 반면 TCP 는 연속되는 byte 가 흐르는 것 같은 착각을 줍니다. <br>
이 때문에 `send()` 한 만큼 그대로 상대가 `recv()` 하지 않을 수도 있습니다. <br><br>

**`TCP 에서의 recv() 는 필요한 바이트 수만큼 읽을 때까지 반복해야 합니다.`**

## `Test #10` 
### TCP 서버 기초 흐름
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int passiveSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(20130);
    if (bind(passiveSock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        cerr << "bind() failed: " << strerror(errno) << endl;
        return 1;
    }

    if (listen(passiveSock, 10) < 0){
        cerr << "listen() failed: " << strerror(errno) << endl;
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    unsigned int sin_len = sizeof(sin);
    int clientSock = accept(passiveSock, (struct sockaddr *) &sin, &sin_len);
    if (clientSock < 0){
        cerr << "accept() failed: " << strerror(errno) << endl;
        return 1;
    }

    char buf[65535];
    int numRecv = recv(clientSock, buf, sizeof(buf), 0);
    if (numRecv == 0){
        cout << "Socket closed: " << clientSock << endl;
    } else if (numRecv < 0){
        cout << "recv() failed: " << strerror(errno) << endl;
    } else {
        cout << "Received: " << numRecv << "bytes, clientSock" << clientSock << endl;
    }

    int offset = 0;
    while (offset < numRecv) {
        int numSend = send(clientSock, buf + offset, numRecv - offset, 0);
        if(numSend < 0){
            cerr << "send() failed: " << strerror(errno) << endl;
        } else {
            cout << "Sent: " << numSend << endl;
            offset += numSend;
        }
    }

    close(clientSock);
    close(passiveSock);
}
```

## `Test #11` 
### 여러 클라이언트를 위한 서버
```c++

```