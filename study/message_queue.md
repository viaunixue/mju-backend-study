# Message Queue

## [RECAP] Producer-Consumer 문제

#### “생산자”(producer)와 “소비자”(consumer) 의 협업

> 생산자는 어떤 일감을 만들어냅니다. <br>
> 소비자는 그 일감을 소비합니다. (즉, 일을 끝마친다.)

#### 생산자와 소비자는 고정된 크기의 메모리 공간(buffer)를 통해 일감을 전달합니다.

> 참고: 버퍼는 메모리 저장 공간을 의미하는데, 실무적으로는 주로 대기열(queue) 자료구조를 씁니다.

#### 생산자는 버퍼가 다 찼다면 일감을 더 이상 집어 넣어서는 안됩니다.

#### 소비자는 버퍼가 비었는데 일감을 빼내려고 해서는 안됩니다.

#### 생산자와 소비자는 동시에 버퍼에 접근할 수 없습니다.

![message_queue_1](/asset/img/message_queue_1.png)

### Producer-Consumer 에서 발견할 수 있는 것

* Producer 와 consumer 는 queue 를 통해서 통신한다.
* Queue 에 집어 넣는 producer 는 여러 개일 수 있다. (예: 복수 개 threads) <br>
그러나 consumer 는 producer 가 몇 개인지 모른다.
* Queue 에서 빼는 consumer 는 여러 개 일 수 있다. (예: 복수 개 threads) <br>
그러나 producer 는 consumer 가 몇 개인지 모른다.

&rarr; **위의 내용을 종합해보면 queue는 producer 와 consumer 를 decoupling 하는 효과가 있다.**

### Producer-Consumer 를 프로세스 수준으로 확장

앞서 우리는 producer-consumer를 `하나의 프로세스 안에서 동작하는 쓰레드`로 이해했습니다.

> queue가 producer와 consumer를 `decouping` 하는 역할을 한다면 <br> 이를 쓰레드가 아니라 프로세스 수준으로 확장할 수 있지 않을까?

&rarr; 물론 Job의 전달은 더 이상 메모리 공간을 공유하는 방식이 아니라 프로세스간 통신 방식에 의존해야 합니다. (소켓)

![message_queue_2](/asset/img/message_queue_2.png)

### Queue Process 에 여러 Queue 추가

* 이제 queue를 전담하는 process가 있고 여기에 하나의 queue만을 담당하게 하는 것은 비효율적입니다.

* 목적에 따라 여러가지 Job queue를 둘 수 있다고 가정하면 이 queue process 안에 여러 Queue를 추가하는 것도 좋습니다.

![message_queue_3](/asset/img/message_queue_3.png)

    앞의 Process 수준 producer-consumer 다이어그램에서 producer가 queue를 통해서 consumer에게 전달하는 것을
    단순히 작업(Job)이라 한정하지 말고 어떤 Message라고 일반화하면 Queue 프로세스는 Message Queue가 됩니다.

## Topic 별 Queue

### 여러 queue가 있고 각각의 queue에 `이름`이 있다면?

`Producer는 특정 이름의 queue에 메시지를 전달할 수 있습니다.`

`Consumer는 특정 이름의 queue에서 메시지를 전달 받을 수 있습니다.`

&rarr; `Message`라는 단어에 걸맞게 Queue에 붙는 이름은 `topic`이라고 표현합니다. 즉, `topic 별 메시지가 갖는 queue`가 됩니다. <br>
> 예: "auth""라는 topic과 "payment"라는 topic을 만들고 각각 인증 요청 메시지는 "auth"로, 결제 메시지는 "payment"로 보냅니다.

![message_queue_4](/asset/img/message_queue_4.png)

### Message Queue 가 지원하는 인터페이스

멀티 쓰레드 producer-consumer 에서는 다음 두 가지 인터페이스가 존재했습니다.

1. Producer를 위한 인터페이스 &rarr; job 추가

2. Consumer를 위한 인터페이스 &rarr; job 에 따른 handler 등록 및 호출

&rarr; 이를 **프로세스 수준**으로 구현된 topic 별 메시지 큐로 변환하면 다음 두가지 인트페이스가 됩니다.

1. Producer 를 위한 인터페이스
    * `Publish` : 특정 topic의 queue에 메시지를 집어넣습니다.

2. Consumer 를 위한 인터페이스
    * `Subscribe` : 특정 topic의 queue의 consumer로 등록합니다.

    * 이후 해당 queue 에 들어오는 메시지를 push 로 보내주거나 consumer가 직접 pull 하게 합니다.

## 마이크로 서비스 아키텍처

#### 단일 애플리케이션이 여러 개의 작은 서비 (= micro service)로 구성되는 방식

이때 `micro service` 는 독립적으로 배치 가능해야 됩니다. <br>

Micro service 들은 서로 *`느슨하게 연결`* 되어야 합니다. 즉, 서로에게 높은 의존성을 가져서는 안됩니다.

### 마이크로 서비스 아키텍처에서 MQ 역할

* Message queue는 publish 와 subscribe 이라는 제한된 인터페이스를 통해서 <br> 통신 상대방이 누구인지와 관계 없이 메시지를 전달 할 수 있다.

* 이 때문에 마이크로 서비스 아키텍처의 조건인 `micro service 간 느슨한 연결` 에 도움을 준다.

&rarr; 이런 이유로 Message Queue 는
마이크로 서비스 아키텍처 방식으로 서비스를 설계하는데 많이 사용된다.

### 참고 : 느슨한 연결 (Loosely Coupled)

대개 "`느슨한 연결`"은 다음 둘 중 하나로 구현됩니다.

#### RESTful API 와 같은 널리 알려진 표준화된 인터페이스로 통신하는 방식

#### 중간에 queue를 두고 통신하는 방식

### State 관점에서 마이크로 서비스 아키텍처

* 마이크로 아키텍처를 구성하는 마이크로 서비스는
대개 요청에 의해 `작은 단위의 작업`만을 수행한다.
(그래서 이름이 micro service 이다.)
* 이 때문에 많은 경우에 마이크로 서비스는 `stateless` 하다.
* stateless 하므로 자동으로 `scale-out`하기에 용이하다.
* Message queue 는 scale-out 하는 마이크로 서비스를 손쉽게 서비스 아키텍처에 인식시킬 수 있다.
전체 서비스 구조에 대한 이해 없이도
기존에 존재하는 queue 에 producer 혹은 consumer 로 추가하면 된다.

## Message Queue 를 통한 통신 패턴 구현

> 영어 단어 “-cast” 는 청중에게 전달한다는 의미가 있다.

1. `Unicast`: 특정 대상에게 메시지 전달
2. `Multicast`: 다수의 사용자에게 메시지 전달
3. `Broadcast`: 모든 사용자에게 메시지 전달
4. `Anycast`: 다수의 사용자 중 아무나 하나에게 전달

### Unicast

#### 특정 consumer를 위한 전용 queue를 둡니다.

![unicast](/asset/img/unicast.png)

### Multicast

#### 특정 topic의 queue에 여러 consumer가 subscribe 합니다.

![multicast](/asset/img/multicast.png)

### Broadcast

#### 특정 topic의 queue에 `모든` consumer가 subscribe 합니다.

![broadcast](/asset/img/broadcast.png)

### Anycast

#### 특정 topic의 queue가 메시지가 도착하면 `consumer를 임의로 골라서` push 합니다.

![anycast](/asset/img/anycast.png)

## Message Queue 솔루션

### RabbitMQ

* AMQP(Advanced Message Queue Protocol)이라는 공개 표준 프로토콜 이용 <br>
&rarr; 따라서 RabbitMQ 사용을 위해 다양한 AMQP client SDK 를 사용할 수 있다.

* 메시지를 어떤 queue 에 보낼지 결정하는 routing 을 위해 exchange 라는 개념이 존재한다. <br>
이 때문에 `유연한 routing 이 가능`하다.

* **메시지는 consumer 에게 전달되면 삭제된다.**

* 보통 micro service 들을 연결하기 위한 메시지 큐로 이용된다.

### Apache Kafka

* RAM 사용시 생길 수 있는 overhead 를 피하고자 `disk 에 의존`한다. <br>
DISK 는 random access 때문에 seek time 이 발생해서 속도가 느리다. <br>
Kafka 는 이를 피하기 위해서 `disk 의 sequential I/O 를 이용`한다.

* Database 영역에서 사용되는 `append-only log 방식`을 이용한다. <br>
Producer 는 queue 에 대응되는 파일 끝에 쓰기만 한다.
이로써 메시지는 disk 에 기본 저장된다. <br>
**Consumer 가 자기가 이 queue 의 어디까지 읽었는지를 기억하면서 pull 한다.**

* **높은 throughput 성능**을 보이며 이를 활용해 log processing 등에 이용된다. <br>
LinkedIn 에서 user activity log 분석을 위해 개발되었다.

### Kafka 의 Append-Only Log

![kafka](/asset/img/kafka.png)

### RabbitMQ vs Kafka

|  | RabbitMQ | Kafka |
|--|--|--|
| Throughput (MB/s) | 수십 | 수백 |
| 메시지 보관 여부 | Consumer에 전달되어 <br> ACK이 발생하면 삭제됨| 기본적으로 디스크 저장 <br> 설정으로 일정 시간 경과 후 삭제 가능 |
| 메시지 전달 (routing) <br>유연성| Broker인 RabbitMQ 에서 <br> 결정할 수 있는 것들이 많음. <br> 상대적으로 consumer는 <br> 별 일을 하지 않음. <br> (smart broker + dumb consumer)| Consumer가 적절하게 `pub-sub` <br> 해야 함. 어디까지 읽었는지 직접 <br> tracking 해야 함. <br> (dumb broker + smart consumer) |
| 클러스터링 방식 | 직접 mirroring | Zookeeper 이용 |
| 적합한 용도 | 복잡한 라우팅을 통해 여러 단계에 <br> 걸쳐 작업을 처리하는 형태에 용이 <br> (예: 비디오 인코딩) | 단일 단계에서 대규모 데이터 처리에 용이 <br> (예: 사용자 행동 로그 처리) |