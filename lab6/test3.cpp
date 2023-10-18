#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

// namespace this_thread {
//     // 현재 쓰레드 식별자를 반환
//     thread::id get_id();

//     // CPU를 포기하고 현재 thread 를 CPU ready queue 에 넣음
//     void yield();

//     // 현재 쓰레드를 일정 시간 동안 잠재움
//     void sleep_until(const chrono::time_point & abs_time);
//     void sleep_for(const chrono::duration & rel_time);
// }

void f1() {
    cout << "f1: " << this_thread::get_id() << endl;
    this_thread::sleep_for(chrono::milliseconds(10 * 1000));
    cout << "f1: woke up" << endl;
}

void f2(int arg){
    cout << "f2: " << arg << endl;
}

int main(){
    thread t1;
    thread t2(f1);
    thread t3(f2, 10);
    cout << "C++ id: " << t3.get_id() << endl;
    cout << "Native id: " << t3.native_handle() << endl;

    t2.join();
    t3.join();

    return 0;
}

