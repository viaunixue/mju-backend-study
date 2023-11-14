<details>
<summary>RECAP</summary>

## 용어
### Race condition 
쓰레드 순서에 따라 결과 값이 달라지는 것
### 원자적(atomic) 
원자가 더이상 쪼갤 수 없듯, "반영됐다", "반영되지 않았다" 두 가지 경우로만 구분되는 것

## C++ Thread
### 헤더
`#include <thread>`
### 쓰레드 생성
class thread의 객체 생성
- 쓰레드가 실행할 함수 및 인자를 클래스 생성자로 넘김
- 생성 후 CPU 대기 큐에 들어가고 이후에 스케쥴링 됨
### 쓰레드 종료시키기 / 재우기
외부에서 중단시킬 수 없고, flag 통해서 "요청"
- 예 : `atomic<bool> quit`
### 쓰레드 종료 후 처리
쓰레드는 종료된 후 다른 쓰레드에 의해 `join()` 되어야 함
- join() 호출 축은 종료시까지 blocking 됨
- join() 되는 쓰레드는 join()후에 메모리 자원이 정리 됨

## C++ Mutex
### 헤더
`#include <mutex>`
### 목적
공유 리소스 접근 시 오직 한 쓰레드만 접근 가능한 것을 보장
### 생성
`class mutex` 객체 생성 (주의 : 생성한다고 바로 lock 잡히는 것이 아님)
### 잠금(Lock), 해제(Unlock)
lock(), unlock() 메서드 호출
### 유틸리티 클래스
class lock_guard (또는 class unique_lock)

## C++ Condition Variable
### 헤더
`#include <condition_variable>`
### 목적
쓰레드 간 어떤 "조건(condition)"의 발생을 `기다리고` `통지하는` 목적
### 생성
`class condition_variable` 객체 생성
### 대기 
`wait() 메서드 호출` (주의 : 반드시 연관되는 Mutex 잡고 호출) <br>
(wait()을 호출하면 blocking 되고, 꺠어날 때 mutex를 잡은 상태로 깨어남)
### 통지
`notify_one()` 또는 `notify_all()` 메서드 호출
(주의 : 반드시 연관되는 mutex 잡고 호출하고, 호출 후 mutex를 해제)

</details>

<br>

# Producer-Consumer Problem

* "생산자"(producer)와 "소비자"(consumer) 의 협업 문제
    * 생산자는 어떤 일감을 만들어냅니다.
    * 소비자는 그 일감을 소비합니다. (즉, 일을 끝마칩니다.)

* 생산자와 소비자는 고정된 크기의 메모리 공간(buffer)을 통해 일감을 저장합니다.
    * 참고: 버퍼는 메모리 저장 공간을 의미하는데, 실무적으로 주로 대기열(queue) 자료구조를 사용합니다.

* 생산자는 버퍼가 다 찼다면 일감을 더 이상 집어 넣어서는 안됩니다.

* 소비자는 버퍼가 비었는데 일감을 빼내려고 해서는 안됩니다.

* 생산자와 소비자는 동시에 버퍼를 접근할 수 없습니다.

## Pseudo code
### 생산자
```c++
void producer() {
    while(True){
        job = produce_job()
        wait(Fillable)
        lock(Mutex)
        append_job(job)
        notify(Filled)
        unlock(Mutex)
    }
}
```
### 소비자
```c++
void consumer() {
    while(True){
        wait(Filled)
        lock(Mutex)
        job = take_job()
        notify(Fillable)
        unlock(mutex)
        do_job(job);
    }
}
```

## `ANSWER #1`
### 쓰레드 생성

> 다음은 [answer1.cpp](/study/producer_consumer/problem.cpp) 코드에 대한 해석입니다.

* 해당 코드는 현재 thread t1, t2 라는 빈 객체를 만들고 OS 상의 쓰레드는 생성하지 않고 있습니다.

#### t1 은 producer 함수를 실행하는 쓰레드를, t2는 consumer 함수를 실행하는 쓰레드를 만들도록 코드를 수정해주세요.

```java
thread t1; -> thread t1(producer);
thread t2; -> thread t2(consumer);
```

`총 몇개의 Thread가 동작하는가?` 

## `ANSWER #2`
### 쓰레드 종료 조건 구현

> 다음은 [answer2.cpp](/study/producer_consumer/answer2.cpp) 코드에 대한 해석입니다.

* 현재는 main() 함수 내에서 쓰레드 생성 후 바로 quit.store(true) 함으로써 쓰레드가 생성되자마자 죽습니다. <br>
&rarr; 이를 약 5초동안 producer, consumer 쓰레드가 동작하게 하고 싶습니다.

#### quit.store(true) 앞에 sleep 함수를 넣어서 5초 뒤에야 quit.store(true) 하도록 수정해주세요.

`<hint>` this_thread::sleep_for() 와 chrono::seconds(5) 혹은 chrono::milliseconds(5000)를 사용합니다.

```c++
this_thread::sleep_for(chrono::seconds(5));
quit.store(true);
```

### 기존의 다음 코드에는 무슨 문제가 있는가?

```java
void producer() {
    ...
    while(quit.load() == false){
        int job = rand() % 100;
        if (que[0] == NO_JOB){
            que[0] = job;       // que[0]이 비어있으면 버림
        }
    }
    ...
}

void consumer() {
    ...
    while(quit.load() == false){
        int job;
        job = que[0];
        que[0] = NO_JOB;
    }
    ...
}
```

1. busy waiting
2. que 에 대해 동시에 접근
3. que 가 비어있으면 버림

## `ANSWER #3`
### 작업의 원자성

> 다음은 [answer3.cpp](/study/producer_consumer/answer3.cpp) 코드에 대한 해석입니다.

#### producer() 와 consumer() 함수 안의 while문 안쪽의 que 배열을 건드리는 작업들이 <br> 원자적으로 동작할 수 있도록 atomic 하게 `queMutex`를 이용하여 코드를 수정해주세요.

`<hint>` 다음 중 하나를 쓰쓸 수 있습니다. **그런데 `unique_lock<mutex>`를 사용하세요.**
* `queMutex.lock()` 과 `queMutex.unlock()`
* `lock_guard<mutex>` lg(queMutex)
* `unique_lock<mutex>` ul(queMutex)

### queMutex 사용

```c++
void producer() {
    ...
    while(quit.load() == false){
        int job = rand() % 100;
        queMutex.lock();
        if (que[0] == NO_JOB){
            que[0] = job;       
        }
        queMutex.unlock();
    }
    ...
}

void consumer() {
    ...
    while(quit.load() == false){
        queMutex.lock();
        int job;
        job = que[0];
        que[0] = NO_JOB;
        queMutex.unlock();
    }
    ...
}
```

### unique_lock<mutex> 사용

```c++
void producer() {
    ...
    while(quit.load() == false){
        int job = rand() % 100;
        {
            unique_lock<mutex> ul(queMutex);
            if (que[0] == NO_JOB){
                que[0] = job;       
            }
        }
    }
    ...
}

void consumer() {
    ...
    while(quit.load() == false){
        int job;
        {
            unique_lock<mutex> ul(queMutex);
            job = que[0];
            que[0] = NO_JOB;    
        }

    }
    ...
}
```

### 현재 남은 문제는?
* `mutex` 작업을 통해 현재 코드는 atomic 해졌지만 다음의 문제가 남아있습니다.

1. producer 는 `que`가 이미 찬 경우 생성한 `job`을 넣지 못하고 버립니다. <br>
이 `job`은 유저가 우리 앱에 보낸 요청일 수 있습니다. <br>

따라서 버리면 안되고 `que`가 빌 때까지 기다렸다가 `job`을 넣을 수 있어야 합니다.
2. consumer 는 `que`가 비어 있는 경우도 매번 `que`를 확인합니다. ( = polling ) <br>
가져간 `job`이 `NO_JOB`인지 체크하고 처리를 하긴 하겠지만, 계속 `que`를 접근합니다. 그러느라 의미없이 CPU를 사용합니다. 

#### 그리고 `que`가 `mutex`로 보호되어 있기 때문에 `mutex` 역시 의미 없이 접근하는 것입니다. <br>
이는 `mutex`에 관해 producer 과 consumer 가 의미 없는 경합(condition)을 하는 것입니다.

## `ANSWER #4`
### condition_variable 추가 #1

> 다음은 [answer4.cpp](/study/producer_consumer/answer4.cpp) 코드에 대한 해석입니다.

#### producer 에서 que가 이미 차 있는 경우 queFillable 이라는 condition_variable을 wait() 해주세요

#### consumer 에서 que에서 데이터를 가져간 후 queFillable 를 notify_one() 하게 해주세요

그리고 notify 다음 줄에 다음 로그를 추가해주세요
```c++
cout << "Consumed: " << job << endl;
```
 
```c++
void producer() {
    ...
    while(quit.load() == false){
        int job = rand() % 100;
        {
            unique_lock<mutex> ul(queMutex);
            while (que[0] == NO_JOB){
                queFillable.wait(ul);      
            }
            que[0] = job; 
        }
    }
    ...
}

void consumer() {
    ...
    while(quit.load() == false){
        int job;
        {
            unique_lock<mutex> ul(queMutex);
            job = que[0];
            que[0] = NO_JOB;   
            queFillable.notify_one();           // que의 길이가 길다면 notify_all()가 효율적
            cout << "Consume: " << job << endl; 
        }

    }
    ...
}
```

<br>

> 주 쓰레드가 1개일 경우 while 대신 if 도 가능하나, <br>
  통상적으로 CV wait() 은 while 로 감싸는 것이 안전합니다. 이유는?
  
  &rarr; Producer가 여러 개 일때를 가정해보면 됩니다. <br>
    Consumer에서 notify_all()을 했다면, 여러 개 중에 하나만 가져가고 나머지는 다시 기다려야 합니다. <br>
    이 부분을 처리하기 위해 while 문을 사용합니다.

## `ANSWER #5`
### condition_variable 추가 #2

> 다음은 [answer5.cpp](/study/producer_consumer/answer5.cpp) 코드에 대한 해석입니다.

#### producer 에서 que에 데이터를 쓰고 queFilled 라는 condition_variable 을 notify_one() 하게 해주세요. 

그리고 notify_one() 뒤에 다음 로그 메세지를 추가해주세요.
```c++
cout << "Produced: " << job << endl;
```

#### consumer에서는 que에 데이터가 없는 경우 queFilled를 wait 하게 해주세요

```c++
void producer() {
    ...
    while(quit.load() == false){
        int job = rand() % 100;
        {
            unique_lock<mutex> ul(queMutex);
            while (que[0] == NO_JOB){
                queFillable.wait(ul);      
            }
            que[0] = job; 
            queFilled.notify_one();
            cout << "Produced: " << job << endl;
        }
    }
    ...
}

void consumer() {
    ...
    while(quit.load() == false){
        int job;
        {
            unique_lock<mutex> ul(queMutex);
            while(que[0] == NO_JOB){
                queFilled.wait(ul);
            }
            job = que[0];
            que[0] = NO_JOB;   
            queFillable.notify_one();           // que의 길이가 길다면 notify_all()가 효율적
            cout << "Consume: " << job << endl; 
        }

    }
    ...
}
```

## 쓰레드 사용 방식들

### 방법 1
#### 작업이 들어오면 그것을 처리하는 쓰레드를 만들고 소멸 시킵니다.

* 장점
    * 구현이 단순합니다.
* 단점
    * 쓰레드의 경우 생성 / 소멸의 부하가 큽니다.
    * 작업의 양이 많을 때 한 번에 너무 많은 쓰레드가 존재할 수 있습니다.

```
일반적으로 프로그래머들에게 시키면 가장 많이 하는 방법
```

### 방법 2
#### 쓰레드를 미리 만들어두고 (= thread pool), 작업이 들어오면 한 쓰레드가 랜덤하게 선택되어 작업을 처리합니다.

* 장점 - 최대 쓰레드 개수가 제한되어 시스템 최대 부하를 예측할 수 있습니다.
* 단점 - 지나치게 많은 쓰레드를 미리 만들어두면 자원을 낭비할 수 있습니다.

## Thrashing

* 너무 많은 쓰레드 / 프로세스 개수 때문에 가상 메모리 사용이 지나치게 증가해서, <br>
메모리 스와핑에 대부분 시간을 쓰고 (이 순간 컨텍스트 스위칭 발생) 프로세서들이 실행을 멈추는 것처럼 보이는 현상을 말합니다.

## Thread Pool

 * 같은 작업을 하는 쓰레드를 미리 여러 개 만들어 두는 것입니다.

 * Producer, Consumer 모두 thread pool 방식을 사용할 수 있습니다.

 * 작업의 요청과 처리는 공유 대기열을 이용합니다.
    * Producer thread pool : thread 들이 생성한 작업을 공유 대기열에 넣습니다.
    * Consumer thread pool : thread 들이 처리할 작업을 공유 대기열에서 추출합니다.

<br>

* 작업 큐를 여러 쓰레드가 공유하므로 앞어 실습처럼 mutex, cv 가 필요합니다. 

![thread_pool](/asset/img/thread_pool.png)

## 쓰레드 간 통신을 위한 작업 큐 구현
* 큐
* 큐에 대한 배타적 접근을 위한 mutex
* ~~큐에 작업을 추가할 수 있는 상태(condition)임을 알리는 CV~~ &rarr; 만일 큐에 길이제한이 없다면 필요없음
* 큐에 작업이 있다는 상태(condition)를 알리는 CV

## `ANSWER #6`
### 쓰레드 개수 늘리기

> 다음은 [answer6.cpp](/study/producer_consumer/answer6.cpp) 코드에 대한 해석입니다.

#### main() 함수에서 producer 1개를 늘려서 t3를 추가하세요.
`<주의>` t3에 대해서 join() 호출을 잊지 말 것!

```c++
int main() {
    ...
    thread t3(producer);
    ...
    if(t3.joinable()){
        t3.join();
    }
    ...
}

```

## `ANSWER #7`
### 길이 제한 없는 큐로 변경하기

> 다음은 [answer7.cpp](/study/producer_consumer/answer7.cpp) 코드에 대한 해석입니다.

#### 현재 공유 큐는 int que[1] 로 길이 제한 1 입니다. 이를 C++의 queue<int>로 수정해주세요.

* #include <queue> 추가
* int que[1] = { NO_JOB }; 를 queue<int> que; 수정
* condition_variable queFillable; 및 관련 부분 모두 삭제
* producer에 while (que[0] == NO_JOB) 과 관계 없이 언제나 `que.push(job);`
* consumer에서 job = que[0]; 및 que[0] = NO_JOB; 을 `job = que.front();` 및 `que.pop();` 으로 변경