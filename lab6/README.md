# lab 6 - Multi-Threading

## `Test #1` 
### C++ Thread 생성
```c++
#include <iostream>
#include <thread>

using namespace std;

void f1() {
    cout << "f1" << endl;
}

void f2(int arg){
    cout << "f2: " << arg << endl;
}

int main() {
    thread t1;
    thread t2(f1);
    thread t3(f2, 10);

    t2.join();
    t3.join();

    return 0;
}

```
* C++의 Multithreading은 OS의 Kernel level thread에 대한 Wrapper 입니다. <br>
&rarr; C++ 상에서 thread를 만들면 OS가 제공하는 thread를 생성합니다. <br><br>

![test1](/asset/img/lab6_test1_1.png)
* 다음 결과물과 같이 순차적인 실행을 보장하지 않습니다. <br>
* CPU가 여러개라서 나오는게 아니라 쓰레드 별로 CPU를 할당하는데, `time quantum`이라는 작은 단위로 쪼개서 실행됩니다.
* I/O 하나가 blocking 되는 사이에 CPU의 다른 데이터가 할당 받은 것입니다.

    ```
    'cout' 이라고 화면에 찍는 명령어는 입출력입니다. -> 느림
    파일로 리다이렉션 안하고 화면에 무엇을 찍는다는 것은 엄청난 노력이 들어갑니다.
    ```

## `Test #2`
### C++ Thread 식별자

```c++
#include <iostream>
#include <thread>

using namespace std;

void f1() {
    cout << "f1" << endl;
}

void f2(int arg){
    cout << "f2: " << arg << endl;
}

int main() {
    thread t1;
    thread t2(f1);
    thread t3(f2, 10);
    cout << "C++ id: " << t3.get_id() << endl;
    cout << "Native id: " << t3.native_handle() << endl;

    t2.join();
    t3.join();

    return 0;
}
```
* `thread::get_id()` &rarr; C++ 상에서의 thread 식별자 타입
* `thread::native_handle()` &rarr; OS 상에서의 thread 타입
![test2](/asset/img/lab6_test1_2.png)

#### C++의 this_thread

```c++
namespace this_thread {
    // 현재 쓰레드 식별자를 반환
    thread::id get_id();

    // CPU를 포기하고 현재 thread 를 CPU ready queue 에 넣음
    void yield();

    // 현재 쓰레드를 일정 시간 동안 잠재움
    void sleep_until(const chrono::time_point & abs_time);
    void sleep_for(const chrono::duration & rel_time);
}
```

## `Test #3`
### this_thread 사용하기

```c++
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void f1() {
    cout << "f1: " << this_thread::get_id() << endl;
    this_thread::sleep_for(chrono::milliseconds(10 * 1000));
    cout << "f1: woke up" << endl;
}

void f2(int arg){
    cout << "f2: " << arg << endl;
    this_thread::sleep_for(chrono::milliseconds(10 * 1000));
    // 주석을 할 경우 마지막 종료된 후 sleep 된 f1 함수가 실행되는 t2가 남아서 2개만 출력됨 
}

int main(){
    thread t1;
    thread t2(f1);
    thread t3(f2, 10);
    cout << "C++ id: " << t3.get_id() << endl;              // c++ 상에서 thread 식별자 타입
    cout << "Native id: " << t3.native_handle() << endl;    // os 상에서의 thread 타입

    t2.join();
    t3.join();
    // join을 한 것은 실제 쓰레드는 종료되었지만 이 쓰레드의 상태값이 메모리에 남아 그것을 없앤 것이 join
    // t3.join을 하지 않으면 메모리, 자원 누수
 
    return 0;
}
```

![test3](/asset/img/lab6_test1_3.png)

<details>
<summary> <strong> main thread 1개 + 새로 생성한 thread 2개 (= t2 ,t3) 하면 3개가 나와야 될 것 같은데 왜 2개일까? </strong> </summary>
&rarr; 마지막 종료된 후 sleep 된 f1 함수가 실행되는 t2가 남아서 2개만 출력됨 
</details>

### std::chrono 현재 시각 알아내기
```
chrono::system_clock::now();
```
&rarr; 서버 시간 (= 대부분 현지 시간)으로서의 현재 시각 

```
chrono::utc_clock::now();
```
&rarr; UTC 기준으로 현재 시각 - `c++20` <br>
ex) g++ -std=c++20 -Wall test3.cpp -o test3.out

### C++ std::chrono 시간 차이 (duration) 알아내기

```c++
// auto 는 타입 알아서 만들어 달라는 의미
auto start = chrono::system_clock::now();
...
auto end = chrono::system_clock::now();

auto duration = end - start;
```

### C++ std::chrono::duration 의 변환

* Duration 변환에는 chrono::duration의 변환
```c++
chrono::duration_cast < chrono::miliseconds>(...) // 밀리초로 변환
chrono::duration_cast < chrono::seconds>(...) // 초로 변환
chrono::duration_cast < chrono::minutes>(...) // 분으로 변환
```

## `Test #4`
### 시간 계산
```c++
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void f1() {
    cout << "f1: " << this_thread::get_id() << endl;
    this_thread::sleep_for(chrono::milliseconds(10 * 1000));
    cout << "f1: woke up" << endl;
}

void f2(int arg){
    cout << "f2: " << arg << endl;
}

int main() {
    auto start = chrono::system_clock::now();

    thread t1;
    thread t2(f1);
    thread t3(f2, 10);
    cout << "C++ id: " << t3.get_id() << endl;
    cout << "Native id: " << t3.native_handle() << endl;

    t2.join();
    t3.join();
    // 자동으로 사라지는 detach 되지 않은 상황이면 무조건 join
    // terminate 될 스레드가 계속 기다림

    auto end = chrono::system_clock::now();
    auto duration = end - start;
    auto seconds = chrono::duration_cast<chrono::seconds>(duration).count();
    cout << "Elapsed: " << seconds << endl;
}
```

### C++ std::chrono::duration 생성

```c++
chrono::miliseconds(100);
chrono::seconds(100);
chrono::hours(100);
chrono::years(100);
```
* 강제로 sleep 하거나 어떤 작업이 일정 시간 내에 마무리 되었는지 아니면 일정 시간 이후에나 요청을 수락할 때 등에 필요합니다.
* 앞서 f1()는 chrono::miliseconds(10 * 1000)을 했으나 이름 chrono::seconds(10)으로 변경해도 동일합니다.

### Thread의 Join

* main thread는 t2.join() 와 t3.join() 을 호출합니다.
* Thread 는 앞서 설명한 life cycle에 의해 최종적으로 terminated 상태가 됩니다.
* join()은 대상이 되는 thread가 terminated 상태가 될 때까지 기다립니다.
* 또한 **`join() 후 terminated 된 thread의 메모리를 정리합니다.`**

    ```
    - 자기 쓰레드에 대하여 join() 할 수 없습니다.
    - 이미 join() 한 thread에 다시 join 할 수 없습니다.
    - Detach 되지 않은 thread는 반드시 join() 으로 정리되어야 합니다.
    ```

### Join 을 하지 않으면 Exception 발생?
* Thread 는 다양한 프로그래밍 언어에서 지원합니다.
* Python은 GIL (Global Interpreter Lock) 때문에 아직까지는 진정한 Thread 지원을 하고 있지 않습니다.

## `Test #5`
### Join() 의 역할

```c++
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void f1() {
    cout << "f1: " << this_thread::get_id() << endl;
    this_thread::sleep_for(chrono::milliseconds(10 * 1000));
    cout << "f1: woke up" << endl;
}

void f2(int arg){
    cout << "f2: " << arg << endl;
}

int main() {
    auto start = chrono::system_clock::now();

    thread t1;
    thread t2(f1);
    thread t3(f2, 10);
    cout << "C++ id: " << t3.get_id() << endl;
    cout << "Native id: " << t3.native_handle() << endl;

    auto end = chrono::system_clock::now();
    auto duration = end - start;
    auto seconds = chrono::duration_cast<chrono::seconds>(duration).count();
    cout << "Elapsed: " << seconds << endl;
}
```
* Thread 생성, 제거를 반복하는 코드 스타일에서 Join을 빠트리면 리소스를 계속 잡아먹습니다.

* Thread는 만들  어지고 나서는 코드를 중단할 수 없습니다.

### 생성한 Thread 중간에 종료하기
* 생성한 작업을 thread를 중간에 종료할 수 있을까? 불가능 합니다.
* C++ class thread 는 OS의 thread에 대응되는 객체일 뿐입니다.
* 따라서 그 객체를 소멸시킨다고 OS thread도 자동으로 종료되는 것이 아닙니다.

#### 그렇다면 만들어진 작업 thread를 어떻게 중간에 종료할 수 있을까?

`Main thread`와 `작업 thread`가 `Boolean flag`를 공유하면서 종료 상황을 전달하게 프로그래밍을 해야 합니다. <br>

* 이때 `flag`는 값을 쓰는 thread, 그걸 읽는 thread 두개가 존재하는 것이므로 실행 순서에 따라 동작이 달라집니다. (= `race condition`)
* 따라서 `race condition` 이 없도록 `atomic한 방식`을 써야 합니다. <br>
&rarr; C++에서는 std::atomic<bool> 같은 것을 사용할 수 있습니다.

## `Test #6`
### Boolean Flag로 Thread 중간에 종료하기

```c++
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

atomic<bool> quit{ false };

void f() {
    for (int i=0 ; quit.load() == false && i < 5 ; ++i) 
    {
        cout << "Running: " << this_thread::get_id() << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));    
    }
}

int main(){
    thread t(f);
    this_thread::sleep_for(chrono::milliseconds(2500));
    quit.store(true);
    t.join();
}
```

### 쓰레드 간 경합 (Contention)

* 복수 개의 쓰레드가 공유될 수 없는 같은 자원에 접근하려고 하는 것입니다.

### 레이스 컨디션 (Race Condition)

* 연산의 결과가 쓰레드의 실행 순서에 의해 달라지는 것입니다.
* 복수 개의 쓰레드가 공유될 수 없는 같은 자원을 동시에 접근함에 따라 발생하는 예상치 못한, 대개의 경우 에러를 유발하는 상황입니다.
* 동기화(Synchronization)을 통해 해결합니다.

## `Test #7`
### Multi-Threaded 에서 Race Condition

```c++
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

int sum = 0;
void f() {
    for (int i=0 ; i< 10 * 1000 * 1000 ; ++i){
        ++sum;
    }
}

int main() {
    thread t(f);
    for (int i = 0 ; i< 10 * 1000 * 1000 ; ++i){
        ++sum;
    }
    t.join();
    cout << "Sum: " << sum << endl;
}
```
&rarr; 결과 값은 2천만이 나오지 않습니다. 

### 동기화(Synchronization)를 구현하기 위한 요소
* Mutex (Mutual Exclusion)
    * 어떤 자원에 접근하기 위한 배타적 권한을 표현하기 위하여 사용됩니다.
    * 즉, 자원에 접근하기 전에 뮤텍스를 잡고, 자원 접근 후 뮤텍스를 풀어줍니다.

* Condition variable - **무조건 Mutex와 같이 사용**
    * 다른 쓰레드에 의해 생성되는 조건(condition)을 기다리기 위해 사용됩니다.
    * 여러 쓰레드가 "조건" 발생에 대해 통신하는 수단이므로 (동시에 여러 쓰레드가 읽고 쓰는 것을 막기 위해) 반드시 `mutex`를 필요로 합니다.
    &rarr; 같은 Mutex가 여러 condition variable을 담당해도 괜찮습니다.

* 그 밖에 OS별로, 프로그래밍 언어 별로 `Critical Section`과 같은 다른 형태의 동기화 요소를 제공하기도 함

### C++에서 Mutex 지원

* class mutex : 기본 뮤텍스 구현
* class recursive_mutex : 같은 쓰레드가 반복해서 잡아도 괜찮은 뮤텍스
* class timed_mutex: 정해진 시간 동안만 보장해주는 뮤텍스
* class recursive_timed_mutex : 위 두 함수를 합친 클래스
    ```
    한번 잡으면 그 잡은 것이라고 하더라도 다시 못 잡습니다. (recursive)
    락을 한번 놓치면 영원히 풀리지 않는다. 문제가 되어 시간을 걸어주는 함수도 지원합니다. (timed)
    부하가 있으므로 기본 mutex 를 사용하는 것을 권장합니다.
    ```

### Mutex 사용 코드 패턴

* 보호 되어야 되는 부분은 `시작 부분에서 "lock"`, `끝 부분에서 "unlock"` 합니다.
* 이 때 경합하는 모든 쓰레드들이 동일한 Mutex를 사용해야 합니다.

```c++
#include <mutex>

mutex m;        // 공유될 자원에 대한 뮤텍스
                // 경합하는 쓰레드들은 모두 같은 뮤텍스를 써야한다.

...
m.lock();       // 뮤텍스를 잠금
...             // 공유자원에 대한 접근 코드
                // 오직 한번에 한 쓰레드만 여기를 실행합니다.
m.unlock();     // 뮤텍스를 품
```

## `Test #8`
### Mutex 사용

```c++
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

int sum = 0;
mutex m;
void f() {
    for(int i=0 ; i < 10 * 1000 * 1000 ; ++i) {
        {
            unique_lock<mutex> ul(m);
            ++sum;
        }
        // m.lock();
        // ++sum;
        // m.unlock();
    }
}

int main() {
    thread t(f);
    for(int i=0 ; i<10 * 1000 * 1000 ; ++i){
        {
            unique_lock<mutex> ul(m);
            ++sum;
        }
        // m.lock();
        // ++sum;
        // m.unlock();
    }
    t.join();
    cout << "Sum: " << sum << endl;
}
```

### C++에서 mutex의 문제점

* unlock() 호출이 누락될 경우 영원히 어떤 쓰레드도 접근하지 못하게 됩니다.
    * 실수로 누락된 경우
    * C++ exception 발생으로 코드가 `unwinding` 된 경우

### C++ 의 class unique_lock 활용

* 생성자에서 mutex 를 잠그고, 소멸자에서 해제하는 역할을 합니다.
* 따라서 명시적으로 lock 을 잠그고 해제하는 것이 아니라 아래처럼 scope을 만들어서 { ... } 로 묶어주기만 하면 됩니다.

### C++ class unique_loc 관련 빈번한 실수

<details>
<summary> unique_lock<mutex> ul(m) 대신 unique_lock<mutex>(m)을 쓰는 경우 버그가 발생합니다. 왜일까? </summary>
&rarr; 디버깅도 되지 않습니다. 락을 사용했는데 동기화가 안됩니다. <br>
이 변수를 만들고 이 scope 안에서만 유효한데, 임시 변수를 만들고 바로 없앱니다. 생성자 한번 부르고 끝인 점을 주의해야 합니다.
</details>

### 교착 (Deadlock)

* 두 개 이상의 작업이 서로 상대방이 배타적 독점한 자원을 기다리느라 결국 어떤 작업도 끝내지 못하는 상태

* `Dining Philosophers Problem (식사하는 철학자 문제)`

<details>
<summary> 교착이 발생하기 위한 4가지 필요조건? (모두 갖춰야 함) </summary>

1. 상호배제 (Mutual Exclusion) : 자원에 대한 배타적 독점 <br>
2. 점유대기 (Hold & Wait) : 자기가 가진 자원을 놓지않고 다른 자원을 기다림 <br>
3. 비선점 (No Preemption) : 작업을 가진 자원을 뺏을 수 없음 <br>
4. 순환 대기 (Circular Wait) : 자원에 대한 의존성이 순환적임 <br>
</details>

**`현실적으로 이 4가지 중 프로그래머가 방지 가능한 일은 "순환 대기" 입니다.`** <br>
&rarr; 따라서 복수 개의 lock을 다룰 때 lock 순서를 반드시 일치 시켜서 순환 대기를 방지해야 합니다.

#### 순환 대기 
```c++
void f() {
    m1.lock();
    m2.lock();
    ...unlock 코드 생략
}

void g() {
    m2.lock();
    m1.lock();
    ...unlock 코드 생략
}
// 높은 확률로 Dead Lock 발생
```

#### Locking 순서로 순환 대기 회피
```c++
void f() {
    m1.lock();
    m2.lock();
    ...unlock 코드 생략
}

void g() {
    m1.lock();
    m2.lock();
    ...unlock 코드 생략
}
```

```
공동작업을 하는데 동기화는 필수고, 동기화를 하는데 뮤텍스는 필수인데 
뮤텍스를 여러 개 사용하는 경우가 반드시 발생합니다.
그때는 순서는 무조건 지켜야 합니다. 
지키지 않으면 무조건 데드락에 걸립니다.
```

#### Unlock까지 표시한 경우
```c++
void f() 와 g() 모두 {
    m1.lock();
    m2.lock();
    ...공유 데이터에 대한 작업
    m2.unlock();
    m1.unlock();
}
```

## `Test #9`
### 순환 대기

```c++
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

int sum = 0;
mutex m;
mutex m2;

void f() {
    for (int i = 0 ; i < 10 * 1000 * 1000 ; ++i){
        m.lock();
        m2.lock();
        ++sum;
        m.unlock();
        m2.unlock();
    }
}

int main() {
    thread t(f);
    for(int i=0 ; i < 10 * 1000 * 1000 ; ++i){
        m2.lock();
        m.lock();
        ++sum;
        m2.unlock();
        m.unlock();
    }
    t.join();
    cout << "Sum: " << sum << endl;
}
```
### Conditionn Variable

* 어떤 조건(condition) 이 발생할 때까지 thread를 중단할 때 쓰입니다.
* 조건을 `wait()` 하는 쓰레드와 조건이 발생했음을 `notify()` 하는 쓰레드가 있습니다.
* **`반드시 대응되는 mutex를 잡은 후 wait()이나 notify()를 합니다.`** <br>
&rarr; 이 말은 CV는 단독으로 쓰이지 않고 반드시 mutex와 같이 사용된다는 의미입니다.
* `wait()`은 호출하면 자동으로 mutex를 풀고 쓰레드를 block 시킵니다. (= 재움) <br>
**`그리고 반환할 때 다시 자동으로 mutex를 잡은 상태로 깨어납니다.`**

* **`notify()를 호출한 뒤에는 반드시 Mutex를 해제하도록 코딩해야 합니다.`** <br>
&rarr; notify()를 한다 하더라도 mutex를 해제해야만 상대가 wait()에서 깨어납니다.

### Condition Variable 사용패턴 (C++ 예시)
```c++
void consumer() {
    // m.lock()
    unique_lock<mutex> lg(m);


    // lock을 잡은 후 호출
    // wait 은 쓰레드를 sleep
    // 깨어날 때 다시 락을 잡은 상태로 만듦
    cv.wait(m)

    // lock이 잡힌 상태. 작업 ON.

    ...
    m.unlock() // lock 이 여기서 해제
}
```
```c++
void producer() {
    // m.lock()
    lock_guard<mutex> lg(m);

    // 작업 진행
    ...
    // lock 잡은 후 호출
    // 호출한다고 lock을 놓지는 않음
    cv.notify()

    m.unlock() // 이제 lock 을 품, 상대는 wait() 에서 깨어남
}
```

### C++ CV의 notify_one() vs notify_all()

* condition_variable 에 대해 wait() 하는 순간 CPU queue 에서 condition_variable의 waiting queue 로 쓰레드가 옮겨집니다.

* notify_one() 은 이 waiting queue 에 있는 것 중 맨 앞 쓰레드 하나를 깨웁니다.

    ```
    대개 notify는 뭔가를 생산하는 쪽에서 호출하는 데,
    만일 여러 쓰레드가 소비할 정도로 충분히 생산한 경우 notify_all()을 호출하고 
    한 쓰레드만을 위한 경우라면 notify_one() 을 호출하는 것이 좋습니다.
    ```

* `만일 notify_all() 을 호출했는데 한 쓰레드만 가능하다면?` <br>
&rarr; 나머지 쓰레드는 다시 잠들어야 합니다.

## `Test #10`
### Condition Variable 사용

```c++
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>

using namespace std;

int sum = 0;
mutex m;
condition_variable cv;

void f(){
    for (int i = 0; i < 10 * 1000 * 1000 ; ++i){
        ++sum;
    }
    {
        unique_lock<mutex> lg(m);
        cv.notify_one();
    }
}
 
int main() {
    thread t(f);
    {
        unique_lock<mutex> lg(m);
        cv.wait(lg);
        // cout << "Sum: " << sum << endl; 
        // 만일 빈번하게 도는 코드였다면 해당 로그 코드는 큰 문제
    }
    cout << "Sum: " << sum << endl;
    t.join();
}
```

## 