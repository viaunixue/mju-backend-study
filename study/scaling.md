# Scaling

## Scaling 의 두가지 방법

### Scale-up
&rarr; Worker 수 변경 없이 더 큰 capacitiy 리소스로 교체하는 방식
> 리소스 : vCPU, 메모리, NIC, 네트워크 bandwidth, storage etc.

> 수직적 확장 (vertical scaling) 이라고도 함

![scale_up](/asset/img/scale_up.png)

### Scale-out
&rarr; 단일 Worker의 capacity 변경 없이 worker 숫자를 늘려 전체 capacity 증가

> 수평적 확장(horizontal scaling) 이라고 함

![scale_out](/asset/img/scale_out.png)

### Scale-up 과 Scale-out 비교

||Scale-up|Scale-out|
|--|--|--|
| 확장 편의성 <br> 및 설계 난이도 | IP, OS 등 기존 환경을 <br> 유지하면서 capacity 증대가 가능 | 새로 추가되는 worker가 반영될 수 있도록 <br> 분산 처리 및 로드 밸런싱 아키텍쳐 설계 필요 |
| 관리 비용 | 관리해야 되는 worker 숫자 변경이 <br> 없으므로 관리 비용 변동 없음 | 관리해야 되는 worker 숫자 증가에 따른 <br> 관리 비용 증대 |
| 확장 비용 | 고사양으로 확장할 수록 <br> 비용이 기하급수적으로 증가함 | 동일 사양의 worker를 추가하는 것이므로 <br> 비용이 선형적으로 증가함 | 
| 확장 한계 | 리소스 별로 최대 확장 가능 용량 <br> 한계가 존재함 (예시: 최대 vCPU 개수 <br> 최대 메모리 용량, 최대 네트워크 bandwidth) | 이론상 한계 없음 |
| 장애 영향 | 전체 서비스 중단 발생 (Single point of failure) | 전체 서비스 중단이 아닌 처리 용량 감소에 <br> 따른 지연 발생 |
| 스케일링 따른 <br> 서비스 다운 타임 | 기존 worker의 사양을 변경하는 동안 <br> down time이 불가피 함 | 기존 서비스 영향 없이 worker를 준비할 수 <br> 있으므로 down time 거의 없음 <br> 단 새 worker가 기존 worker들에게 <br> 인식되는 동안의 지연은 발생 가능 |
| 적합한 응용 타입 | Stateful한 응용 | Stateless 한 응용 |

## 장애/서비스 중단 관련 용어

### Fail-over

장애가 발생했을 때 장애를 극복할 수 있는 대안 (즉, 이중화 시 다른 경로)

### Fail-safe

장애가 발생해도 장애 극복을 할 수 있는 상태 (즉, fail-over가 있는 상태)

### Fail-stop

장애가 발생했을 때 장애 극복을 하지 못하고 서비스 중단을 야기하는 상태 (즉, fail-over가 없는 상태)

### Single Point of Failure (SPOF; 단일 장애점)

서비스 구성 요소 중, 동작하지 않으면 전체 서비스 중단을 일으키게 되는 요소 <br>
즉 이중화 되어 있지 않은 구성 요소 또는 fail-stop 하는 구성 요소

### Availability (가용성)

서비스가 얼마나 죽지 않고 살아있는지에 대한 특성 <br>
* up time 비율로 계산 <br>
&rarr; 99.99% availability : 365 days x 24 hours x 99.99% = 8,759.124 시간 동안 살아있음 <br>

* 경우에 따라서는 연중 downtown 숫자로 나타내기도 함
&rarr; 99.99% availability : 연중 8,760 - 8,759.124 hours = 연중 52.6분 down time

### 고가용성 (high availability)

사전적 의미로 `높은 availability 숫자를 제공하는 서비스`를 의미하는데 실제적으로는 <br>
`어느 정도의 장애에 대해서는 Fail-over가 지원 됨`의 의미로 쓰임 (= 어느 정도의 장애에 대해서는 Fail-safe 함)

![fail_1](/asset/img/fail_1.png)

## Pay-As-You-Grow

* 미리 비용을 지불하는 대신 필요할 때( = 성장했을 때 ) 가 되면 비용을 지불한다는 의미

* 인프라 스트럭처에 초기부터 투자하는 대신 서비스가 성장함에 따라 인프라스트럭처를 늘리는 방식

* Scale-out 방식의 생태계에서 주로 언급되는 마케팅 캐치 프레이즈지만 <br>
Scale-up 역시 "서비스 성장 후 고사양 변경" 이 가능하므로 동일하게 적용됨

## Auto Scaling

* Scaling이 필요한 경우 이를 자동으로 수행하는 것 입니다.

* Scale-up은 scaling 과정 중 리소스 교체를 위해 down time 이 발생하므로, `현실적으로 auto-scaling은 scale-out을 의미`합니다.

* **리소스**가 scaling할 대상임에 유의해야 합니다. <br>
&rarr; Auto-scaling은 *서버를 확장한다* 라고 단순하게 생각할 수 있지만 보다 정확한 의미는 **`리소스를 확장한다`** 입니다. <br>
(리소스 : 가상 서버, 웹 서버 프로세스, Docker container, 마이크로 서비스, DB 등등)

* Scaling 이 발동 조건 : 리소스의 과부하 <br>
&rarr; 보통 request 숫자, CPU 사용량, I/O 요청 숫자 등으로 과부하 여부를 판단합니다.

## Manual Scaling

* 수작업으로 scaling 하는 것 입니다.

### Manual Scaling이 필요한 경우

* Auto Scaling에 의한 과도한 요금 걱정
* 설계상 새로운 자원 인식을 위해 down time이 발생하는 경우
* 명시적으로 down time 이 없더라도 scaling 타이밍을 직접 통제하고 싶을 때
* 심리적 불안감
* Stateful 한 구조 (= Scale-up 밖에 방법이 없으므로 down-time 이 발생)

## 추가/삭제 된 서버를 인식하는 방법

* 서버 그룹에 Scale-out 이벤트(= 추가/삭제)가 발생했을 때, 이 서버들과 통신하는 peer는 추가/삭제된 정보를 알아야 합니다.
    * 추가 된 서버를 모르면 그 서버에 요청을 보내지 못하므로 scale-out 의미가 없습니다.
    * 삭제된 서버를 모르면 그 서버에 계속 요청을 보내서 request time-out 이 발생합니다.

* 서버 추가/삭제를 인식 시키는 방식에는 다음과 같은 방법들이 존재합니다.
    * DNS round-robin 이용하는 방식
    * 로드 밸런서를 사용하는 방식
    * 설정 DB를 이용하는 방식

### DNS RR

* name resolution &rarr; 사람이 읽는 호스트 이름 : IP 주소

* DNS server &rarr; Name resolution 제공하는 서버

* DNS record types

    |||
    |--|--|
    | A record | 위의 호스트 이름 &rarr; IP 주소 정보 <br> (예 : www.mju.ac.kr &rarr; 202.30.111.50) |
    | CNAME record | 별명 (alias) 정보 <br> (예 : www.mju.ac.kr &rarr; mju.ac.kr) |
    | NS record | name server 정보 |
    | MX record | mail server 정보 |
    | TXT record | 임의의 문자열 정보 |

* DNS 서버에 같은 타입의 record가 여러 개 존재할 수 있음

* 그리고 하나의 key 에 여러 value 가 존재할 수 있음

#### 따라서 같은 호스트 이름의 A 레코드에 여러 IP 주소를 저장할 수 있음
&rarr; 이 때 Name resolution 하는 client 측에서는 같은 호스트 이름에 대해서 지정된 여러 IP 중 랜덤한 것을 선택할 수 있음

![nslookup](/asset/img/nslookup.png)

* 서버 역시 여러 IP 주소 중 순서대로 하나씩 응답해주고 <br>
클라이언트 역시 반환 받은 것이 여러 개 일 경우 하나 씩 순서대로 사용하므로 이를 `DNS round-robin`이라고 합니다.

* 이와 같은 속성을 이용하여 worker가 추가/삭제 되면 DNS server 상에 A record 에 이를 반영할 수 있습니다.

* 단점
    * DNS는 계층적이며, 각 계층에서 응답을 caching한 후 이를 활용합니다.
    * 따라서 DNS record 의 변경은 즉각적으로 반영되지 않을 수 있습니다.
    * 즉각적으로 반영되도록 하기 위해 cache 만료 시간에 해당하는 TTL(time-to-live)를 줄일 수 있습니다. <br> 
        <details>
        <summary> 이 경우 어떤 일이 발생할까요? </summary> 
            캐시는 오래된 정보를 갖고있게 되는게 단점인데, 이를 해결하기 위해 만료 시간을 줄이면<br> 실제로 캐시 성능이 크게 저하됩니다. 
            원래 리소스를 관리하는 입장에서는 그 부하를 그대로 다 받게 되는 것입니다. <br>
            주로 읽기 전용에 특화되어 있는 캐시를 쓴다고 한다면 아무데서나 캐시를 쓰는게 아니고, 읽기 전용일 경우에 캐시가 적합합니다.
        </details>

#### DNS를 바라보는 다른 시각

* Database 는 반드시 SQL 서버를 의미하지 않음

* 어떤 정보를 구조화하여 저장한 것은 모두 Database에 해당함

<br>

&rarr; 따라서 DNS record 역시 특정 도메인에 대한 주소 정보를 나타내므로 DNS는 일종의 주소 정보 Database로 볼 수 있음

그러면서 DNS가 cache 방식으로 동작하기 때문에, DNS는 caching 가능한 좀처럼 바뀌지 않는 read-only 정보를 저장하는 Database로 볼 수 있음

###  로드 밸런서

* 로드 밸런서는 대상 server pool 을 가지고 있습니다.

* 이 server의 서비스를 이용하는 client는 각 server의 존재를 알지 못한 채 로드밸런서에게 요청을 보냅니다.

* 로드 밸런서는 일정한 규칙대로 server를 선택하여 client의 요청을 전달합니다.

    ![load_balancer](/asset/img/load_balancer.png)

### 설정 DB 활용

*  활용 가능한 DB 
1. SQL server
2. Redis 로 "servers"라는 set에 각 서버 정보를 가지고 있는 Key 이름 저장
3. Apache Zookeeper
4. Message Queue 솔루션 중 multicast 기능 활용

* 통지 방식
1. Client에 push 하는 방식 <br>
&rarr; message Queue의 multicast, Zookeeper, Redis의 pub-sub 활용할 때

2. Client가 polling 하는 방식 <br>
&rarr; SQL server, pub-sub 없는 Redis 활용할 때 <br>
&rarr; Polling은 리소스 낭비를 유발하므로 바람직하지 않음. <br>
    이를 줄이기 위해 Polling 된 정보를 caching할 수 있으나 <br> 
    이 경우 오래된 cache 정보와의 데이터 불일치 문제를 고려해야 함

## Case Study : Apache Zookeeper

* `분산 coordination 서비스`
    * 분산 시스템 구현을 위한 계층적 key-value 저장소

* 응용 범위
    * 분산 시스템 내에서 설정(configuration) 정보의 공유
    * 분산 시스템 구현에 있어서 데이터 동기화 구현을 위한 빌딩 블록
    * 분산 시스템 내에서 directory service
    * 어떤 서비스를 제공하는 구성 요소가 여러 개 일 때 “IP:port” 정보 저장
    * 분산 시스템 내 master election 같은 서비스를 제공하는 IP:port 가 여러 개 있을 때 그 중 누가 master 이고 나머지는 slave 인지 선출

### 장점

* Fail-over 지원
    *  Zookeeper 서버는 여러 노드가 클러스터링 되어 동작하므로 <br> 
    그 중 한 node 에 장애가 발생하더라도 전체 zookeeper 서비스는 안정적으로 동작함

* 단순한 구조
    * File-system 과 유사한 단순한 계층 구조 형태로 key-value 를 저장함

* 고성능의 읽기 성능
    * 분산 시스템 내 데이터 중 읽기가 많이 발생하는 경우 고성능 보장 <br>
    (Zookeeper 서버가 read replica 로 동작함)
 
* 확장성
    * 서버를 추가함으로써 손쉽게 확장 가능

### 개념도

![zookeeper_1](/asset/img/zookeeper_1.png)

### 구조

ZK 는 file system 과 마찬가지의 계층적 구조를 가지며 각 node 가 namespace 역할을 함 <br>

> 어디까지나 논리적으로 그런 것일 뿐, 내부적으로 node 가 dir 나 file 로 대응되는 것은 아님

![zookeeper_2](/asset/img/zookeeper_2.png)

### 사용

* Java 혹은 C++ library 로 Zookeeper 서비스에 연결함

* Zookeeper 서비스를 마치 atomic한 file system 인 것처럼 사용함
    * 예: 특정 역할의 서버들 → 특정 node 아래 있는 파일들에 자신의 정보를 기록
    * 예: 특정 서버의 master → 사전에 정한 node를 먼저 작성하면 그게 master 가 됨

* 특정 node 에 변경이 발생하는 것을 watch 하고 알림을 받을 수 있다.
    * 예: master 정보를 저장한 node 를 soft-state 처럼 사용 <br>
    먼저 그 node 를 작성한 서버가 master 가 됨 <br>
    그 서버는 주기적으로 master node 를 갱신 <br>
    master node 갱신에 실패해서 사라지면 master 가 죽은 것으로 인식함 <br>
    이를 통지 받은 다른 서버들이 master 가 되기 위해서 경쟁 (= 그 node 를 먼저 작성) <br>

### 설정 참고

`보통 Zookeeper 서버는 3대를 사용함` <br>
그보다 많은 댓수를 넣으면 서버간 데이터 동기화에 부하가 커짐 <br>
3대보다 적을 때는 Zookeeper 장애 발생시 복구를 못함

### 성능 정보

![zookeeper_3](/asset/img/zookeeper_3.png)

![zookeeper_4](/asset/img/zookeeper_4.png)

## 응용에서 Auto Scaling 가능 여부의 판단

* 많은 응용에는 stateful 한 구성 요소와 stateless 한 구성 요소가 섞여 있음

* Auto-scaling 은 stateless 한 구성 요소에서 가장 잘 동작함

    - 연결 기반이 아닌 필요한 요청이 있을 때만 통신하는 RESTful API 기반  <br>
    - 각 worker 가 유저 데이터를 caching 하지 않고
    매 요청마다 DB 에서 유저 데이터를 읽어 들이는 구조

## Auto Scaling 적용 Case Study : 모바일 앱

모바일 앱의 경우 대부분 3-tier architecture 로 구성됨  <br>
이 경우 web server 와 app server 는 손쉽게 auto-scaling 할 수 있음

### 3-Tier Architecture

![3-Tier](/asset/img/3_tier.png)

* Presentation tier
    * 사용자 UI 및 통신 담당
    * 정보를 표시하고(presentation) 사용자로부터 정보를 수집
    * 모바일 앱 클라이언트, 웹 브라우저/서버가 해당
* Application tier
    * Logic tier 라고도 함
    * Data 를 가공해서 핵심 business logic 구현
    * 앱 서버가 해당
* Data tier
    * Application 이 처리하는 정보의 저장 및 관리  


## Auto Scaling 적용 Case Study : MMORPG
* MMORPG 의 지역 서버의 경우 특정 지역을 서버에 맵핑하는 방식을 사용함

    * 즉 클라이언트가 게임 내 지역을 이동하는 경우 이전과 다른 서버와 통신하는 방식임
    
         다른 지역으로 넘어갈 때는 이전 서버와의 연결이 종료되고 새로운 서버와의 통신이 시작되면서 유저 입장에서 환경이 달라지는 것을 느끼게 됨

         이런 차이를 느끼지 못하게 이전 서버와의 연결을 일정 시간 동안 계속 유지하는 게임을 seamless 게임이라고 함

* 이런 방식은 `stateful` 하므로 **auto-scaling 은 불가능함**

* 다른 구성 요소인 인증/빌링/채팅 서버 등은 auto-scaling 이 가능함

## Auto Scaling 적용 Case Study : FPS

* FPS 의 경우 게임 세션이 시작되면 dedicated server 를 복제해서 띄우고 같은 게임 세션 내 유저들은 같은 `dedicated server` 로 접속해서 플레이하는 구조

    * 이미 만들어진 dedicated server 를 auto-scaling 할 수는 없음
    
    * 그러나 dedicated server 를 호스팅할 가상 서버는 auto-scaling 할 수 있음

* 매칭 서버의 경우 구현 방식에 따라 stateful 할 수도 있고 아닐 수도 있음
    * Stateful 한 경우: 매치 요청을 서버의 메모리에 저장하는 방식
    * Stateless 한 경우: 매치 요청을 DB 에 저장하는 방식