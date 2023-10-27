#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

const int NO_JOB = -1;

// 쓰레드 종료 조건 조율을 위한 flag
// boolean 이라고 하더라도 atomic 하지 않기 때문에 atomic<bool> 을 이용
atomic<bool> quit(false);

// 공유 자원인 queue
// 공유 자원이 있다면 이를 보호하기 위한 Mutex
// 그리고 자원의 상태(condition)에 변화가 생기는 것은 condition_variable 로 통신
// condition_variable은 mutex 반드시 필요
// 여러 개의 condition_variable 이 Mutex 하나를 쓰는 지금 상황에 주목할 것
mutex queMutex;
condition_variable queFillable;
condition_variable queFilled;
int que[1] = { NO_JOB };

// producer thread 의 thread 함수
void producer() {
    cout << "Producer starting. Thread id: " << this_thread::get_id() << endl;

    // thread 종료 flag가 켜질 때까지 동작 시킴
    while(quit.load() == false){
        int job = rand() % 100;
        {
            unique_lock<mutex> ul(queMutex);
            while(que[0] != NO_JOB){
                queFillable.wait(ul);
            }
            que[0] = job;
            queFilled.notify_one();
            cout << "Produced: " << job << endl;
        }
    }
    cout << "Producer finished" << endl;
}

// consumer thread 의 thread 함수
void consumer() {
    cout << "Consumer starting. Thread id: " << this_thread::get_id() << endl;

    // thread 종료 flag가 켜질 때까지 동작 시킴
    while(quit.load() == false){
        int job;
        {
            unique_lock<mutex> ul(queMutex);
            while(que[0] == NO_JOB){
                queFilled.wait(ul);
            }
            job = que[0];
            que[0] = NO_JOB;
            queFillable.notify_one();
            cout << "Consumed: " << job << endl;
        }
    }
    cout << "Consumer finished" << endl;
}

int main(){
    cout << "Main thread started. Thread id: " << this_thread::get_id() << endl;

    // 랜덤 생성기의 초기값을 지정한다
    srand(time(NULL));

    // Producer / consumer 쓰레드의 핸들을 저장할 C++ 측 객체
    thread t1(producer);
    thread t2(consumer);
    

    this_thread::sleep_for(chrono::seconds(5));
    
    quit.store(true);

    if(t1.joinable()){
        t1.join();
    }

    if(t2.joinable()){
        t2.join();
    }

    cout << "Main thread finished" << endl;
}