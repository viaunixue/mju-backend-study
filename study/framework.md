# Web 기반 Framework

## 서버 구현 방식 가이드라인
![guide-line](/asset/img/guide_line.png)

## Web Server

* **HTTP 프로토콜을 구현하는 TCP Socket 서버** <br>
ex) Apache, Nginx, Python의 http.server 모듈, Node.js의 http 모듈

* `역할 1` : 정적인(static) 데이터(= 주로 파일)을 전송하는 역할

* `역할 2` : 동적인(dynamic) 데이터의 경우
    * HTTP 요청 해석하여 동적 데이터를 생성할 수 있는 대상 (application server)에 전달합니다.
    * 생성된 동적 데이터를 HTTP 응답으로 바꾸어 클라이언트에 전달합니다.

### Server 구조
![basic_server](/asset/img/server_1.png)

### Web Server 구조
![basic_server2](/asset/img/server_2.png)

### 동적 데이터 생성을 위한 Web Service 아키텍쳐
![basic_server2](/asset/img/server_3.png)

## CDN (Content Delivery Network)

* 정적인 데이터 배포를 하는 경우 `HTTP 서버` 기능은 동일하고, `응용`에 따라 달라지는 것은 오직 `배포할 데이터`입니다. (주로 파일을 배포) <br>
&rarr; 따라서 직접 웹 서버를 운용하는 대신 **데이터 배포**를 `대신` 해주는 것이 가능합니다.

### 이런 서비스가 CDN 입니다.

* 즉, 고객(=개발사)이 배포하고 싶은 데이터를, 자신들이 운용하는 HTTP 서버들을 이용해 유저에게 배포
* 대개 지역별로 수 많은 서버를 운용하며, 이 파일들을 이 서버들에 복제
* CDN 의 웹 서버가 원본 데이터를 최초로 1회 받아간 이후 이를 `cache` 합니다. <br>

#### 그 때문에 CDN은 web cache 또는 proxy 라고도 불립니다.

```
아마존에서 제공하는 CDN 서비스의 이름은 CloudFront 입니다. 왜 용어를 이렇게 지었을까?
-> IP를 보고 해당 지역에 가까운 웹 서버에서 데이터를 전해주는 의미에서 착안
CDN 은 Content Delivery Network 라고 불리우는데 왜 Network를 붙였을까?
인터넷 위에 동작하는 어떤 기능적인 부분에서 자기 서버들끼리 특정 목적으로 연결한 후 어떤 Content를 전달한다는 의미의 Network
```

### CDN 동작
* 직접 파일을 배포할 때 (`https://myservice.mju.ac.kr/funfile.mp4`)
![cdn_1](/asset/img/cdn_1.png)

* CDN을 통해 배포할 때 (`https://www.akami.com/myservice/funfile.mp4`)
![cdn_1](/asset/img/cdn_2.png)

### CDN 장단점
* 장점
1. 원본 서버의 부하 감소
    * 복제된 수 많은 서버에서 cache 된 파일을 전송하므로 원본 서버 부하 감소
    * 일정 부분 Dos 공격 방어에 기여하기도 함

2. 빠른 전송
    * Caching 에 따른 효과
    * IP 등의 정보로부터 유저의 위치에 따라 cache가 선택됨
        * 참고: IP 주소가 100% 정확한 지역 정보는 아니지만, 상당히 신뢰도가 높음

* 단점 : Cache 때문에 데이터가 갱신되더라도 오래된 정보가 전송될 수 있음 <br>
&rarr; 해결방법? Expire Time을 만들어 일정 시간이 지나면 다시 받아올 수 있게끔 함

## Framework

* 사전적 의미 : 뼈대, 틀
* 소프트웨어에서의 의미
    * SW를 개발함에 있어서 미리 만들어 둔 틀
    * 즉, 개발자는 개발하는 SW의 로직에 집중하게 하고 그 밖에 부수적인 것들을 제공하는 역할을 함

* 이점 : `높은 생산성` (= 빠른 개발 속도 및 유지의 편의성)

* 라이브러리 (library)와의 차이
    * 라이브러리는 그걸 사용하는 개발자가 라이브러리 코드를 호출하는 방식입니다.
    * 프레임워크는 `필요 시` 개발자가 만든 코드를 호출하는 방식입니다. <br>
    대개 프레임워크는 `이벤트 기반`으로 만들어져서 이벤트 발생 시 개발자 코드를 호출합니다.

### Framework에 사용자 코드 추가
* Framework는 대부분 `이벤트 방식`으로 동작합니다.
    * 어떤 사건이 발생하는 것을 이벤트로 정의 <br>  
    &rarr; 새로운 연결, 기존 연결이 끊김, 연결로부터 패킷이 들어옴 etc.

    * 각 이벤트에 대해 사용자 코드를 등록하는 방법을 제공합니다. (이때 이벤트 때 호출되는 사용자 코드를 `이벤트 핸들러`라고 칭함) <br>
    &rarr; 핸들러를 등록하는 방식, 이벤트 핸들러를 호출하는 방식, 패킷 핸들러를 등록하는 방식 etc.

### Application Server Frameworks
* Python Flask, Django : URL로 어떤 요청 수락 &rarr; 그 이벤트를 등록하는 방식으로 동작
* Java Spring (Boot) : Web Server 기능까지 다 포함 가능
* Node.js Express

&rarr; REST API 방식으로 하게 되면 URL 형식으로 들어오는데, 특정 URL에 어떤 메소드 호출이 있는지? <br> 이 함수, 이 핸들러를 호출해주는 방식으로 동작합니다.

### Application Server Framworks rlsmdemf
* 웹 서비스 아키텍처 상으로 다음 두 부분으로 이루어집니다.
    * Web Server : 소켓 + 작업큐 + 워커 (멀티쓰레드 / 멀티프로세스)
    * Application Server : HTTP 프로토콜 처리 + 동적 데이터 생성을 위한 Logic 처리

* 따라서 서비스 구현에 필요한 기능들은 다음으로 나열할 수 있습니다.
    * TCP Socket 처리
    * 작업큐
    * 워커 (멀티쓰레드 / 멀티프로세스)
    * HTTP 프로토콜로 된 request 인식
    * 동적 데이터 생성
    * 동적 데이터 포함하는 HTTP 프로토콜로 된 response 생성

* Framework는 이 필요 기능들 중 전부 혹은 일부를 제공합니다.

#### Python Flask 
* 주로 HTTP 프로토콜 처리 및 동적 데이터 처리 관련 부분
 <br> (참고: web server 부분도 있으나 production 목적이 아니라 development 목적)

#### Java Spring Boot, Node.js Express
* 전체 기능들을 지원할 수 있음

