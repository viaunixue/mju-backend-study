#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace std;

enum {
    VERSION_CHEKCK = 0,
    LOGIN_REQUEST = 1,
    PROFILE_REQUEST = 2,
};

class AppMessage {};

typedef int MessageType;
typedef void (*MessageHandler)(const AppMessage *);
typedef map<MessageType, MessageHandler> HandlerMap;

void OnVersionCheck(const AppMessage *msg){
    cout << "OnVersionCheck" << endl;
}

void OnLoginRequest(const AppMessage *msg){
    cout << "OnLoginRequest" << endl;
}

void OnProfileRequest(const AppMessage *msg){
    cout << "OnProfileRequest" << endl;
}

static HandlerMap handlers {
    {VERSION_CHEKCK, OnVersionCheck},
    {LOGIN_REQUEST, OnLoginRequest},
    {PROFILE_REQUEST, OnProfileRequest},
};

int main(){
    string line;
    while (cin >> line) {
        stringstream ss(line);
        int type;
        ss >> type;

        AppMessage *msg = new AppMessage;
        if(handlers.find(type) != handlers.end()){
            handlers[type](msg);
        } else {
            cerr << "Invalid type: " << type << endl;
        }
    }
}

