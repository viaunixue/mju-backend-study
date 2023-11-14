#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <iostream>

using namespace std;

constexpr int WORKER_THREAD_NUMBER = 2;

void message_worker_thread(int id){
    cout << "메세지 작업 쓰레드 #" << id << "생성\n";
    // 메세지 작업 쓰레드 동작 구현
}

int main(){
    vector<thread> worker_threads;

    for(int i=0 ; i < WORKER_THREAD_NUMBER ; ++i){
        worker_threads.emplace_back(message_worker_thread, i);
    }

    cout << "Port 번호  9130 에서 서버 동작 중\n" << endl;

    // 메세지 작업 쓰레드 종료 대기
    for (auto& thread : worker_threads){
        thread.join();
    }

    return 0;
}