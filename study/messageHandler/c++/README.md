# C++ Message Handler

## `Before` 
메시지 타입이 string 이 아니라 int 인 경우를 흉내낸 것입니다.<br> 
socket 을 붙이면 코드가 더 이해하기 어려우니 cin 으로 받은 것을 메시지 타입으로 가정하고 처리했습니다.
```c++
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


void OnVersionCheck(const AppMessage *msg) {
  cout << "OnVersionCheck" << endl;
}


void OnLoginRequest(const AppMessage *msg) {
  cout << "OnLoginRequest" << endl;
}


void OnProfileRequest(const AppMessage *msg) {
  cout << "OnProfileRequest" << endl;
}


int main() {
  string line;
  while (cin >> line) {
    stringstream ss(line);
    int type;
    ss >> type;

    AppMessage *msg = new AppMessage;
    switch (type) {
      case VERSION_CHECK:
        OnVersionCheck(msg);
        break;
      case LOGIN_REQUEST:
        OnLoginRequest(msg);
        break;
      case PROFILE_REQUEST:
        OnProfileRequest(msg);
        break;
    }
  }
}
```

## `After` 
int 형의 메시지 타입이 주어진 경우 handler 를 찾아서 함수 호출하는 것을 보여줍니다. <br> C++ 의 타입 관련된 코드가 복잡하므로 이해를 돕기 위해서 handler 를 교체하는 것이나, <br> handler 를 연속으로 붙이는 것은 생략했으나 python 의 경우와 유사하게 구현 가능합니다.
```c++
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace std;


enum {
  VERSION_CHECK = 0,
  LOGIN_REQUEST = 1,
  PROFILE_REQUEST = 2,
};


class AppMessage {};

typedef int MessageType;
typedef void (*MessageHandler)(const AppMessage *);
typedef map<MessageType, MessageHandler> HandlerMap;


void OnVersionCheck(const AppMessage *msg) {
  cout << "OnVersionCheck" << endl;
}


void OnLoginRequest(const AppMessage *msg) {
  cout << "OnLoginRequest" << endl;
}


void OnProfileRequest(const AppMessage *msg) {
  cout << "OnProfileRequest" << endl;
}


static HandlerMap handlers {
  {VERSION_CHECK, OnVersionCheck},
  {LOGIN_REQUEST, OnLoginRequest},
  {PROFILE_REQUEST, OnProfileRequest},
};


int main() {
  string line;
  while (cin >> line) {
    stringstream ss(line);
    int type;
    ss >> type;

    AppMessage *msg = new AppMessage;
    if (handlers.find(type) != handlers.end()) {
      handlers[type](msg);
    } else {
      cerr << "Invalid type: " << type << endl;
    }
  }
}
```
* `static` 키워드는 해당 맵이 정적 변수로 선언되었음을 나타냅니다. <br>
   프로그램이 시작될 때 한 번 초기화되며, 메모리에서 해제되지 않는다는 의미입니다.
* `stringstream`은 문자열을 스트림으로 처리할 수 있는 C++ 클래스로, <br> 
   문자열에서 데이터를 추출하는데 사용됩니다. 
* `ss >> type` 은 `ss`  스트림에서 데이터를 추출하여 `type` 변수에 저장합니다.
* `handlers.find(type)`은 `handlers` 맵에서 `type`을 키로 검색하여 해당 핸들러 함수의 반복자(iterator)를 반환합니다.
* `handlers[type](msg)` 는 맵에서 `type`에 해당하는 핸들러 함수를 호출합니다.
* `msg` 매개변수는 모든 핸들러가 받는 `const AppMessage*` 매개변수를 전달합니다.