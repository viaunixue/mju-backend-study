# Redis : status 저장을 위한 in-memory DB

## Redis 특성

### NoSQL 형태의 In-memory DB 서버

* 주의: SQL 서버는 DB 서버이지만, 모든 DB 서버가 SQL 서버는 아닙니다.

    &rarr; SQL 형태가 아닌 DB 서버를 “NoSQL 서버” 라고 부릅니다.

* In-memory DB 이므로 기본적으로 `휘발성`입니다. (= 리부팅시 데이터 손실)

* 복잡한 복구 로직을 제공하지 않기 때문에 높은 성능을 낼 수 있습니다.

### Key-value 기반의 저장소

* 모든 레코드가 동일한 필드를 갖게끔 구조화된 SQL 서버와 달리 <br>
key 에 대응되는 value 저장소 역할을 합니다. (= `레코드 별로 각기 다른 key 가능`)

* SQL 처럼 쿼리를 만드는 것이 아니라 key 로 look-up 을 하고 저장된 value 를 읽는 방식으로 씁니다.

## Redis 용도

### 분산 시스템에 다목적으로 사용됩니다.

* 서버 간 공유 데이터 저장소

* 서버 간 메시지 큐

* 특정 데이터를 cache 하기 위한 용도

* 유저의 세션 정보 저장소

* 방 세션 정보 저장소
    &rarr; 채팅 혹은 게임 등에서 유저가 속한 방의 고유 식별 번호

## Redis 의 ACL (Access Control List)

* SQL 서버처럼 Redis 역시 DB를 여러 개 만들어 데이터를 구분할 수 있습니다.

* SQL이 DB 별로 유저의 권한 목록을 지정할 수 있는 반면, Redis는 이런 권한 지정을 지원하지 않습니다.
    ```
    Redis 에서는 유저를 구분해 놓아도 유저들이 모든 DB에 접근 가능합니다.
    ```

* Redis 는 한 Redis 서버에 여러 DB 를 띄우는 것이 아니라 `DB 당 Redis 서버를 다시 띄우는 방식`을 많이 사용합니다.
    ```
    물론 사용자를 만들 수도 있지만, 이 역시 잘 쓰지 않습니다.
    보통 docker를 통해 여러 개의 서버를 띄웁니다.
    ```

## 저장 가능한 데이터 타입

* `string` - 가장 기본이 되는 타입, 숫자 문자 구분 없음 <br>
    &rarr; 연속되는 byte 값 형태입니다. why? `utf-8` 때문입니다.

* `list` - 문자열의 linked list 형태

* `set` - 순서가 보장되지 않는 문자열의 집합

* `sorted set` - 순서가 보장되는 문자열의 집합

* `hash` - key-value 쌍

* `JSON` - (별도 모듈 동작시켜야 함. 추후 설명)

* etc
    * `stream` : append-only log
    * `bitmap` : bit vector

## Redis CLI

* Redis 서버에 접속 해 명령을 입력할 수 있는 command line interface (CLI) <br>
&rarr; 접속할 Redis의 IP, Port를 명시 가능

* Shell 작업 시 주로 사용

* 프로그래밍 언어에서는 redis-cli 를 실행하는 것이 아니라, <br>
각 언어별 language binding (= 쉽게 말해 SDK) 를 이용해 함수 호출처럼 사용함
    ```
    C/C++ -> hiredis
    Python -> redis-py
    Java -> Jedis
    ```

## Redis Key

* 임의의 문자열이 key로 쓰일 수 있음

* Key 사용 컨벤션
    * Key 충돌을 막기 위해 일반적으로 `{데이터타입} : {이름}` 형태로 씁니다.
    * 데이터 타입이 중첩되는 경우 `{데이터타입} : {서브타입} : {이름}` 처럼 콜론(:)을 계속 붙입니다.
        ```
        USER : jmond
        PUBG:USER:jmond
        USER:SESSION:1234567890
        ```

## Redis Key 기본 명령어 모음

### KEYS : Key 리스트 보기
    모든 key 보기 : KEYS *
    a로 시작하는 key 들 보기 : KEYS a*

### EXITS : Key 가 존재하는지 보기
    ex) EXITS USER:dkmoon

### DEL : 특정 key 삭제하기
    ex) DEL USER:dkmoon

### SET : key-value 저장하기
    ex) SET USER:dkmoon:hp 100

### GET : key로부터 value 읽어오기
    ex) GET USER:dkmoon:hp

### EXPIRE : 일정 시간 후 key 만료
    ex) EXPIRE USER:dkmoon 5

* 참고 : Redis를 사용해 soft-state 구현할 때 사용됩니다.
    ```유저의 로그인 정보를 일정 시간 뒤에 만료 시키는 경우 <br>
    서버는 그 제한 시간이 다 되기 전에 주기적으로 redis 에 다시 EXPIRE 명령을 해 만료 시간을 변경합니다.
    ```

### INCRBY : value 값을 증가시김
    ex) INCRBY USER:dkmoon:hp 10

### DECRBY : value 값을 감소시킴
    ex) DECRBY USER:dkmoon:hp 10

## Key-Value 에서 Value 가 `List` 인 경우

### LPUSH : list 앞(left) 데이터 추가
    ex) 하나 추가 : LPUSH USER:dkmoon:items 10001
    ex) 여러 개 반복 추가 : LPUSH USER:dkmoon:items 10002 10003 10004 

### RPUSH : list 뒤(right) 데이터 추가
    ex) 하나 추가 : RPUSH USER:dkmoon:items 10009
    ex) 여러 개 반복 추가 : RPUSH USER:dkmoon:items 10010 10011 10012

### LPOP : list 제일 왼쪽 것 삭제

### RPOP : list 제일 오른쪽 것 삭제

### LREM : list 중 원소 삭제

### LRANGE : list 원소 보기
    ex) LRANGE USER:dkmoon::items 0 -1
    
## Key-Value 에서 Value가 `Hash` 인 경우 

* Redis 수준의 key-value 구조 외 hash 자체 key-value 한 단계 더 존재하게 되는 것입니다. 

### HSET : hash table 을 만들거나 데이터를 추가
    ex) Field 하나 추가 : HSET USER:dkmoon:inventory helmet 1002
    ex) Field 여러개 추가 : HSET USER:dkmoon:inventory helmet 1002 hand

### HGET : hash table key 의 field 의 값을 얻어냄
    ex) HGET USER:dkmoon:inventory hand

### HMGET : hash table key 의 특정 field 들의 값을 얻어냄 (multiple get)
    ex) HMGET USER:dkmoon:inventory helmet hand

## Key-Value 에서 Value가 `Set` 인 경우 

### SADD : set을 만들거나 set에 원소를 추가
    ex) SADD USER:dkmoon:visitedCountries USA Japan China

### SMEMBERS : set의 원소들을 얻어냄
    ex) SMEMBERS USER:dkmoon:visitedCountries

### SISMEMBER : set의 원소인지 확인
    ex) SISMEMBER USER:dkmoon:visitedCountries Egypt

## Key-Value 에서 Value가 `Sorted Set` 인 경우 

* Set 과 유사한데 score 점수를 같이 저장하고 그 점수로 오름차순 정렬을 해줍니다.

### ZADD : sorted set 생성 혹은 원소 추가
    ex) ZADD USER:dkmoon:sortedItems 10 RedPotion1
    ex) ZADD USER:dkmoon:sortedItems 3 BluePotion2

### ZRANGE : sorted set 내 원소 보기
    ex) ZRANGE USER:dkmoon:sortedItems 0 -1

### ZREM : 원소 삭제

## Key-Value 에서 Value가 `JSON` 인 경우 
* 기본 타입으로 JSON이 지원되지는 않고, Redis 서버를 별도 모듈과 같이 실행하면 사용 가능합니다.

### JSON.SET : 주어진 key에 JSON 정보 저장
    ex) JSON.SET USER:dkmoon . '{"name":"정종현", "email":"jmond@mju.ac.kr"}'

### JSON.GET : 주어진 key에 대응되는 JSON 문자열을 반환함
    ex) JSON.GET USER:dkmoon

### JSON.OBJKEYS : JSON 안에 있는 key 들을 얻어냄
    ex) JSON.OBJKEYS USER:dkmoon

## JSONPath 

### `$` : 루트 요소

### `.` 또는 `[ ]` : 하위 연산자

### `..` : 몇 단계일지 모르지만 계속 내려가서 찾음

### `*` : 와일드 카드

### `[ ]` : 배열일 경우 첨자
<br>

> JSON Path 예시
```json
{
    "store" : {
        "book" : [
            {
                "author":"Nigel Rees",
                "title":"Saying of the Century"
            },
            {
                "author":"Evelyn Waugh",
                "title":"Sword of Honour"
            }
        ]
    }
}
```
* `$.store.book[*].author` <br>
&rarr; root 레벨에 있는 store 객체 아래 <br>
        book 이라는 array 가 있는데, <br> 
        그 모든 array 요소에 대해 author 라는 필드

* `$..author` <br>
&rarr; root 에서부터 계속 내려가서 <br>
        author 라는 필드 전부 다

* `$..book[0]` <br>
&rarr; book의 첫번째 원소

* JSON.GET 명령에 JSONPath 적용하기

    > JSON.GET USER:dkmoon
    &rarr; JSON 문자열 전부

    > JSON.GET USER:dkmoon .name
    &rarr; JSON 중 name 이란 필드

    > JSON.GET USER:dkmoon .email 
    &rarr; JSON 중 email 이란 필드