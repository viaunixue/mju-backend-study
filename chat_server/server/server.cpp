#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <map>
#include <set>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>

using namespace std;

static const uint16_t PORT = 19130;
constexpr int WORKER_THREAD_NUMBER = 2;

struct ClientInfo {
    int sock_num;
    string nickname;
    time_t last;
};

// 현재 연결된 client socket (active socket)
map<int, ClientInfo> clientSocks;
set<int> willClose;

// 명령어 처리 함수
void ProcessCommand(int client_sock, const string& command){
    // 명령어 따라 적절한 동작 선택
    // 예) /name, /rooms, /create, /join, /leave, /shutdown 등
}

void MessageWorkerThread(int worker_id){
    cout << "메세지 작업 쓰레드 #" << worker_id << "생성\n";
}

// 클라이언트와 통신 
void CommunicateWithClient(int clientSock){
    while(true) {
        char buf[65536];
        int numRecv = recv(clientSock, buf, sizeof(buf), 0);

        if (numRecv == 0){
            cout << "Socket closed : " << clientSock << endl;
            willClose.insert(clientSock);
            break;
        } else if (numRecv < 0) {
            cerr << "recv() failed : " << strerror(errno) << ", client_sock : " << clientSock << endl;
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
                    for (auto& [otherSock, _] : clientSocks){
                        if (otherSock != clientSock){
                            send(otherSock, receivedData.c_str(), receivedData.size(), 0);
                        }
                    }
                }
            }

        }
    }
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
                clientSocks[clientSock] = {clientSock, "jjh", time(NULL)};
                thread(CommunicateWithClient, clientSock).detach(); // 클라이언트 통신 스레드 시작
            }
        }

        for (auto it = clientSocks.begin() ; it != clientSocks.end() ; ++it){
            int clientSock = it -> first;
            auto &info = it -> second;
            if (!FD_ISSET(clientSock, &rset)){
                if (info.last + 3 <= time(NULL)){
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

int main(){
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

    vector<thread> worker_threads;

    for(int i=0 ; i < WORKER_THREAD_NUMBER ; ++i){
        worker_threads.emplace_back(MessageWorkerThread, i);
    }

    cout << "Port 번호 " << PORT << "에서 서버 동작 중\n" << endl;

    while(true){
        HandleClientConnection(passiveSock);
    }
    close(passiveSock);

    // 메세지 작업 쓰레드 종료 대기
    for (auto& thread : worker_threads){
        thread.join();
    }

    return 0;
}