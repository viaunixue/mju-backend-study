# Socket Options

## Socket 의 옵션 확인 및 변경
```c++
int getsockopt (SOCKET s, int level, int optname, char *optval, int *optlen)
int setsockopt (SOCKET s, int level, int optname, char *optval, int optlen)
```

### 자주 사용하는 옵션 1 -  `SO_REUSEADDR`
```c++
int on = 1
setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
```
* bind() 는 특정 L4 타입에 대해서 <IP, Port> 쌍이 사용 중이 아닐 경우만 가능합니다. 
* OS는 한 번 `bind`했던 것을 바로 사용 가능한 것으로 해제하지 않을 수 있습니다. <br>
이는 서버를 리부팅 했는데 bind()를 실패하는 경우가 생길 수도 있음을 의미합니다. <br>
```
만약 1000번 포트를 bind 하는데 만약 서버가 죽었을 때 서버를 다시 띄우면 바로 bind 못하는 경우도 존재합니다. 
왜냐하면 TCP 같은 경우 이미 다른 Client와 통신한 적이 있을 수 있습니다. (1000번 포트로 어떤 데이터가 날아오고 있을 수도 있다는 뜻)
그렇기 때문에 `bind`를 바로 못하도록 OS에서 막도록 되어 있습니다.
``` 
<br>

`다음은 SO_REUSEADDR 옵션이 적용된 lab5의 test12.cpp 코드입니다.`
```c++
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

#include <unistd.h>
#include <iostream>

using namespace std;

int main() {
    int passiveSock = socket(AF_INET, SOCK_STREAM, 0);

    int on = 1;
    if(setsockopt(passiveSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))){
        cerr << "setsock() failed: " << strerror(errno) << endl;
        return 1;
    }

    int v;
    unsigned int vsize = sizeof(v);
    if(getsockopt(passiveSock, SOL_SOCKET, SO_REUSEADDR, &v, &vsize) < 0){
        cerr << "getsockopt() failed: " << strerror(errno) << endl;
        return 1;
    }
    cout << "getsockopt: " << v << endl;

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(20000 + 130);
    if(bind(passiveSock, (struct sockaddr *) &sin, sizeof(sin)) < 0){
        cerr << "bind() failed: " << strerror(errno) << endl;
        return 1;
    }

    if(listen(passiveSock, 10) < 0){
        cerr << "listen() failed: " << strerror(errno) << endl;
        return 1;
    }

    while(true){
        memset(&sin, 0, sizeof(sin));
        unsigned int sin_len = sizeof(sin);
        int clientSock = accept(passiveSock, (struct sockaddr *) &sin, &sin_len);
        if(clientSock < 0){
            cerr << "accept() failed: " << strerror(errno) << endl;
            return 1;
        }

        char buf[65535];
        int numRecv = recv(clientSock, buf, sizeof(buf), 0);
        if(numRecv == 0){
            cout << "Socket closed: " << clientSock << endl;
        } else if (numRecv < 0) {
            cout << "recv() failed: " << strerror(errno) << endl;
        } else {
            cout << "Received: " << numRecv << "bytes, clientSock " << clientSock << endl;
        }

        int offset = 0;
        while(offset < numRecv) {
            int numSend = send(clientSock, buf + offset, numRecv - offset, 0);
            if(numSend < 0){
                cerr << "send() failed: " << strerror(errno) << endl;
            } else {
                cout << "Sent: " << numSend << endl;
                offset += numSend;
            }
        }
        close(clientSock);
    }
    close(passiveSock);
}
// result 
// getsockopt: 1
// Received: 1024bytes, clientSock 4
// Sent: 1024
// Received: 1024bytes, clientSock 4
// Sent: 1024
// ^C
```
* 아래의 결과 값은 test7 값을 receive 한 결과 입니다. <br>
getsockopt 값이 출력될 수 있는 이유는 `SO_REUSEADDR`을 사용하였기 때문입니다. <br>
&rarr; **`직접 TCP 서버를 만들 때에는 거의 반드시 사용해야 하는 옵션입니다.`** <br>
<br>

<details>
   <summary> <strong>이미 `bind()` 한 프로세스가 떠 있는데, 두 번째 프로세스가 똑같은 IP, port를 bind() 한다고 해보자.
이제 두 프로세스가 모두 bind()를 하고 있는 상황이 되었다. 이 경우 어떤 쪽이 새 TCP 연결을 받게 될까?</strong> </summary>

&rarr; 알 수 없습니다. <br>
실무적으로 `SO_REUSEADDR`은 서버를 리부팅했을 때 일시적으로 `bind()`에 실패하는 것을 완화하기 위함이지 이처럼 복수 개의 서버가 같이 bind 하는 것이 목적이 아닙니다. <br>
**따라서 서버가 이미 떠 있는데, SO_REUSEADDR 로 같은 포트를 bind 하는 것은 피해야 합니다.**

</details>

### 자주 사용하는 옵션 2 -  `TCP_NODELAY`

```c++
int on = 1;
setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
```
* `Nagle's algorithm` 은 작은 데이터를 바로바로 보내지 않고 잠깐 기다리면서 모아뒀다가 합쳐서 보내게 됩니다.
* **이 때문에 `latency`를 증가시킬 수 있다.**
&rarr; Interactive 한 app 일 경우 이는 문제가 됩니다. (ex. RPG 캐릭터의 움직임) 
* `TCP_NODELAY` 옵션을 켜면 `Nagle's algorithm` 을 끈다는 의미입니다.

### Socket이 사용 중인 L3, L4 주소 얻어내는 방법

* 내 주소 얻어내기 (UDP, TCP)
```c++
struct sockaddr_in sin;
memset(&sin, 0, sizeof(sin));
unsigned int sin_size = sizeof(sin);
if (getsockname(sock, (struct sockaddr *) &sin, &sin_size)  == 0){
    cout << "My address: " << inet_ntoa(sin.sin_addr) << endl;
    cout << "My port: " << ntohs(sin.sin_port) << endl;
}
```

* 상대방 주소 얻어내기 (TCP)
```c++
struct sockaddr_in sin;
memset(&sin, 0, sizeof(sin));
unsigned int sin_size = sizeof(sin);
if (getpeername(sock, (struct sockaddr *) &sin, &sin_size) == 0){
    cout << "Your address: " << inet_ntoa(sin.sin_addr) << endl;
    cout << "Your port: " << ntohs(sin.sin_port) << endl;
}
```
* 일반적으로 UDP에서는 `sendto()` 할 때 주소를 명시합니다. 받는 측에서는 `recvfrom()`으로 얻어냅니다. <br>
TCP 같은 경우 `connect()` 할 때, 주소를 명시하고, 받는 쪽은 `accept()` 할 때 주소가 나옵니다.

* 보통 `Client`에서 log 나 자료구조를 통해 해당 내용을 저장합니다. <br>
만일 코드 중간에서 해야할 경우, `getsockname` 함수를 통해 자기 주소를 알 수 있습니다. <br> 
* UDP 는 상대방이라는 개념이 없으므로, 보낼 때마다 `sendto()` 로 계속 바꿔 보낼 수 있습니다. <br>
반면에 TCP는 연결이 끝났으면 상대방이라는 것이 존재합니다. 그러므로 `getpeername()` 함수로 상대방 주소를 얻을 수 있습니다.
