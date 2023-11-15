#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <map>
#include <set>
#include <csignal>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>

using namespace std;

static const uint16_t PORT = 19130;
constexpr int WORKER_THREAD_NUMBER = 2;

// 쓰레드 종료 조건 위한 플래그
atomic<bool> quit(false);

// 공유 자원인 큐와 관련된 Mutex 및 조건 변수
mutex msgQueMutex, clientSocksMutex, willCloseMutex;
condition_variable msgQueFilled;
queue<string> msgQueue;
set<int> willClose;

// Producer
void ProduceMessage(const string& msg){
    unique_lock<mutex> ul(msgQueMutex);
    msgQueue.push(msg);
    msgQueFilled.notify_one();
}

// Consumer
void ConsumeMessage(int clientSock){
    while(!quit.load()){
        string message;
        {
            unique_lock<mutex> ul(msgQueMutex);
            while(msgQueue.empty()){
                msgQueFilled.wait(ul);
            }
            message = msgQueue.front();
            msgQueue.pop();
        }
        // 클라이언트에게 메세지 전송
        // TODO : send 함수 실패 처리 추가 필요
        if(send(clientSock, message.c_str(), message.size(), 0) < 0){
            cerr << "send() failed : " << strerror(errno) << ", client_sock : " << clientSock << endl;
            // 실패 처리
            unique_lock<mutex> ul(willCloseMutex);
            willClose.insert(clientSock);
            break;
        }
    }
}

struct ClientInfo {
    int sock_num;
    string nickname;
    time_t last;
};

// 현재 연결된 client socket (active socket)
map<int, ClientInfo> clientSocks;

vector<thread> worker_threads;

// 명령어 처리 함수
void ProcessCommand(int client_sock, const string& command){
    // 명령어 따라 적절한 동작 선택
    // 예) /name, /rooms, /create, /join, /leave, /shutdown 등
}

void MessageWorkerThread(int worker_id){
    cout << "메세지 작업 쓰레드 #" << worker_id << "생성\n";
    while(!quit.load()){
        // 메세지 작업 수행
        this_thread::sleep_for(chrono::milliseconds(100));
    }
} 

// 클라이언트와 통신 
void CommunicateWithClient(int clientSock){
    while(true) {
        char buf[65536];
        int numRecv = recv(clientSock, buf, sizeof(buf), 0);

        if (numRecv == 0){
            cout << "Socket closed : " << clientSock << endl;
            unique_lock<mutex> ul(willCloseMutex);
            willClose.insert(clientSock);
            break;
        } else if (numRecv < 0) {
            cerr << "recv() failed : " << strerror(errno) << ", client_sock : " << clientSock << endl;
            unique_lock<mutex> ul(willCloseMutex);
            willClose.insert(clientSock);
            break;
        } else {
            cout << "Received : " << numRecv << " bytes, client_sock : " << clientSock << endl;

            // 클라이언트로부터 받은 데이터 분리
            string receivedData(buf, numRecv);
            size_t pos = receivedData.find('\n');
            if (pos != string::npos){
                string command = receivedData.substr(0, pos);
                string chatMessage = receivedData.substr(pos + 1);

                if(command[0] == '/'){
                    // 명령어인 경우 처리
                    ProcessCommand(clientSock, command);
                } else {
                    // 일반 메세지 -> broadcasting
                    ProduceMessage(receivedData);
                }
            }
        }
    }
    // 종료 전 쓰레드 생성 및 쓰레드 생성 및 detach()
    thread(ConsumeMessage, clientSock).detach();
}
// 클라이언트와 연결
void HandleClientConnection(int serverSock){
    while (true){
        fd_set rset;                    // select() 호출 위한 fd_set 변수 생성
        FD_ZERO(&rset);                 // 자료구조 초기화
        
        FD_SET(serverSock, &rset);    // 자료구조에 소켓 등록
        int maxFd = serverSock;            

        for (auto it = clientSocks.begin() ; it != clientSocks.end() ; ++it) {
            int clientSock = it -> first;
            if (clientSock > maxFd){
                maxFd = clientSock;   
            }
        }

        // select()
        struct timeval tv = {0, 1000};
        int numReady = select(maxFd + 1, &rset, NULL, NULL, &tv);
        if(numReady < 0){
            cerr << "select() failed : " << strerror(errno) << endl;
            continue;
        } else if (numReady == 0){
            continue;
        }

        // 새 클라이언트 연결 수락
        if(FD_ISSET(serverSock, &rset)){
            struct sockaddr_in sin;
            memset(&sin, 0, sizeof(sin));
            unsigned int sin_len = sizeof(sin);
            int clientSock = accept(serverSock, (struct sockaddr *) &sin, &sin_len);
            if (clientSock < 0){
                cerr << "accept failed : " << strerror(errno) << endl; 
            } else {
                char clientAddr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(sin.sin_addr), clientAddr, INET_ADDRSTRLEN);
                int clientPort = ntohs(sin.sin_port);
                cout << "Accepted connection from : " << clientAddr << " : " << clientPort << " (socket : " << clientSock << ")" << endl;
                 
                unique_lock<mutex> ul(clientSocksMutex);
                clientSocks[clientSock] = {clientSock, "jjh", time(NULL)};
                thread(CommunicateWithClient, clientSock).detach(); // 클라이언트 통신 스레드 시작
            }
        }

        unique_lock<mutex> ul(clientSocksMutex);
        for (auto it = clientSocks.begin() ; it != clientSocks.end() ; ++it){
            int clientSock = it -> first;
            auto &info = it -> second;
            if (!FD_ISSET(clientSock, &rset)){
                if (info.last + 30 <= time(NULL)){
                    cout << "Idle timeout. clientSock: " << clientSock << endl;
                    willClose.insert(clientSock);
                }
                continue;
            }
        }

        // 닫아야 하는 소켓 정리
        for (int clientSock : willClose){
            cout << "Close : " << clientSock << endl;
            close(clientSock);
            clientSocks.erase(clientSock);
        }
    }
}

void ShutdownServer() {
    quit.store(true);
    // 클라이언트 소켓 닫기
    for (auto& [clientSock, _] : clientSocks){
        close(clientSock);
    }

    // 쓰레드 종료 대기
    for (auto& thread : worker_threads){
        if(thread.joinable()){
            thread.join();
        }
    }
}

int main(){
    signal(SIGINT, [](int) {
        cout << "Received SIGINT, shutting down server...\n";
        ShutdownServer();
        exit(0);
    });

    int passiveSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // port 재사용 옵션 설정
    int on = 1;
    if (setsockopt(passiveSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
        cerr << "setsockopt() failed : " << strerror(errno) << endl;
    }

    // 특정 TCP 포트에 바인딩
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT);
    if(bind(passiveSock, (struct sockaddr *) &sin, sizeof(sin)) < 0){
        cerr << "bind() failed : " << strerror(errno) << endl;
        return 1;
    }

    // passive socket 으로 client 리스닝
    if(listen(passiveSock, 10) < 0){
        cerr << "listen() failed : " << strerror(errno) << endl;
        return 1;
    }

    cout << "Port 번호 " << PORT << "에서 서버 동작 중\n" << endl;

    for(int i=0 ; i < WORKER_THREAD_NUMBER ; ++i){
        worker_threads.emplace_back(MessageWorkerThread,i);
    }

    while(true){
        HandleClientConnection(passiveSock);
    }
    close(passiveSock);

    return 0;
}