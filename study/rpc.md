## RPC (Remote Procedure Call)

#### 네트워크 상의 다른 컴퓨터에 동작하는 프로그램에 어떤 동작을 요청하는 방식입니다.

&rarr; RPC는 Client-Server 방식을 따릅니다. (기능 제공하는 쪽이 Server, 호출하는 쪽이 Client)

&rarr; RPC는 `local procedure call 처럼 동기적 통신`입니다. <br>
    즉, 요청을 보내고 요청을 받을 때까지 client는 다음 요청을 보낼 수 없습니다. <br>
    (Client 내에서 요청을 보낸 thread는 Local procedure call 처럼 blocking 됩니다.)

### 좁은 의미에서 RPC

Local procedure call 호출 <br>
&rarr; 네트워크 메시지로 패키징 <br>
&rarr; 요청을 전달 후 응답을 받아 마치 local procedure 에서 반환하는 것처럼 포장해주는 것

![rpc_1](/asset/img/rpc_1.png)

* Google Protobuf 및 gRPC
* Apache Thrift
* Twitter Finagle

## HTTP를 RPC로 이용하는 경우

* 넗은 의미로 RPC 원격 컴퓨터에 "동작을 요청"하는 것이므로 HTTP를 RPC 방식으로 이용할 수 있습니다.

    * 데이터를 얻어내는 동작 : HTTP의 GET 메소드 사용
    * 데이터를 변경하는 동작 : HTTP의 POST 메소드 사용

## HTTP 이용한 RPC와 RestFul API와의 차이

| | |
| -- | -- |
| HTTP 이용한 RPC | RestFul API |
| `동작 관점` | `리소스` 관점 |
| "이런 이런 동작을 지시한다"| "이런 이런 리소스에 접근한다" |
| 호출 End Point는 동작 이름을 포함 | 호출 End Point는 리소스 이름을 포함함 |
| GET, POST 만 주로 사용 | CRUD를 위해 POST, GET, PUT, DELETE 사용 |

| | RPC (동작) | REST(리소스) | 
| -- | -- | -- |
| 가입 | POST /signup | POST /persons |
| 회원 정보 읽기 | GET /readPerson?id=100 | GET /persons/100 |
| 회원 장바구니 읽기 | GET /readPersonCart?id=100 | GET /persons/100/cart |
| 회원 장바구니에 상품 추가 | POST /updatePersonCart?id=100&item=200 | POST /persons/100/cart/200 |

## 서버 간 통신 방법

### 동기식 통신이 필요

* 작업이 stateless &rarr; HTTP 기반 방식 사용 (HTTP를 RPC로 쓰거나 REST API)

* 작업이 stateful &rarr; RPC library 사용

### 비동기식 통신이 필요 &rarr; Message Queue 사용

<details> 
    <summary> Throughput 관점에서 동기식과 비동기식의 차이는? </summary>
<br>
한번에 하나의 작업만 처리하는 동기식 관점보다 여러 작업이 동시에 실행되는 비동기식 관점이 <br>
Throughput 관점에서 동기식 처리보다 효율적이라고 볼 수 있습니다. <br>
다만 시스템의 복잡성, 작업의 종류, 필요한 리소스를 고려해야 합니다.

</details>