# lab 3 - UDP Socket Programming

## `Test #1` 
### **Opaque Handle로서의 Socket Descriptor** <br>
```c++
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

int main() {
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	cout << "Socket ID: " << s << endl;

	close(s);
	return 0;
}	
// result
// Socket ID: 3
```
* SOCK_DGRAM 안 세부 프로토콜 IP 위의 DGRAM은 UDP 밖에 없으므로 그냥 0을 써도 됩니다.
* Socket descriptor (Socket ID)는 숫자입니다. <br> 어떤 의미인지 모르지만 그 소켓을 구분할 목적으로 이후 소켓 함수와 같이 전달합니다.

## `Test #2` 
### **재사용 되는 Socket Descriptor 값** <br>
```c++
#include <sys/socket.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
using namespace std;

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cout << "Socket ID : " << s << endl;
    close(s);

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cout << "Socket ID : " << s << endl;
    close(s);
    return 0;
}
// result
// Socket ID: 3
// Socket ID: 3
```
* Socket ID 는 Socket descriptor 를 의미한다.
* 위 출력 결과 값을 통해 Socket discriptor 값이 재사용 된다는 것을 알 수 있다.
* 이 떄, **값이 재사용 된다고 해서 `같은 소켓`이라는 뜻이 아니다.**
<br> 우연히 값이 같아도 둘은 **`완전히 다른 소켓`** 이다.
* Socket discriptor는 Process 별로 구분된다. 각각의 Process 안에서 3 이라는 숫자는 다른 소켓을 의미한다. 
> 따라서 socket()으로 얻어낸 것은 `오직 1회만 close()`해야 한다.

* close()를 안하면 &rarr; 소켓 자원 누수
* close() 를 2회 이상 하면 &rarr; 우연히 같은 번호를 쓰게 된 소켓을 닫아버릴 수 있음
<br> ex) 다른 쓰레드에서 얻은 소켓을 닫아버림
* Memory alloc/free 와 같음  

## `Test #3` 
### UDP 소켓으로 데이터 보내기 <br>
 
```c++
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    string buf = "Hello World from test 3";

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM);
    sin.sin_addr.s_addr = inet_addr(IP_NUM);

    int numBytes = sendto(s, buf.c_str(), buf.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    cout << "Sent : " << numBytes << endl;

    close(s);
    return 0;
}
// result
// Sent : 23
```
* Socket API 는 여러 주소 체계 지원을 가정하고 있습니다.
* 주소 요소 중 공통 부분을 **`struct sockaddr`** 에 넣고 각 주소 체계 별로 별도 struct 지정
<br> IPv4 &rarr; **struct sockaddr_in**, IPv6 &rarr; **sockaddr_in6**
* sendto() 함수는 struct sockaddr * 을 매개변수로 받으므로, struct sockaddr_in을  강제 형변환을 합니다.
> sendto() : 네트워크로 보냈으면 보낸 Byte 수 반환 (상대가 받았다는 뜻이 아님을 주의)
> <br> recvfrom() : 데이터를 읽었으면 읽은 Byte 수 반환 / 읽지 못했으면 -1 반환 
<br> &rarr; N개 클라이언트를 서비스하더라도 서버 쪽 UDP 소켓은 단 1개만 사용됨을 의미합니다.
<br> &rarr; 서버에서 관리해야 되는 state도 없고 그때 그때 들어오는 UDP 발신자만 알려주면 되므로 부하가 적습니다.

## `Test #4` 
### **UDP로 데이터 받기** <br>
```c++
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"
#define BUF2_BYTES      65535

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    string buf = "Hello World from test 4";

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM); 
    sin.sin_addr.s_addr = inet_addr(IP_NUM);

    int numBytes = sendto(s, buf.c_str(), buf.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    cout << "Sent : " << numBytes << endl;

    char buf2[BUF2_BYTES];
    memset(&sin, 0, sizeof(sin));
    socklen_t sin_size = sizeof(sin);
    numBytes = recvfrom(s, buf2, sizeof(buf2), 0, (struct sockaddr *) &sin, &sin_size);

    cout << "Received: " << numBytes << endl;
    cout << "From " << inet_ntoa(sin.sin_addr) << endl;

    close(s);
    return 0;
}
// result
// Sent : 23
// Received : 23
// From : 127.0.0.1
```
**`sendto()` 를 통해 보낸 값을 "127.0.0.1" loop back을 통해 받을 수 있도록 이후 코드에 recvfrom () 을 추가하여 구현해보았습니다.**

## `Test #5` 
### 입력 받은 문자열을 서버로 전송

```c++
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"
#define BUF2_BYTES      65535

int main(){
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    string buf;
    while(true){
        cout << "문자열을 입력하세요 (Ctrl+D로 종료) : ";
        getline(cin, buf);

        if (cin.eof()){
            cout << endl;
            break;
        }

        if(buf.empty()){    
            cout << "빈 문자열은 전송되지 않습니다." << endl;
            continue;
        }

        struct sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(PORT_NUM);
        sin.sin_addr.s_addr = inet_addr(IP_NUM);

        int numBytes = sendto(s, buf.c_str(), buf.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
        cout << "Sent : " << numBytes << endl;

        char buf2[BUF2_BYTES];
        memset(&sin, 0, sizeof(sin));
        socklen_t sin_size = sizeof(sin);
        numBytes = recvfrom(s, buf2, sizeof(buf2), 0, (struct sockaddr *) &sin, &sin_size);

        cout << "Received : " << numBytes << endl;
        cout << "From " << inet_ntoa(sin.sin_addr) << endl;
        cout << buf << endl;
    }

    close(s);
    return 0;
}
// result 
// 문자열을 입력하세요 (Ctrl+D로 종료) : Hello
// Sent : 5
// Received : 5
// From 127.0.0.1
// Hello
// 문자열을 입력하세요 (Ctrl+D로 종료) : Hi    
// Sent : 2
// Received : 2
// From 127.0.0.1
// Hi
// ...
```
**`cin 또는 getline()` 등 으로부터 읽은 문자열을 서버로 전송하는 프로그램을 구현해보았습니다**

## `Test #6` 
### 내 Socket 이 쓰는 Port 번호
```c++
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"
#define BUF2_NUM        65535

int main(){
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    string buf = "Hello World from test 6";

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM);
    sin.sin_addr.s_addr = inet_addr(IP_NUM);

    int numBytes = sendto(s, buf.c_str(), buf.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    cout << "Sent : " << numBytes << endl;

    char buf2[BUF2_NUM];
    memset(&sin, 0, sizeof(sin));
    socklen_t sin_size = sizeof(sin);
    numBytes = recvfrom(s, buf2, sizeof(buf2), 0, (struct sockaddr *) &sin, &sin_size);

    cout << "Received: " << numBytes << endl;
    cout << "From " << inet_ntoa(sin.sin_addr) << endl;

    // memset(&sin, 0, sizeof(sin));
    sin_size = sizeof(sin);
    int result = getsockname(s, (struct sockaddr *) &sin, &sin_size);

    if(result == 0) {
        cout << "My addr: " << inet_ntoa(sin.sin_addr) << endl;
        cout << "My port: " << ntohs(sin.sin_port) << endl;
    }

    close(s);
}
// result
// Sent : 23
// Received: 23
// From 127.0.0.1
// My addr: 0.0.0.0
// My port: 47129
```
* UDP header를 설명할 때 송신자, 수신자에 대해서 각각 주소 요소인 port를 써야 한다고 했습니다.<br>
앞의 코드의 예에서 `sendto()`를 통해서 상대방 port는 기재했습니다. <br>
그럼 내 port 번호는 어떻게 된 것일까요? <br>
    > &rarr; UDP 데이터그램 헤더에는 송신자와 수신자 IP 주소와 port 번호가 포함됩니다.<br>
    송신자의 port 번호는 client 측에서 선택됩니다. <br> 
    client가 server에 데이터를 보낼 때 client는 임의 port 번호를 선택하여 사용하게 됩니다. <br>
    이 port 번호는 대개 동적으로 할당되며 OS가 관리합니다. <br>
    수신자의 port 번호는 server에서 바인딩한 번호입니다. server가 특정 port 번호에서 수신을 대기하는 경우,<br> 해당 port 번호로 오는 모든 client 연결을 수신합니다. server는 `bind()`를 통해 특정 포트 번호를 선택하고 <br> 해당 port 번호를 들어오는 데이터를 수신합니다. 따라서 UDP server의 port 번호는 server가 수신을 대기하는 port 번호로, <br> client의 port 번호는 client 소켓이 임의로 선택한 port 번호로 각각 다르게 설정됩니다. <br>
    client의 port 번호는 server 에서 데이터를 수신하는 port 번호로 설정하며, server의 port 번호는 <br>
    `bind()`를 통해 지정합니다. client는 일반적으로 랜덤 port 번호를 사용하고, server 는 특정 port 번호를 사용합니다.


## `Test #7` 
### 내 Socket 이 쓸 Port 번호 지정
```c++
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"
#define BUF2_NUM        65535

int main(){
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cout << "Socket Creation Error" << endl;
        return 1;
    }

    string buf = "Hello World from test 7";

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM);
    sin.sin_addr.s_addr = INADDR_ANY;
    // if(bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0){
    //     cerr << strerror(errno) << endl;
    //     return 1; 
    // } 
    // -> test8 UDP 서버에서 binding 처리

    int numBytes = sendto(s, buf.c_str(), buf.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    if(numBytes < 0){
        cerr << "Send error : " << strerror(errno) << endl;
        return 1;
    }

    char buf2[BUF2_NUM];
    memset(&sin, 0, sizeof(sin));
    socklen_t sin_size = sizeof(sin);
    numBytes = recvfrom(s, buf2, sizeof(buf2), 0, (struct sockaddr *) &sin, &sin_size);
    if(numBytes < 0){
        cerr << "Recvfrom error : " << strerror(errno) << endl;
        return 1;
    }

    cout << "Received: " << numBytes << endl;
    cout << "From: " << inet_ntoa(sin.sin_addr) << endl;

    memset(&sin, 0, sizeof(sin));
    sin_size = sizeof(sin);
    int result = getsockname(s, (struct sockaddr *) &sin, &sin_size);

    if(result == 0) {
        cout << "My addr: " << inet_ntoa(sin.sin_addr) << endl;
        cout << "My port: " << ntohs(sin.sin_port) << endl;
    }

    close(s);  
    return 0;
}
// Received: 23
// From: 127.0.0.1
// My addr: 0.0.0.0
// My port: 60639
```
* **직접 `내 서브넷 숫자`를 사용하여 지정된 Port를 설정해보았습니다.**
* **`bind()`** 는 client, server 관계 없이 할 수 있지만, 통상적으로 서버의 port는 <br>
미리 정해져야 client 가 요청을 보낼 수 있으므로 대개 **서버 측에서 `bind()`를 호출합니다.**
* 그러나 서버가 자기 IP 주소와 Port를 `Redis/Zookeeper` 등 특수한 목적의 DB에 등록하게끔 하는 `service directory`를 구현한 경우 <br>
`bind()` 대신 `getsockname()`을 한 결과를 `Redis/Zookeeper`에 저장할 수 있습니다. 
<br> (이 경우 `client`는 `Redis/Zookeeper`에서 서버의 IP, Port를 알아내서 `sendto()` 해야 합니다.)

## `Test #8` 
### UDP 서버 만들기

```c++
#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"
#define BUF2_NUM        65535

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT_NUM);

    if(bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        cerr << "Bind error: " << strerror(errno) << endl;
        close(s);
        return 1;
    }

    while(true){
        char buf2[BUF2_NUM];
        memset(buf2, 0, sizeof(buf2));
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int receivedBytes = recvfrom(s, buf2, sizeof(buf2), 0, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if(receivedBytes < 0){
            cerr << "Recvfrom error: " << strerror(errno) << endl;
            continue;
        } else if(receivedBytes == 0){
            cout << "빈 문자열입니다. "<< endl;
            continue;
        }

        cout << "Received: " << receivedBytes << " bytes" << endl;
        cout << "Received Message: " << buf2 << endl;

        int numBytes = sendto(s, buf2, receivedBytes, 0, (struct sockaddr *) &clientAddr, clientAddrLen);
        if(numBytes < 0){
            cerr << "Sendto error: " << strerror(errno) << endl;
            continue;
        }

        cout << "Echoed: " << numBytes << " bytes" << endl;
        cout << "Echoed Message: " << buf2 << endl;
    }
    close(s);

    return 0;
}
```
* **UDP 서버인 `TEST8` 을 만들면서, TEST3 ~ TEST7 과 호환되도록 Port를 설정하였습니다.**

* > `sendto()` 에서 전송 요청한 데이터 중 일부만 전송되는 경우는 없습니다. 
* > `recvfrom()` 에서 일부만 수신하는 경우는 없습니다.
* > `UDP 통신 방식`은 상대가 제대로 수신했는지 확인할 방법이 없습니다.
* > `TCP / UDP Port` 번호는 완전히 다른 의미로 받는 트래픽도 완전히 달라 공존할 수 있습니다.
* > `bind()` 대상은 <IP 주소, L4 Protocol 타입, Port 번호> 로 결정됩니다. ( 이 셋중 하나라도 다르면 서로 다른 바인딩이 됩니다.)