#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <iomanip>
#include <queue>
#include <map>
#include <set>
#include <csignal>
#include <regex>
#include <string>
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "json.hpp"
#include "message.pb.h"

using namespace nlohmann;
using namespace std;

enum MessageType {
    CSName,
    // 다른 메세지 유형 이후 추가 
};

// 문자열을 MessageType으로 매핑하는 맵
static map<string, MessageType> stringToMessageType = {
    {"CSName", CSName},
    // 다른 메세지 유형에 대한 매핑 추가
};

class AppMessage {};

typedef void (*MessageHandler)(int clientSock, const AppMessage& data);
typedef map<MessageType, MessageHandler> HandlerMap;

static const uint16_t PORT = 19130;
constexpr int WORKER_THREAD_NUMBER = 5;

// 쓰레드 종료 조건 위한 플래그
atomic<bool> quit(false);

// 공유 자원인 큐와 관련된 Mutex 및 조건 변수
mutex coutMutex, msgQueuesMutex, willCloseMutex;
condition_variable msgQueFilled;
queue<string> msgQueue;

// 동적으로 생성된 워커 쓰레드를 저장하기 위한 벡터
vector<thread> workerThreads;
map<int, queue<string>> msgQueues;

// 현재 연결된 client socket (active socket)
set<int> clientSocks;
set<int> willClose;

void SendMessage(int clientSock, const string& message) {
    cout << "[SendMessage] clientSock and message : " << clientSock << " and " << message << endl;
    
    // 메시지 길이 2바이트로 표현
    uint16_t messageLength = static_cast<uint16_t>(message.size());

    // 길이 정보를 메시지에 합치기
    string combinedMessage;
    combinedMessage.push_back(static_cast<char>((messageLength >> 8) & 0xFF));
    combinedMessage.push_back(static_cast<char>(messageLength & 0xFF));
    combinedMessage += message;

    if (send(clientSock, combinedMessage.c_str(), combinedMessage.size(), 0) < 0) {
        cerr << "[SendMessage] send() failed : " << strerror(errno) << ", client_sock : " << clientSock << endl;
        // 실패 처리
        {
            unique_lock<mutex> ul(willCloseMutex);
            willClose.insert(clientSock);
        }
        
    }
    cout << "[SendMessage] combinedMessage : " << message << endl;
}

void HandleNameChange(int clientSock, const AppMessage& newNickname){
    cout << "[HandleNameChange] clientSock " << clientSock << endl;
    if(clientSocks.find(clientSock) != clientSocks.end()){
        json response;
        response["type"] = "SCSystemMessage";
        response["text"] = "이름이 변경되었습니다.";
        // response["text"] = "이름이 " + static_cast<string>(newNickname) + "으로 변경되었습니다.";

        cout << "[HandleNameChange] response message: " << response.dump() << endl;
        
        SendMessage(clientSock, response.dump());
    }
}

static HandlerMap handlers {
    {CSName, HandleNameChange}
    // 다른 메시지 유형 핸들러 추가 예정
};

void HandlerCommand(int clientSock, const string& msg){
    cout << "[HandlerCommand] client socket, message : " << clientSock << ", " << msg << endl;
    try {
        // JSON 형식 명령어 파싱
        auto parsedJson = json::parse(msg);

        if(parsedJson.contains("type")){
            string commandTypeStr = parsedJson["type"];
            // 문자열을 MessageType으로 변환
            auto typeIt = stringToMessageType.find(commandTypeStr);
            if (typeIt != stringToMessageType.end()) {
                MessageType commandType = typeIt->second;

                auto handleIt = handlers.find(commandType);
                if (handleIt != handlers.end()) {
                    // 해당 유형에 맞는 핸들러 호출
                    AppMessage AppMessage;
                    handleIt->second(clientSock, AppMessage);
                } else {
                    cerr << "[HandlerCommand] Handler not found for command type : " << commandType << endl;
                }
            } else {
                cerr << "[HandlerCommand] Invalid message type : " << commandTypeStr << endl;
            }
        } else {
            cerr << "[HandlerCommand] Invalid type : " << msg << endl;
        }

    } catch (const exception& e){
        cerr << "[HandlerCommand] JSON parsing failed for command : " << e.what() << endl;
        {
            unique_lock<mutex> ul(willCloseMutex);
            willClose.insert(clientSock);
        }
    }
}

// Producer
void ProduceMessage(int clientSock, const string& msg){
    cout << "[ProduceMessage] clientSock :  " << clientSock << ", message : " << msg << endl;
    try {
        {
            unique_lock<mutex> ul(msgQueuesMutex);
            msgQueues[clientSock].push(msg); // 클라이언트별 메시지 큐에 메시지 추가
            msgQueFilled.notify_one();
        }
        cout << "[ProduceMessage] messageQueue Size : " << msgQueues[clientSock].size() << endl;
    } catch (const exception& e){
        cerr << "Invalid JSON format: " << e.what() << endl;
    }
}

// Consumer
void ConsumeMessage(int clientSock){
    cout << "[ConsumeMessage] socket: " << clientSock << endl;
    while(!quit.load()){
        cout << "[ConsumeMessage] socket ( while loop ) : " << clientSock << endl;
        string message;
        {
            unique_lock<mutex> ul(msgQueuesMutex);
            while(!quit.load() && msgQueues[clientSock].empty()) {
                msgQueFilled.wait(ul);
            }
            cout <<  "[ConsumeMessage] After msgQueFilled  : " << message << endl;

            if (!msgQueues[clientSock].empty()){
                cout <<  "[ConsumeMessage] Before pop message : " << message << endl;
                message = msgQueues[clientSock].front();
                msgQueues[clientSock].pop();
            }
        }
        cout <<  "[ConsumeMessage] Consumed message: " << message << endl;
        if (!message.empty()){
            HandlerCommand(clientSock, message);
        }
    }
}

void CloseClientSockets(){
    for (int clientSockNum : willClose){
        cout << "Close : " << clientSockNum << endl;
        close(clientSockNum);
        // clientSocks.erase(clientSockNum);
    }
    willClose.clear(); // 클리어 후 다음 사용 위해 초기화
}

// 클라이언트와 연결
void HandleClientConnection(int serverSock){
    while (true){
        fd_set rset;                    // select() 호출 위한 fd_set 변수 생성
        FD_ZERO(&rset);                 // 자료구조 초기화
        
        FD_SET(serverSock, &rset);    // 자료구조에 소켓 등록
        int maxFd = serverSock;            

        // 클라이언트 소켓 추가하고 최대 소켓 번호 갱신
        for (int clientSock : clientSocks) {
            FD_SET(clientSock, &rset);
            if (clientSock > maxFd){
                maxFd = clientSock;   
            }
        }

        // select()
        int numReady = select(maxFd + 1, &rset, NULL, NULL, NULL);
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
                
                clientSocks.insert(clientSock);

                cout << "Added client socket : " << clientSock << endl;

                thread consumeThread(ConsumeMessage, clientSock);
                workerThreads.push_back(move(consumeThread));
            }
        }

        for (int clientSock : clientSocks) {
            if (!FD_ISSET(clientSock, &rset)){
                continue;
            }

            // 이 연결로부터 데이터를 읽음
            char buf[65536];
            int numRecv = recv(clientSock, buf, sizeof(buf), 0);
            if (numRecv == 0){
                cout << "Socket closed: " << clientSock << endl;
                willClose.insert(clientSock);
            } else if (numRecv < 0){
                cerr << "recv() failed: " << strerror(errno) << ", clientSock: " << clientSock << endl;
                willClose.insert(clientSock);
            } else {
                cout << "[HandleClientConnection] Received: " << numRecv << " bytes, clientSock " << clientSock << endl;
                string receivedData(buf, numRecv);
                cout << "[HandleClientConnection] Before receivedData: " << receivedData << endl;

                // 중괄호 이전의 모든 값을 제거
                size_t bracePos = receivedData.find('{');
                if (bracePos != string::npos) {
                    receivedData = receivedData.substr(bracePos);
                } else {
                    cerr << "중괄호를 찾을 수 없습니다." << endl;
                    return;
                }
                cout << "[HandleClientConnection] After receivedData: " << receivedData << endl;
                ProduceMessage(clientSock, receivedData);
            }
        }

        // 닫아야 하는 소켓 정리
        for (int clientSock : willClose){
            cout << "Close : " << clientSock << endl;
            close(clientSock);
            clientSocks.erase(clientSock);
            {
                unique_lock<mutex> ul(msgQueuesMutex);
                msgQueues.erase(clientSock);
            }
        }
        willClose.clear();
    }
}

void ShutdownServer() {
    quit.store(true);

    // 쓰레드 종료 대기
    for (auto& thread : workerThreads){
        thread.join();
    }

    // 클라이언트 소켓 닫기
    CloseClientSockets();
    exit(0);
}

int main(){
    signal(SIGINT, [](int) {
        cout << "\nReceived SIGINT, shutting down server...\n";
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
        cout << "메세지 작업 쓰레드 #" << i << " 생성\n";
    }
    
    while(true){
        HandleClientConnection(passiveSock);
    }

    close(passiveSock);

    return 0;
}