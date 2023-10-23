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
    // t3.join을 하지 않으면 메모리 / 자원 누수
 
    return 0;
}

