# I/O Multiplexing 과 Non-blocking I/O

## I/O Multiplexing

**Multiplexing은 I/O를 묶어서 처리한다는 의미입니다.** <br>
```
socket descriptor와 file descriptor 처리 방식은 동일합니다. 
따라서 file 또한 I/O multiplexing 이라고 말합니다.
예로 만일 log 파일이 프로그램 10곳에서 계속 추가되고 있는데 
프로그램이 10개를 동시에 읽으면서 합치는 역할을 하는 프로그램을 만들겠다 합니다.
```
```
I/O multiplexing 을 이용하면, 어느 한쪽에서 읽는 것을 blocking 되어서 기다리는 게 아니라 
10개 파일에 대해서도 데이터가 있을 때만 깨어나서 처리할 수 있게끔 하는 것입니다.
```
### 서버의 복수 동시 접속 처리

* 서버는 동시에 여러 클라이언트를 서비스 할 수 있어야 합니다. <br>
&rarr; 여러 복수 개의 `Active Socket`에서 recv()/send() 를 할 수 있어야 됨을 의미합니다. <br>
&rarr; 동시에 `Passive Socket`에서 새로 들어오는 연결을 accept() 해야 합니다.
* 하지만 지금까지 다룬 `accept()/connect()/recv()/send()`는 `blocking` 됩니다.
    ```
    server 는 실행하면 새 연결을 accept() 하기 위해 기다립니다
    client 는 connect() 완료를 기다립니다
    server, client 는 아무 데이터도 오지 않았다면 recv() 는 기다립니다
    server, client 내부 버퍼 상태에 따라 send() 는 순간 정지할 수 있습니다
    ```
 * 다음 3 가지 방법을 통해 동시 접속 처리를 할 수 있습니다.
 1. **멀티 쓰레딩** <br>
 &rarr; 소켓 별로 처리 스레드를 두게 합니다. <br>
 &rarr; 단점으로 무거운 자원인 스레드 개수가 많아져 성능 문제가 발생합니다. 
 2. **Non-blocking I/O** <br>
 &rarr; 소켓은 blocking 이 기본값입니다. 이를 `non-blocking`으로 변경하는 것입니다. <br>
 &rarr; `nonblocking` 이라고 하더라도 주기적으로 함수를 호출해주어야 합니다. <br>
        **`만일 클라이언트가 1만개라면 소켓 1만개에 대하여 함수를 호출해야 합니다.`**
 3. **I/O multiplexing** <br>
 &rarr; 소켓의 읽기/쓰기/에러 이벤트를 커널로부터 통지받아 이벤트가 있는 경우만 함수를 호출합니다.

### select 함수
&rarr; POSIX 에 정의되어 있는 가장 기초가 되는 함수입니다. (Linux, Window 모두 사용 가능)
```c++
int select(
    int nfds,               // 제일 큰 소켓 번호에 1 더한 숫자
    fd_set *readfds,        // 읽기 이벤트 체크할 소켓 집합 및 그 결과
    fd_set *writefds,       // 쓰기 이벤트 체크할 소켓 집합 및 그 결과
    fd_set *exceptfds,      // 예외 이벤트 체크할 소켓 집합 및 그 결과
    const timeval *timeout  // 이벤트 기다릴 시간 값 
);
```

### select 이벤트 종류

<img width="830" alt="스크린샷 2023-10-15 18 50 11" src="https://github.com/mjubackend/io_multiplexing/assets/77084379/f0a4308d-7afc-45b4-be55-225a9fc9bf5d">

```c++
fd_set rset, wset;
FD_ZERO(&rset);
FD_ZERO(&wset);

select(&rset, &wset, NULL, NULL);

// listen 중인 passive socket 에 새 입력이 있는지 여부는 읽기 이벤트 입니다.
FD_SET(passiveSock, &rset);

// Active socket 에서 데이터가 들어왔는지 여부는 읽기 이벤트 입니다.
FD_SET(activeSock, &rset);

// 소켓에 데이터를 쓸 수 있는지 여부는 쓰기 이벤트 입니다.
FD_SET(activeSock, &wset);
```
> 보통 소켓을 다룰 때는 읽기 정도만 해도 프로그램에 큰 문제는 없습니다. <br>

> 읽기 이벤트 할 때는 OS 내에 버퍼가 비었는지 확인 할 때입니다.

### select() 함수의 fd_set
 <img width="597" alt="스크린샷 2023-10-15 18 51 41" src="https://github.com/mjubackend/io_multiplexing/assets/77084379/5796d2a8-5fa1-443b-8dec-dc0111af14b0">

### select() 의 timeout 시간
```c++
// NULL 을 지정하면 어떤 이벤트든 발생할 때까지 blocking 합니다.
select(&rset, &wset, &eset, NULL);
```

```c++
// 그 외에는 timeval 구조체 지정 값에 따라 다릅니다.
struct timeval timeout;
timeout.tv_sec = 0;     // 0 second
timeout.tv_usec = 100;  // 0 micro second
// 100 micro second 는 0.0001 Second 입니다.

select(&rset, &wset, &eset, &tv)
```

### select() 의 한계
* 다룰 수 있는 소켓 숫자가 제한되어 있습니다.
    * `fd_set`은 소켓들의 이벤트 여부를 나타내기 위해 배열을 사용합니다.
    * Window 에서는 64개, Linux 에서는 1024개 정도 지원합니다.
        ```
        만일 socket descriptor가 1024를 넘어가면 select 사용이 불가능합니다.
        ulimit -n 명령어는 TCP 서버를 다룰 때는 반드시 알아야 합니다
        ```
* 이벤트 발생 여부 체크를 위한 loop
    * 여전히 각 소켓에 대하여 loop 로 체크해 주어야 합니다.

### select() 의 대안
* 이벤트 발생한 것을 배열에서 빼는 방식이 아니라 list 형태로 제공하는 것을 사용합니다. <br>
&rarr; Linux : `epoll`
* I/O를 비동기적으로 처리하고 결과를 알려주는 방식을 이용합니다. <br>
&rarr; Window: `IOCP`

### [tcp_server2.cpp](https://github.com/almond0115/mju-backend-dev/blob/main/study/IO/io_multiplexing/tcp_server2.cpp) 
&rarr; select() 를 이용한 다중 클라이언트 I/O multiplexing
#### 코드 분석
```c++
set<int> clientSocks;
```
&rarr; 클라이언트가 연결이 되면 accept() 호출을 해야 하는데, `호출된 각각의 client 소켓들`을 set을 사용한 `clientSocks`에 저장합니다. <br>
&rarr; 일반적인 경우, `clientSocks` 에 들어온 순서대로 데이터가 없으면 그냥 blocking 이 됩니다.
```
만약 처음에 연결한 Client의 descriptor가 10 이라고 할 때, 
그 다음에 accept한 descriptor가 11이라고 한다면 loop를 돌면서
10번에 데이터 유무를 기다리는데 데이터가 없다면 recv가 blocking이 되고, 
11번의 데이터가 있더라도 10번에서 blocking 되어 11번은 처리가 안됩니다.
```
```
select() : 10번, 11번에 대하여 순서대로 기다리지 말고, 둘 중 아무거나 데이터가 있으면 알려주는 함수입니다.
```
<br>

```c++
fd_set rset;
FD_ZERO(&rset); 

FD_SET(serverSock, &rset); 
int maxFd = serverSock; 
for (int clientSock : clientSocks) {
    FD_SET(clientSock, &rset);
    if (clientSock > maxFd){
        maxFd = clientSock;
    }
}
```
&rarr; `&rset` 에 `serverSock`(새로운 값)과 `clientSock`(기존 값) 모두 저장하고 `maxFd`를 업데이트 합니다. <br>

```c++
int numReady = select(maxFd + 1, &rset, NULL, NULL, NULL);
if (numReady < 0){
    cerr << "select() failed: " << strerror(errno) << endl;
    continue;
} else if (numReady == 0) {
    continue;
}
```
&rarr; `maxFd`보다 1 큰 값을 넣고 `timeout`값을 `NULL`로 저장합니다. <br>
&rarr; `&rset`에 지정되어 있는 socket descriptor 중 어떤 것이라도 `read 가능한 상황`이 되면 반환합니다. <br>
&rarr; timeout 값이 `NULL` 이므로 무한정 기다립니다. 
<br>

```c++
if (FD_ISSET(serverSock, &rset)){
    memset(&sin, 0, sizeof(sin));
    unsigned int sin_len = sizeof(sin);
    int clientSock = accept(serverSock, (struct sockaddr *) &sin, &sin_len);
    if (clientSock < 0){
        cerr << "accept() failed: " << strerror(errno) << endl;
    } else {
        clientSocks.insert(clientSock);
    }
}
```
&rarr; `FD_ISSET`을 통해 `&rset`에 `serverSock` 이 있는지 물어봅니다. <br>
&rarr; 있다면 새로운 연결이 있는 것이므로 이 순간에 `accept()` 함수를 호출합니다. <br>
&rarr; 이미 새 연결이 있다는 것을 알고 들어왔기 때문에 더 이상 `blocking` 하지 않고 바로 처리합니다. <br>
&rarr; 새 `clientSock`를 만들고 전체 리스트에 `insert()` 합니다.

```c++
set<int> willClose;
for (int clientSock : clientSocks) {
    if (!FD_ISSET(clientSock, &rset)){
        continue;
    }

    ...

}
```
&rarr; 각 연결에 대해서 clientSock도 있는지 없는지 확인합니다. <br>
&rarr; **만약 데이터가 없다면 무시하고 있다면 이미 데이터가 있다는 것을 알고 들어왔기 때문에 `recv()`를 해도 `blocking`이 되지 않습니다.** <br>

```c++
set<int> willClose;
for (int clientSock : clientSocks) {

    ...

    char buf[65536];
    int numRecv = recv(clientSock, buf, sizeof(buf), 0);
    if (numRecv == 0){
        cout << "Socket closed: " << clientSock << endl;
        willClose.insert(clientSock);
    } else if (numRecv < 0){
        cerr << "recv() failed: " << strerror(errno) << ", clientSock: " << clientSock << endl;
        willClose.insert(clientSock);
    } else {
        cout << "Received: " << numRecv << " bytes, clientSock " << clientSock << endl;

        int offset = 0;
        while (offset < numRecv) {
            int numSend = send(clientSock, buf + offset, numRecv - offset, 0);
            if (numSend < 0){
                cerr << "send() failed: " << strerror(errno) << ", clientSock: " << clientSock << endl;
                willClose.insert(clientSock);
                break;
            } else {
                cout << "Sent: " << numSend << ", clientSock: " << clientSock << endl;
                offset += numSend; 
            }
        }
    }
}
```
&rarr; 일반적으로 `send()`, `recv()` 한 만큼 offset 을 계산하면서 loop을 돕니다. <br>
&rarr; 해당 코드 `recv()` 함수에 offset 코드가 반영되지 않은 이유는 몇 byte를 받아야 하는지 모르는데, 길이를 앞에 인코딩 하거나 제대로 읽었는지를 반복적으로 체크하는 코드를 구현해야 합니다.
### [tcp_server3.cpp](https://github.com/almond0115/mju-backend-dev/blob/main/study/IO/io_multiplexing/tcp_server3.cpp) 
&rarr; select() 에 timeout 값을 지정하여 idle 클라이언트 삭제

## Non-blocking I/O

### Blocking Call
* 프로세스가 어떤 작업을 위해 OS에 기능을 요청했는데, (= System call 호출) <br>
OS가 바로 이를 처리할 수 없을 때 thread를 "sleep" 또는 "blocked" 상태로 변경하게 됩니다.
* OS 는 요청된 작업을 다 처리한 후 thread를 깨웁니다.
* Thread를 이렇게 sleep 상태로 만들 수 있는 함수를 `blocking 함수`라고 합니다. <br>
그런 상태로 만들 수 있는 I/O 작업을 `blocking I/O` 라고 합니다.
* Blocking I/O는 thread를 sleep 시키므로 CPU 를 소모하지 않습니다. <br>
**대신 그 순간 thread는 아무 작업도 할 수 없게 되어 `throughput(처리율)` 은 떨어집니다.**

### Non-Blocking Call
* **thread를 sleep 시키지 않는 함수**를 `Non-blocking 함수`라고 합니다. <br>
* 이런 I/O 작업을 `Non-blocking I/O`라고 부릅니다. <br>
&rarr; **`이 때 단순히 "시간이 오래 걸리는지 아닌지"로 판단하는 것이 아니라 thread를 sleep 시키는지가 중요합니다. `**
* 설령 해당 함수가 내부에서 복잡하게 연산을 하여 시간이 오래 걸리더라도 thread 는 sleep 하지 않았으므로, <br>
blocking 함수가 아니라 Non-blocking 함수입니다.
* Non-blocking I/O는 CPU 사용률을 올리게 됩니다. <br>
&rarr; CPU를 무조건 많이 쓴다고 나쁜게 아니라 의미 있는 일을 하는지 아닌지가 중요합니다.

### Polling & Busy Wait
* Non-blocking 함수 중 내부적으로 loop을 돌면서 작업이 끝났는지를 체크하는 함수들이 있습니다.
    ```
    만일 1000개의 descriptor 가 있다고 한다면, 
    I/O multiplexing 같은 경우 1000개 중에 read 가 발생하면 깨워줍니다.
    반면 Non-blocking은 1000개 모두 각각 데이터 유무를 물어보고 blocking 되지 않도록 막아줍니다.
    예를 들어 하나 체크하는데 1 m/s가 걸린다고 한다면  1000개를 돌리면 1s 가 걸립니다.
    polling은 loop을 돌면서 있는지 없는지 물어보는 것 입니다.
    ```
    &rarr; loop 량이 많아질 수록 느려질 수 밖에 없습니다. <br>

* 이 떄 `작업이 끝났는지 체크`하는 것을 `polling`이라고 부릅니다.
* 그리고 이렇게 loop을 돌면서 기다리는 것을 CPU를 사용하면서 기다린다고 하여 `busy wait`이라고 부릅니다.

* **`Polling이 blocking 작업일 경우 loop을 돌면서 기다리는 것도 blocking입니다.`** <br>
**`따라서 polling 자체는 busy wait임을 의미하지는 않습니다.`** <br>
**`Non-blocking polling으로 loop을 돌면서 기다리는 경우가 busy wait이 됩니다.`** <br>
&rarr; CPU 소모 여부가 중요합니다.

### 적정 CPU 사용률
* CPU 사용률은 어떤 작업을 하고 있는지로 판단하는 것이 적절합니다.
* CPU 사용률이 높더라도 의미 있는 작업을 하면서 throughput 이 높다면 적절합니다. <br> 의미 없이 busy wait 하는 것은 부적절합니다.
* 반대로 busy wait 없이 처리할 작업이 없어서 CPU 사용률이 낮은 건 좋지만, <br> 처리할 작업이 있는데도 계속 thread 가 sleep 되는 것은 **throughput 관점에서 `서버가 밀린다` 라는 느낌**을 주게 됩니다.

#### 따라서 코드 분석을 통해 의미 없이 CPU를 낭비하지 않고, 의미 없이 sleep하지 않게 구현하는 것이 중요합니다.

#### 서버는 통상 실무적으로 대략 `50% ~ 70%` CPU 사용률을 목표로 합니다.

(명령어 `top`을 통해 다음 화면을 확인할 수 있습니다) <br>

<img width="659" alt="스크린샷 2023-10-15 21 07 14" src="https://github.com/mjubackend/io_multiplexing/assets/77084379/2c7d1370-ade7-4005-91c4-27e608c1aa41">
<br>

* `%CPU`는 한 코어 개수를 기준으로 나옵니다.

```
만약 코어 2개라고 한다면 50% 해당하는 프로세스는 100%가 나옵니다
```

### [lab5_test10_with_nonblocking.cpp](https://github.com/almond0115/mju-backend-dev/blob/main/study/IO/non_blocking/lab5_test10_with_nonblocking.cpp) 
&rarr; TCP server를 non-blocking socket 으로 변경했습니다. <br>
&rarr; test7.cpp의 send() 함수 앞에 sleep(3); 코드를 두면 동작 확인이 가능합니다.

<img width="955" alt="스크린샷 2023-10-15 21 49 27" src="https://github.com/mjubackend/io_multiplexing/assets/77084379/a2918618-6553-4cc4-b5e4-f14f9eb7375e">

* while 문 형태로 수정하여 Eagain이면 반복해서 trying 하도록 코드를 수정하면 정상 작동합니다. (하지만 그런 방식은 polling)