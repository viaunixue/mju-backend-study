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

struct ChatRoom {
    int roomNumber;
    string roomTitle;
    vector<string> participantNames;
    vector<int> participantSockets;
};

vector<ChatRoom> chatRooms;

enum MessageType {
    CSName,
    CSRooms,
    CSCreateRoom,
    CSJoinRoom,
    // 다른 메세지 유형 이후 추가 
};

// 문자열을 MessageType으로 매핑하는 맵
static map<string, MessageType> stringToMessageType = {
    {"CSName", CSName},
    {"CSRooms", CSRooms},
    {"CSCreateRoom", CSCreateRoom},
    {"CSJoinRoom", CSJoinRoom},
    // 다른 메세지 유형에 대한 매핑 추가
};

class AppMessage {
    public:
        int roomId;
        string name;
        string roomTitle;
};

typedef void (*MessageHandler)(int clientSock, const AppMessage& data);
typedef map<MessageType, MessageHandler> HandlerMap;

static const uint16_t PORT = 19130;
constexpr int WORKER_THREAD_NUMBER = 5;

// 쓰레드 종료 조건 위한 플래그
atomic<bool> quit(false);

// 공유 자원인 큐와 관련된 Mutex 및 조건 변수
mutex msgQueuesMutex, willCloseMutex;
condition_variable msgQueFilled;

// 동적으로 생성된 워커 쓰레드를 저장하기 위한 벡터
vector<thread> workerThreads;
map<int, queue<string>> msgQueues;

// 클라이언트 소켓과 이름 매핑
map<int, string> clientSocketToName;
map<int, thread::id> threadIdMap;
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

void HandleNameChange(int clientSock, const AppMessage& message){
    cout << "[HandleNameChange] clientSock : " << clientSock << message.name << endl;

    // 새로운 이름으로 변경 전의 클라이언트 이름 저장
    string oldClientName = clientSocketToName[clientSock];

    if(clientSocks.find(clientSock) != clientSocks.end()){
        // 새 클라이언트 이름을 맵에 저장
        clientSocketToName[clientSock] = message.name;

        json response;
        response["type"] = "SCSystemMessage";
        response["text"] = "이름이 " + message.name + "로 변경되었습니다.";

        cout << "[HandleNameChange] response message: " << response.dump() << endl;
        
        SendMessage(clientSock, response.dump());
    }

    // 방에 이미 들어가 있는지 확인
    auto currentRoomIt = find_if(chatRooms.begin(), chatRooms.end(), [clientSock](const ChatRoom& room){
        return find(room.participantSockets.begin(), room.participantSockets.end(), clientSock) != room.participantSockets.end();
    });

    // 클라이언트 참여 중인 대화방 인덱스 찾기
    int roomIndex = distance(chatRooms.begin(), currentRoomIt);

    if (currentRoomIt != chatRooms.end()){
        // 채팅 방 모든 멤버에게 이름 변경 시스템 메시지 전송
        for (int participantSock : chatRooms[roomIndex].participantSockets){
            json nameChangeNotification;
            nameChangeNotification["type"] = "SCSystemMessage";
            nameChangeNotification["text"] = oldClientName + " 님의 이름이 " + message.name + " 로 변경되었습니다.";
            SendMessage(participantSock, nameChangeNotification.dump());
        }
        cout << "[HandleNameChange] " << oldClientName << " changed their name to " << message.name << " in room #" << chatRooms[roomIndex].roomNumber << endl;
    }
}

void HandleRoomStatus(int clientSock, const AppMessage& message){
    cout << "[HandleRoomStatus] clientSock " << clientSock << endl;
    if(clientSocks.find(clientSock) != clientSocks.end()){
        json response;
        response["type"] = "SCRoomsResult";

        // ChatRoom 배열에 저장된 방 정보를 나열
        for (const ChatRoom& room : chatRooms) {
            json roomInfo;
            roomInfo["roomId"] = room.roomNumber;
            roomInfo["title"] = room.roomTitle;

            // 참여 중인 멤버들의 이름을 배열로 저장
            vector<string> participants;
            for (int participantSock : room.participantSockets){
                participants.push_back(clientSocketToName[participantSock]);
            }

            roomInfo["members"] = participants;
            response["rooms"].push_back(roomInfo);
        }

        cout << "[HandleRoomStatus] response message: " << response.dump() << endl;
        
        SendMessage(clientSock, response.dump());
    }
}

void HandleRoomCreate(int clientSock, const AppMessage& message){
    cout << "[HandleRoomCreate] clientSock " << clientSock << " " << endl;
    cout << "[HandleRoomCreate] message.roomTitle " << message.roomTitle << " " << endl;

    string clientName = clientSocketToName[clientSock];
    cout << "[HandleRoomCreate] clientName " << clientName << " " << endl;

    // 방 번호 생성 (간단한 예시로 현재 방 개수 + 1로 설정)
    int roomNumber = chatRooms.size() + 1;

    // 생성된 방 정보 저장
    ChatRoom newRoom;
    newRoom.roomNumber = roomNumber;
    newRoom.roomTitle = message.roomTitle;
    newRoom.participantSockets.push_back(clientSock); // 생성자도 방에 참여
    newRoom.participantNames.push_back(clientName); // 방을 만든 사람의 닉네임

    // 채팅방 목록에 추가
    chatRooms.push_back(newRoom);

    // 클라이언트에게 채팅방 생성 성공 메세지 전송
    json response;
    response["type"] = "SCSystemMessage";
    response["text"] = clientName + "님이 방에 입장했습니다.";
    SendMessage(clientSock, response.dump());

    // 생성된 방 정보 출력
    cout << "[HandleRoomCreate] Created Room - Number: " << roomNumber << ", Title: " << message.roomTitle << endl;

//     // 새로 생성된 방에 참여한 사용자에게 시스템 메시지 전송
//     // json systemMessage;
//     // systemMessage["type"] = "SCSystemMessage";
//     // systemMessage["text"] = "방에 참여하셨습니다.";

//     // SendMessageToRoom(roomNumber, systemMessage.dump());
}

void HandleRoomJoin(int clientSock, const AppMessage& message){
    cout << "[HandleRoomJoin] clientSock " << clientSock << " " << endl;
    // 클라이언트가 참가하려는 방의 번호
    int targetRoomId = message.roomId;
    string clientName = clientSocketToName[clientSock];
    cout << "[HandleRoomJoin] clientName " << clientName << " " << endl;

    // 해당 방이 존재하는지 확인
    auto roomIt = find_if(chatRooms.begin(), chatRooms.end(), [targetRoomId](const ChatRoom& room){
        return room.roomNumber == targetRoomId;
    });

    // 방에 이미 들어가 있는지 확인
    auto currentRoomIt = find_if(chatRooms.begin(), chatRooms.end(), [clientSock](const ChatRoom& room){
        return find(room.participantSockets.begin(), room.participantSockets.end(), clientSock) != room.participantSockets.end();
    });

    // 유저가 이미 방에 들어가 있는 경우
    if(currentRoomIt != chatRooms.end()){
        cerr << "[HandleRoomJoin] " << clientName << " is already in another room." << endl;

        // 클라이언트에게 시스템 메시지 전송
        json errorResponse;
        errorResponse["type"] = "SCSystemMessage";
        errorResponse["text"] = clientName + "님은 이미 다른 방에 참가하셨습니다.";
        SendMessage(clientSock, errorResponse.dump());
        return;
    }

    // 방이 존재하는 경우
    if (roomIt != chatRooms.end()){
        // 이미 참가 중인지 확인
        auto participantIt = find(roomIt->participantSockets.begin(), roomIt->participantSockets.end(), clientSock);
        
        // 아직 참가하지 않은 경우
        if(participantIt == roomIt->participantSockets.end()){
            // 클라이언트를 방에 추가
            roomIt->participantSockets.push_back(clientSock);
            roomIt->participantNames.push_back(clientName);

            // 클라이언트에게 시스템 메시지 전송
            json response;
            response["type"] = "SCSystemMessage";
            response["text"] = clientName + "님이 방에 참가하셨습니다.";
            SendMessage(clientSock, response.dump());

            // 방에 참가한 멤버들에게 새로운 멤버 알림
            for(int participantSock : roomIt->participantSockets){
                if(participantSock != clientSock){
                    json notification;
                    notification["type"] = "SCSystemMessage";
                    notification["text"] = clientName + "님이 방에 참가하셨습니다.";
                    SendMessage(participantSock, notification.dump());
                }
            }

            cout << "[HandleRoomJoin] " << clientName << " joined room #" << targetRoomId << endl;
        } else {
            // 이미 참가한 경우
            cerr << "[HandleRoomJoin] Client is already in the room #" << targetRoomId << ": " << clientName << endl;
            // 클라이언트에게 에러 메시지 전송
            json errorResponse;
            errorResponse["type"] = "SCSystemMessage";
            errorResponse["text"] = clientName + "님은 이미 해당 방에 참가 중입니다.";
            SendMessage(clientSock, errorResponse.dump());
        }
    } else {
        // 방이 존재하지 않는 경우
        cerr << "[HandleRoomJoin] Room not found: " << targetRoomId << endl;
        // 클라이언트에게 에러 메시지 전송
        json errorResponse;
        errorResponse["type"] = "SCSystemMessage";
        errorResponse["text"] = "존재하지 않는 방입니다.";
        SendMessage(clientSock, errorResponse.dump());
    }
}

static HandlerMap handlers {
    {CSName, HandleNameChange},
    {CSRooms, HandleRoomStatus},
    {CSCreateRoom, HandleRoomCreate},
    {CSJoinRoom, HandleRoomJoin},
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
                    AppMessage appMessage;

                    // AppMessage 객체를 생성하면서 roomId 값 설정
                    if (parsedJson.contains("roomId")) {
                        appMessage.roomId = parsedJson["roomId"];
                    }

                    // AppMessage 객체를 생성하면서 name 값 설정
                    if (parsedJson.contains("name")) {
                        appMessage.name = parsedJson["name"];
                    }

                    // AppMessage 객체를 생성하면서 roomTitle 값 설정
                    if (parsedJson.contains("title")) {
                        appMessage.roomTitle = parsedJson["title"];
                    }
                    
                    handleIt->second(clientSock, appMessage);
                } else {
                    cerr << "[HandlerCommand] Handler not found for command type : " << commandType << endl;
                }
            } else {
                cerr << "[HandlerCommand] Invalid message type : " << commandTypeStr << endl;

                // 디버깅용: stringToMessageType 맵 전체를 출력
                cerr << "[HandlerCommand] stringToMessageType map contents:" << endl;
                for (const auto& entry : stringToMessageType) {
                    cerr << entry.first << " => " << entry.second << endl;
                }
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

void ConsumeMessage(int clientSock);

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

        ConsumeMessage(clientSock);
    } catch (const exception& e){
        cerr << "Invalid JSON format: " << e.what() << endl;
        // 예외가 발생한 경우 클라이언트 소켓을 안전하게 닫음
        unique_lock<mutex> ul(msgQueuesMutex);
        cout << "Closing socket due to exception: " << clientSock << endl;
        close(clientSock);
        clientSocks.erase(clientSock);
        msgQueues.erase(clientSock);
    }
}

// Consumer
void ConsumeMessage(int clientSock){
    cout << "[ConsumeMessage] socket: " << clientSock << endl;
    std::thread::id cosumeThreadId = this_thread::get_id(); // 현재 쓰레드
    {
        unique_lock<mutex> ul(msgQueuesMutex);
        threadIdMap[clientSock] = cosumeThreadId; // 맵에 쓰레드 ID 추가
    }

    while(!quit.load()){
        cout << "[ConsumeMessage] socket enter the loop : " << clientSock << endl;
        string message;
        {
            unique_lock<mutex> ul(msgQueuesMutex);
            while(!quit.load() && msgQueues[clientSock].empty()) {
                cout << "[ConsumeMessage] socket enter the waiting loop : " << clientSock << endl;
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
    cout <<  "[ConsumeMessage] Consume finished : " << clientSock << endl;

    {
        unique_lock<mutex> ul(msgQueuesMutex);
        threadIdMap.erase(clientSock); // 맵에서 쓰레드 ID 제거
    }
}

void CloseClientSockets(){
    unique_lock<mutex> ul(msgQueuesMutex);
    for (int clientSockNum : willClose){
        cout << "Close : " << clientSockNum << endl;
        close(clientSockNum);
        clientSocks.erase(clientSockNum);

        unique_lock<mutex> ulMsgQueues(msgQueuesMutex);
        msgQueues.erase(clientSockNum);
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
        {
            unique_lock<mutex> ul(msgQueuesMutex);
            for (int clientSock : clientSocks) {
                // 소켓 유효성 확인
                if(clientSock <= 0){
                    continue;
                }
                FD_SET(clientSock, &rset);
                if (clientSock > maxFd){
                    maxFd = clientSock;   
                }
            }
        }

        cout << "[HandleClientConnection] rset status before select(): ";
        for (int clientSock : clientSocks) {
            cout << (FD_ISSET(clientSock, &rset) ? "1" : "0") << ":";
        }
        cout << endl;

        // select()
        int numReady = select(maxFd + 1, &rset, NULL, NULL, NULL);

        // select() 이후 로그
        cout << "[HandleClientConnection] rset status after select(): ";
        for (int clientSock : clientSocks) {
            cout << (FD_ISSET(clientSock, &rset) ? "1" : "0") << ":";
        }
        cout << endl;
        
        if(numReady < 0){
            cerr << "[HandleClientConnection] select() failed : " << strerror(errno) << endl;
            if(errno == EBADF){
                {
                    // client 소켓이 종료된 경우
                    cerr << "[HandleClientConnection] EBADF : " << errno << endl;
                    unique_lock<mutex> ul(msgQueuesMutex);
                    for (auto it = clientSocks.begin(); it != clientSocks.end();){
                        int clientSock = *it;
                        cout << "[HandleClientConnection] clientSock : " << clientSock << endl;
                        if (!FD_ISSET(clientSock, &rset)) {
                            
                            cerr << "[HandleClientConnection] Closing socket : " << clientSock << endl;

                            // 종료 메시지를 큐에 추가
                            msgQueues[clientSock].push("EXIT_MESSAGE");
                            msgQueFilled.notify_one();

                            // 클라이언트 종료
                            try {
                                close(clientSock);
                            } catch (const exception& e){
                                cerr << "[HandleClientConnection] Error closing socket : " << e.what() << endl;
                            }

                            // 쓰레드 종료
                            auto threadIt = find_if(workerThreads.begin(), workerThreads.end(), [clientSock](const thread& t){
                                unique_lock<mutex> ul(msgQueuesMutex);
                                return t.joinable() && t.get_id() == threadIdMap[clientSock];
                            });

                            if (threadIt != workerThreads.end()) {
                                threadIt->join();
                                workerThreads.erase(threadIt);
                            }

                            // 해당 소켓의 큐도 정리
                            it = clientSocks.erase(it);
                            msgQueues.erase(clientSock);
                            threadIdMap.erase(clientSock);

                            cout << "[HandleClientConnection] Socket closed and cleaned up: " << clientSock << endl;
                        } else {
                            cout << "[HandleClientConnection] FD_ISSET : " << clientSock << endl;
                            ++it;
                        }
                    }
                }
            }
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
                cerr << "[HandleClientConnection] accept failed : " << strerror(errno) << endl; 
            } else {
                char clientAddr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(sin.sin_addr), clientAddr, INET_ADDRSTRLEN);
                int clientPort = ntohs(sin.sin_port);
                cout << "Accepted connection from : " << clientAddr << " : " << clientPort << " (socket : " << clientSock << ")" << endl;
                {
                    unique_lock<mutex> ul(msgQueuesMutex);
                    clientSocks.insert(clientSock);
                }

                cout << "Added client socket : " << clientSock << endl;

                thread consumeThread(ConsumeMessage, clientSock);
                workerThreads.push_back(move(consumeThread));
            }
        }

        {
            unique_lock<mutex> ul(msgQueuesMutex);
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
                        cerr << "[HandleClientConnection] 중괄호를 찾을 수 없습니다." << endl;
                        return;
                    }
                    cout << "[HandleClientConnection] After receivedData: " << receivedData << endl;
                    thread produceThread(ProduceMessage, clientSock, receivedData);
                    workerThreads.push_back(move(produceThread));

                    // ProduceMessage(clientSock, receivedData);
                }
            }
        }

        {
            unique_lock<mutex> ul(msgQueuesMutex);
            for (int clientSockNum : willClose) {
                cout << "Close : " << clientSockNum << endl;
                close(clientSockNum);
            }
            willClose.clear();
        }
        
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
    cout << "\nServer shutdown complete.\n";
}

void HandleSIGINT(int){
    cout << "\nReceived SIGINT, shutting down server...\n";
    ShutdownServer();
    exit(0);
}

int main(){
    signal(SIGINT, HandleSIGINT);

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