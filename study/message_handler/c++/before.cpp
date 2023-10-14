#include <iostream>
#include <sstream>
#include <string>

using namespace std;

enum {
    VERSION_CHECK = 0,
    LOGIN_REQUEST = 1,
    PROFILE_REQUEST = 2,
};

class AppMessage {};

void OnVersionCheck(const AppMessage *msg){
    cout << "OnVersionCheck" << endl;
}

void OnLoginRequest(const AppMessage *msg){
    cout << "OnLoginRequest" << endl;
}

void OnProfileRequest(const AppMessage *msg){
    cout << "OnProfileRequest" << endl;
}

int main(){
    string line;
    while (cin >> line) {
        stringstream ss(line);
        int type;
        ss >> type;

        AppMessage *msg = new AppMessage;
        switch(type) {
            case VERSION_CHECK:
                OnVersionCheck(msg);
                break;
            case LOGIN_REQUEST:
                OnLoginRequest(msg);
                break;
            case PROFILE_REQUEST:
                OnProfileRequest(msg);
        }
    }
}