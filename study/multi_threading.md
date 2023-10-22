## Multi Threading

### Task Queue + Workers Pool
Worker는 thread로 구현되거나 Process로 구현될 수 있습니다.

#### thread를 이용한 방식
* 서버 process 하나 안에 여러 worker thread가 존재하는 구조입니다. <br>
* 같은 process 내에 존재하므로 메모리가 공유되어 다루기 편합니다. <br>
* Task queue 역시 process 안의 공유 메모리로 존재하는 자료 구조로 구현이 가능합니다.

#### Process를 이용한 방식
* Worker thread 별로 process 를 띄우는 방식
* Process 간에는 메모리가 공유되지 않음
* Task queue 역시 별도의 process 로 존재함 (ex. ZeroMQ, RabbitMQ)

기본적으로 Multi threading을 할 때 `스레드 간의 동기화를 한다`고 한다면 `Task Queue`와 `Worker Pool`을 떠올리면 된다.

### CPU 관련 중요 Metrics

#### CPU utilization (또는 Usage)
* (특정 시점에) CPU 처리 용량(capacity) 대비 CPU 가 얼마나 사용되고 있는지를 의미합니다. <br>
&rarr; 의미있는 작업만을 포함하는 것이 아니라, 의미없이 busy waiting 하는 경우도 모두 계산됩니다.

* CPU Utilization = 100% - "CPU idle 비율" (ex. idle = 90% &rarr; utilization = 10%)

* 전체 CPU 숫자 (= core 숫자) 기준인지 CPU 1개 기준인지 구분해야 합니다. 
    * 리눅스의 경우 CPU 1개 기준 숫자입니다. (ex. 4 vCPU &rarr; 400가 최대치)
    * ex. 4 vCPU &rarr; 400% 서버 에서 utilization은 10%입니다.

1. OS 전체 수준에서의 utilization &rarr; 세부 내역으로 구분되므로 단순 utilization 계산은 100% - idle 로 계산
    > $ top <br>
    > %Cpu(s): 0.5 us (user 약자), 0.7 sy (system 약자), 98.8 id (idle 약자)
    <br>

    <img width="598" alt="스크린샷 2023-10-16 16 20 37" src="https://github.com/almond0115/clean-code/assets/77084379/1c8f1c4a-3e3d-4a99-a044-b9c73b08ec17">
    <br><br>

    > $mpstat (또는 코어를 다 표시하고 싶으면 -P ALL 옵션 추가)
    <br>

    <img width="691" alt="스크린샷 2023-10-16 16 32 06" src="https://github.com/almond0115/clean-code/assets/77084379/73288f4d-5216-4331-9fc2-9273fc9b3f5d">
<br>

2. 특정 Process 의 utilization
    > $ ps -u$USER (내 유저와 관련된 모든 프로세스)
    <br>

    <img width="404" alt="스크린샷 2023-10-16 16 42 28" src="https://github.com/almond0115/clean-code/assets/77084379/f7279e6c-371b-40f8-abac-b046652106df">
    <br><br>

    > $ ps -o pcpu,pid,user,args
    <br>

    <img width="1282" alt="스크린샷 2023-10-16 16 45 21" src="https://github.com/almond0115/clean-code/assets/77084379/69bc3208-7e60-4583-9bf0-badbe1be0dda">
    <br><br>

    > $ ps -o pcpu,pid,user,args -u$USER | grep node 
    <br>

    <img width="1404" alt="스크린샷 2023-10-16 16 48 19" src="https://github.com/almond0115/clean-code/assets/77084379/60eee44a-b694-4b38-98e4-cd09371acc10">
    <br><br>

    > $ htop (시각적으로 좀 더 예쁘게 보여줍니다)
    <br>

    <img width="1402" alt="스크린샷 2023-10-16 16 49 52" src="https://github.com/almond0115/clean-code/assets/77084379/bb9e1315-c161-426e-a764-8b1bc99bced2">

#### CPU Load

* 특정 시점에 CPU를 사용하거나 기다리고 있는 프로세스 수 <br>
&rarr; CPU Utilization은 CPU가 놀고있는지 아닌지를 보기 때문에 얼마나 뒤에 기다리고 있는지는 알 수 없습니다.

* 그러나 `특정 시점`보다 `평균치`가 더 유용합니다. (최근 1분/5분/15분 평균 CPU Load)
    ```
    15분, 5분, 1분을 보았는데 15분에는 부하가 컸는데, 5분은 좀 적고, 1분은 좀 줄었다면,
    부하가 점점 낮아지고 있는 패턴임을 확인할 수 있습니다.  
    ```
* `load average` 확인하는 명령어
    > $ w (work의 약자)
    <br>

    <img width="506" alt="스크린샷 2023-10-16 17 47 28" src="https://github.com/almond0115/clean-code/assets/77084379/49f71f36-3d8f-4c5b-a168-81add34f8639">
    <br>

    `load average는 1분, 5분, 15분을 의미합니다.`
    <br><br>

    > $ uptime
    <br>

    <img width="486" alt="스크린샷 2023-10-16 17 54 40" src="https://github.com/almond0115/clean-code/assets/77084379/f336022c-3184-474f-9e78-cc24c88d12de">
    <br><br>

    > $ top
    <br>

    <img width="598" alt="스크린샷 2023-10-16 16 20 37" src="https://github.com/almond0115/clean-code/assets/77084379/1c8f1c4a-3e3d-4a99-a044-b9c73b08ec17">
    <br><br>


* 해석
    * CPU load 수치 <= CPU 개수 (ex. 4vCPU 인테 CPU Load 가 1.0) <br>
        ```
        CPU를 지금 쓰거나 기다리고 있는 개수가 1 
        CPU가 4개라고 하면 최대 4개까지 쓰거나 기다릴 수 있는 것입니다.
        ```
        &rarr; 모든 프로세스가 CPU를 바로바로 할당 받고 있습니다.
    * CPU load 수치 > CPU 개수 (ex. 4vCPU 인테 CPU Load 가 5.0) <br>
        ```
        이 상황에서 CPU utilization은 몇일까? 대충 100을 찍습니다.
        지난 수업에 말했던 CPU 사용량 목표치 50~70%는 이 CPU load 값을 말합니다.
        CPU 한계 기준으로 0.5 ~ 0.75 를 타겟팅 해야합니다.
        ```
        &rarr; CPU를 바로바로 할당 받지 못하고 잠깐 대기하는 프로세스들이 있습니다.
        <br><br>

#### CPU Utilization과 CPU Load 와의 관계
* Utilization 은 현재 CPU 사용 여부만 감안하고, load는 그것 외에 대기 여부까지 감안합니다.
* 1개 CPU 기준으로 설명하면 <br>
&rarr; load 가 1 이하 : utilization 100% 이하 <br>
&rarr; load 가 1 초과 : utilization 은 거의 항상 100
* 따라서 utilization 만으로는 파악이 어렵고 load 를 같이 고려해야 합니다.   
ex) CPU가 100% 인데 load가 1.0 일 때와 load가 10.0 일 떄

* 대개 1개 CPU 기준으로 50% ~ 70% 정도의 utilization 을 목표로 합니다. <br>
&rarr; Load 로는 대략 0.5 ~ 0.7 정도가 됩니다. <br>
&rarr; 4 vCPU 의 경우 4배 곱하여 utilization 200% ~ 280%, load 2.0 ~ 2.8이 됩니다.

#### CPU Load 가 높을 때 증상
* 실행 중인 모든 프로세스의 반응이 느립니다. <br>
&rarr; 경우에 따라 shell 이 멈춘 것처럼 느립니다. <br>
&rarr; 프로세스가 로그를 찍을 때도 느립니다. 

* 프로세스를 시작할 때 바로 뜨지 않고 약간의 시차가 있습니다. <br>
&rarr; 새로 프로세스를 시작하면 마찬가지로 CPU 대기열에 들어가기 때문입니다.
&rarr; shell 명령어 입력하였는데 늦게 실행됩니다.

* 과열이 지속되면 OS가 리부팅 됩니다. <br>
&rarr; 클라우드 환경에서는 이를 직접적으로 느끼기 어렵지만, 데이터 센터 내에서 물리적으로 CPU 과열로 시스템이 리부팅되기도 합니다.

## 용어 정리

### 멀티 프로그래밍 (Multi-Programming)
* CPU 이용률 (utilization) 을 높이기 위한 방식입니다. 
* CPU 에는 하나의 프로그램만 돌고, 나머지 프로그램들은 대기열에서 관리합니다.
    ```
    프로세스 수준에서 block 되는 것은 잠자는 것을 의미합니다.
    오래 걸릴 것 같으니 CPU에서 내려달라는 뜻입니다. (내가 원하는 I/O 작업)
    네트워크도 외부에서 입출력이 들어오므로 I/O 입니다.
    "그런 I/O 작업이 끝나면 나를 깨워줘" -> blocked 되는 상황입니다.
    ```
* 이 때문에 돌고 있던 프로그램이 무엇인가(=resource)를 대기해야 되는 경우, 어떤 프로그램도 CPU를 사용하지 못하게 되어 CPU 이용률이 낮아집니다.
* 따라서 CPU 상의 프로그램이 잠시 멈춰야 될 때가 발생하면 <br>
&rarr; 해당 프로그램을 CPU 에서 내리고 대기열 맨 뒤로 집어 넣습니다. <br>
&rarr; 대기열 맨 앞의 프로그램을 꺼내서 CPU 에 올리고 실행합니다. <br>

* 이렇게 CPU 상에 올라가는 프로그램이 변경될 때 `컨텍스트 스위칭` 이 필요합니다.
    ```
    CPU 입장에서는 돌리고 있던 프로세스가 바뀐 것입니다.
    그러니 해당 프로세스를 컨텍스트라고 이야기 합니다.
    ```

### 컨텍스트 스위칭
* 컨텍스트 (Context): <br>
&rarr; CPU 가 현재 실행하고 있는 기계어 코드 상태들 : CPU registers, Program counter (Instruction pointer)...
    ```
    함수 이름이 포인터, 즉 코드의 주소입니다. 
    작성하는 코드는 CPU에 다운로드 할 수 없습니다.
    내가 만든 실행파일이 10MB라고 한다면 CPU에 올리는 것은 불가능하고 메모리에 올려야 합니다.
    CPU에서는 다음 실행할 명령어를 찾는 방법은 그 메모리에 내가 무엇을 실행하고 있는지를 PC로 가리키고 있다가 메모리에서 읽어와서 처리합니다.
    옛날에는 하나씩 읽어 왔는데 요즘 CPU는 덩어리로 읽어옵니다. 
    요리할 때 도마나 접시 처럼 올릴 수 있는 것을 레지스터라고 합니다.
    ```
    <br>

* 컨텍스트 스위칭 (Context Switching) <br>
&rarr; 기존에 있던 Context를 메모리에 저장하고, 메모리에 저장되어 있던 것으로 context를 변경하는 것입니다. <br>
&rarr; CPU 가 실행하는 프로그램을 변경할 때 발생합니다.
    ```
    CPU 입장에서는 부하가 굉장히 큰 작업입니다.
    Context Switching이 많이 발생하면 CPU 입장에서는 비효율적입니다.
    CPU는 아무 것도 안하고 있으니 Utilization은 내려갑니다.
    Context Switching을 계속하면 CPU 사용량은 계속 떨어지고 아무 일도 못합니다.
    ```

### 멀티 태스킹
* `멀티 프로그래밍`은 프로그램이 뭔가를 (=resource) "기다릴 때"까지 해당 프로그램이 CPU를 계속 사용하는 것을 허용합니다. (즉, 컨텍스트 스위칭 X) <br>
&rarr; 단점: 대기열에 있는 다른 프로그램들은 CPU를 받지 못하고 한정없이 기다립니다. <br>
&rarr; 이렇게 무언가를 받지 못하고 한정 없이 기다리는 것을 "기아(starvation)" 라고 합니다. <br> 따라서 이 경우는 "나머지 프로그램들은 `CPU starvation` 상태다" 라고 합니다.

* `멀티 태스킹`은 프로그램이 스스로 기다리는 상태가 되지 않더라도, <br> 
OS가 강제로 컨텍스트 스위칭을 일으켜 대기열에 있는 프로그램 모두 실행되고 있는 것 같은 환상을 주는 방식입니다.

* 주로 `time sharing` 방식 (또는 time slicing 방식)을 이용합니다. &rarr; `시분할` <br>
&rarr; CPU를 점유할 수 있는 시간을 잘게 쪼개서 여러 프로그램이 이 시간 동안만 CPU를 사용하게 하는 것입니다. ("time quantum" or "time slice")

* `동시성(Concurrency)`은 실제로는 동시에 실행되지 않으나 그렇게 보이는 것을 말합니다.
    ```
    멀티 프로그래밍은 여러 프로그래밍이 실행되더라도 앞의 것이 sleep 상태가 되어야 다음 프로그램을 사용할 수 있기 때문에 동시에 사용하는 기분은 줄 수 없습니다.
    ```

### 멀티 프로세싱 (Multiprocessing)
* `멀티 태스킹`은 1개의 CPU 만으로라도 여러 task를 실시간 실행하는 것 같은 환상을 줄 수 있습니다. <br> 
&rarr; 동시성 (Concurrency)

* 이 떄 2개 이상의 CPU를 사용하는 것을 `Multiprocessing` 이라고 합니다. <br>
&rarr; 동시성과 달리 실제로 동시에 실행되며 이를 `병렬성(parallelism)` 이라고 합니다.

* 대기열에 있는 task는 context switch 로 CPU를 배정 받을 때 이전과 다른 CPU를 배정 받을 수 있습니다.

    ```
    CPU pinnning (또는 Processor affinity) : 특정 task가 언제나 같은 특정 CPU를 배정 받게 하는 것
    ```

### 쓰레드 (Thread)

* 쓰레드란 독립된 실행 흐름 입니다. &rarr; 독립된 코드 실행 흐름 (마치 각각의 실과 같습니다.)
* 하나의 프로세스 안에서 동시성 / 병렬성을 개선합니다.
* 각 쓰레드는 독립된 `프로그램 카운터 (PC)`, `CPU registers`, `Stack` 을 가집니다.
* 쓰레드 간 `코드 영역`, `데이터 영역`, `파일/소켓` 등의 OS 자원을 공유
    ```
    Process와 달리 모든 것이 독립적으로 할당되어야 하는 것은 아니기에
    `Lightweight Process`라고 불리기도 합니다.
    ```

### 쓰레드 구현 방식

* 쓰레드 관리 (= 생성 / 소멸)와 스케쥴링, 컨텍스트 스위칭 방식에 따라서 2가지 방식이 있습니다.

1. 유저 레벨 (User level) - 코루틴 (co-routine)
    * 쓰레드의 `생성 / 소멸 관리`, `스케쥴링`, `컨텍스트 스위칭`을 유저 레벨의 라이브러리에 구현
    *  Kernel은 이에 대한 정보를 전혀 갖지 않고 Process 단위로 스케쥴링 함
    * CPU를 할당 받으면 process 내 쓰레드 라이브러리가 실행할 유저 쓰레드를 선정하고 실행될 쓰레드의 상태를 복구해줍니다. &rarr; 유저 레벨의 컨텍스트 스위칭
        ```
        - 장점
        1. 쓰레드 생성/소멸, 스위칭에 커널이 개입하지 않으므로 빠릅니다.
        ex. System Call 호출 (커널 개입 작업)은 느림
        2. 쓰레스 스케쥴링 알고리즘을 프로세스가 제어할 수 있습니다.
        3. OS 에 독립적 구현이 가능합니다.

        - 단점
        1. 어느 한 쓰레드가 blocking이 되면 프로세스 전체가 blocking 됩니다. 
        2. 커널 상에서는 하나의 쓰레드로만 존재하므로 멀티 프로세싱의 병렬성 혜택을 볼 수 없습니다. 
        ```
        ```
        간식을 사오라고 10만원을 주었는데, 과 대표가 그 돈을 먹었다면 10만원을 준 사람은 그 사실을 모릅니다. 그런 문제가 생길 수 있다는게
        유저 레벨 쓰레의 단점입니다.
        ```
2. 커널 레벨 (Kernel level) - Windows, Linux
    * 쓰레드의 `생성/소멸 관리`,  `스케쥴링`, `컨텍스트 스위칭`을 커널이 관리합니다. <br> (커널이 쓰레드별로 CPU Queue에 넣고, 컨텍스트 스위칭 역시 쓰레드별로 복구해줍니다.)
        ```
        - 장점
        1. 쓰레드 별로 CPU를 할당하므로, 
        멀티프로세싱 환경에서 동시에 여러 CPU를 쓸 수 있습니다. (병렬성)
        2. 프로세스 내에 한 쓰레드가 blocking 되어도 다른 쓰레드는 계속 동작합니다.

        - 단점
        1. 쓰레드의 생성/소멸에 커널이 관여하므로 느립니다.
        2. 쓰레드 간의 스위칭 역시 커널이 관여하므로 느립니다.
        3. 쓰레드 구현이 OS에 의존적입니다.
        ```


