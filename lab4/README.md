# lab 4 - Message Formatting - Protobuf, JSON

## `Test #1` 
### **proto 파일 생성** <br>
```proto
syntax = "proto2";

package mju;

message Person {
    required string name = 1;
    optional int32 id = 2;

    enum PhoneType {
        MOBILE = 0;
        HOME = 1;
    }

    message PhoneNumber {
        optional string number = 1;
        optional PhoneType type = 2 [default = HOME];
    }

    repeated PhoneNumber phones = 4;
}
```
* person.proto 파일을 생성합니다.
* **Person** 클래스를 통해 **name** 정보를 `required` 정보로, **id** 정보를 `optional` 정보로 생성합니다. 
* **PhoneNumber** 클래스를 Person 클래스의 `nest class`로 **Phone** 정보와 **Phone Type**을 `optional`로 생성합니다.
* **PhoneType**은 `[default = HOME]`으로 `field`에 기본 값을 설정합니다.
* **PhoneNumber**은 `repeated` 배열 형태 필드로 선언합니다.

## `Test #2` 
### **proto 파일 컴파일** <br>
```
~/mju_backend_dev$ protoc -I. --cpp_out=. person.proto 
~/mju_backend_dev$ protoc -I. --python_out=. person.proto
```
* `-I` : proto 파일이 다른 proto 파일을 포함할 때, 파일을 찾는 상대 경로로 쓰입니다.
* `--cpp_out / --python out` : c++, python 파일을 생성하는데, 그 디렉터리를 지정합니다.
* 위 명령어를 통해 **person.pb.h** c++ 헤더 파일과 **person.pb.cc** 그리고 **person_pb2.py** 를 생성합니다.

## `Test #3` 
### **C++에서 Serialize 해보기** <br>
```cpp
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <iostream>

#include "person.pb.h"

using namespace std;
using namespace mju;

int main(){
    Person *p = new Person;
    p -> set_name("JongHyun Jung");
    p -> set_id(12345678);

    Person::PhoneNumber* phone = p -> add_phones();
    phone -> set_number("010-111-1234");
    phone -> set_type(Person::MOBILE);

    phone = p -> add_phones();
    phone -> set_number("02-100-1000");
    phone -> set_type(Person::HOME);

    const string s = p -> SerializeAsString();
    cout << "Length : " << s.length() << endl;
    cout << s << endl;
}
// result
// Length : 55
// JongHyun Jung���"
// 010-111-1234"
// 02-100-1000
```
```
~/mju_backend_dev$ g++ -o test3 test3.cpp person.pb.cc -l protobuf
```
* `SerializeAsString` 함수를 통해 p 주소의 객체 값들을 Serialize하여 s에 저장합니다.
* 이름 13 byte + 아이디 4 byte + 모바일 11 byte + 집 10 byte < 55 byte
* 저장된 필드 크기보다 약간 더 큰 데이터를 저장하고 있음을 확인할 수 있습니다.

## `Test #4` 
### **C++에서 Deserialize 해보기** <br>
```cpp
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <iostream>

#include "person.pb.h"

using namespace std;
using namespace mju;

int main(){
    Person *p = new Person;
    p -> set_name("JongHyun Jung");
    p -> set_id(12345678);

    Person::PhoneNumber* phone = p -> add_phones();
    phone -> set_number("010-111-1234");
    phone -> set_type(Person::MOBILE);

    phone = p -> add_phones();
    phone -> set_number("02-100-1000");
    phone -> set_type(Person::HOME);

    const string s = p -> SerializeAsString();
    cout << "Length : " << s.length() << endl;
    cout << s << endl;

    Person *p2 = new Person;
    p2 -> ParseFromString(s);
    cout << "Name : " << p2 -> name() << endl;
    cout << "ID : " << p2 -> id() << endl;
    for (int i = 0 ; i < p2 -> phones_size() ; ++i){
        cout << "Type : " << p2 -> phones(i).type() << endl;
        cout << "Phone : " << p2 -> phones(i).number() << endl; 
    }
}
// result
// Length : 55
// JongHyun Jung���"
// 010-111-1234"

// 02-100-1000
// Name : JongHyun Jung
// ID : 12345678
// Type : 0
// Phone : 010-111-1234
// Type : 1
// Phone : 02-100-1000
```
```
~/mju_backend_dev$ g++ -o test4 test4.cpp person.pb.cc -l protobuf
```
* `ParseFromString`을 통해 Serialize 되어 s에 저장된 객체 값들을 Deserialize 합니다.
* Serialize 된 문자열 s를 네트워크로 전송하면 받는 쪽에서 deserialize 할 수 있습니다.

## `Test #5` 
### **다른 언어와의 호환성** <br>
```c++
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <iostream>

#include "person.pb.h"

using namespace std;
using namespace mju;

int main(){
    Person *p = new Person;
    p -> set_name("JongHyun Jung");
    p -> set_id(12345678);

    Person::PhoneNumber* phone = p -> add_phones();
    phone -> set_number("010-111-1234");
    phone -> set_type(Person::MOBILE);

    phone = p -> add_phones();
    phone -> set_number("02-100-1111");
    phone -> set_type(Person::HOME);

    const string s = p -> SerializeAsString();
    cout << "Length: " << s.length() << endl;
    cout << s << endl;
    cout << endl;

    Person *p2 = new Person;
    p2 -> ParseFromString(s);
    cout << "Name : " << p2 -> name() << endl;
    cout << "ID : " << p2 -> id() << endl;
    for (int i=0 ; i < p2 -> phones_size() ; ++i){
        cout << "Type : " << p2-> phones(i).type() << endl;
        cout << "Phone : " << p2 -> phones(i).number() << endl;
    }

    ofstream f("mybinary", ios_base::out | ios_base::binary);
    f << s;
}
```
```
~/mju_backend_dev$ g++ -o test5 test5.cpp person.pb.cc -l protobuf
~/mju_backend_dev$ ./test5
~/mju_backend_dev$ ls -al mybinary
-rw-r--r-- 1 u60162176 F2023 55 10월  2 22:20 mybinary
```
* `ofstream` 함수를 통해 쓰기용, binary 조건을 지정한 "mybinary" 파일을 만듭니다.
* `f << s` 로 s 객체를 생성된 Binary 파일로 입력합니다. 
<br>

```python
import person_pb2
import sys

def main(argv): 
    with open('mybinary', mode='rb') as f:
        s = f.read()
        p = person_pb2.Person()
        p.ParseFromString(s)
        print('Name', p.name)
        print('Id', p.id)
        print('Phone1', p.phones[0].type, p.phones[0].number)
        print(p)

if __name__ == '__main__':
    main(sys.argv)

# result
# Name JongHyun Jung
# Id 12345678
# Phone1 0 010-111-1234
# name: "JongHyun Jung"
# id: 12345678
# phones {
#   number: "010-111-1234"
#   type: MOBILE
# }
# phones {
#   number: "02-100-1111"
#   type: HOME
# }
```
* test5.cpp 를 통해 생성한 `mybinary` 파일을 통해 test5.py 에서 상호 통신할 수 있음을 확인할 수 있습니다.
 
## `Test #6` 
### **UDP로 Protobuf 전송** <br>
```c++
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>
#include <string>
#include <iostream>

#include "person.pb.h"

#define PORT_NUM    20130
#define IP_NUM      "127.0.0.1"
#define BUF2_BYTES  65535

using namespace std;
using namespace mju;

int main(){
    int sc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sc < 0){
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    Person *p = new Person;
    p -> set_name("JongHyun Jung");
    p -> set_id(12345678);

    Person::PhoneNumber* phone = p -> add_phones();
    phone -> set_number("010-111-1234");
    phone -> set_type(Person::MOBILE);

    phone = p -> add_phones();
    phone -> set_number("02-100-1000");
    phone -> set_type(Person::HOME);

    const string s = p -> SerializeAsString();

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM);
    sin.sin_addr.s_addr = inet_addr(IP_NUM);

    int numBytes = sendto(sc, s.c_str(), s.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    cout << "Sent : " << numBytes << endl;
    cout << endl;

    char s2[BUF2_BYTES];
    memset(&sin, 0, sizeof(sin));
    socklen_t sin_size = sizeof(sin);
    numBytes = recvfrom(sc, s2, sizeof(s2), 0, (struct sockaddr *) &sin, &sin_size);

    cout << "Received: " << numBytes << endl;
    cout << "From " << inet_ntoa(sin.sin_addr) << endl;
    cout << endl;

    Person *p2 = new Person;
    if(!p2 -> ParseFromString(string(s2, numBytes))){
        cerr <<  "Failed to parse protobuf message" << endl;
    }
    cout << "Name : " << p2 -> name() << endl;
    cout << "ID : " << p2 -> id() << endl;
    for(int i=0 ; i < p2->phones_size() ; ++i){
        cout << "Type : " << p2 -> phones(i).type() << endl;
        cout << "Phone : " << p2 -> phones(i).number() << endl;
    }

    close(sc);
}
// result
// Sent : 55

// Received: 55
// From 127.0.0.1

// Name : JongHyun Jung
// ID : 12345678
// Type : 0
// Phone : 010-111-1234
// Type : 1
// Phone : 02-100-1000
```
* Serialize 된 s를 `UDP echo Server` 로 전송 후 돌려받은 데이터를 이용하여 Deserialize 합니다.
* s2 로 돌려받아 해당 값을 `ParseFromString(string(s2, numBytes))` 로 돌려받습니다.

## `Test #7` 
### **JSON Serialize** <br>
```python
import json
import sys

def main(argv):
    obj1 = {
        'name' : 'JongHyun Jung',
        'id' : 12345678,
    }

    s = json.dumps(obj1, indent=2)
    print(s)
    print('Type', type(s).__name__)

if __name__ == '__main__':
    main(sys.argv)

# result
# {
#   "name": "JongHyun Jung",
#   "id": 12345678
# }
# Type str
```
* `json.dumps()` 함수를 통해 JSON 문자열로 Serialize 합니다. 
* JSON Object Type은 `string` 임을 알 수 있습니다. *이는 Protobuf가 `binary` 인 것과 다릅니다.*
* **key 값은 언제나 문자열 형태로 사용합니다.**

## `Test #8` 
### **JSON Serialize - JSON Array** <br>
```python
import json
import sys

def main(argv):
    obj1 = {
        'name' : 'JongHyun Jung',
        'id' : 12345678,
    }

    obj2 = {
        'phone' : '010-0000-0000',
        'age' : 20,
    }

    obj = [obj1, obj2]

    s = json.dumps(obj, indent = 2)
    print(s)

if __name__ == '__main__':
    main(sys.argv)

# result
# [
#   {
#     "name": "JongHyun Jung",
#     "id": 12345678
#   },
#   {
#     "phone": "010-0000-0000",
#     "age": 20
#   }
# ]
```
* JSON 에서 배열 안에 들어가는 정보는 동일한 형태일 필요가 없습니다.
* obj1과 obj2 를 obj array로 배열화 하여 Serialize 할 수 있습니다.

## `Test #9` 
### **JSON Serialize - JSON Array** <br>
```python
import json
import sys

def main(argv):
    obj1 = {
        'name' : 'JongHyun Jung',
        'id' : 12345678,     
        'work' : {
            'name' : 'Myongji University',
            'address' : '116 Myongji-ro'
        }
    }

    s = json.dumps(obj1, indent = 2)
    print(s)

if __name__ == '__main__':
    main(sys.argv)

# result
# {
#   "name": "JongHyun Jung",
#   "id": 12345678,
#   "work": {
#     "name": "Myongji University",
#     "address": "116 Myongji-ro"
#   }
# }
```
* `work` 라는 **key 값** 안에 value로 다른 JSON object를 넣었습니다.

## `Test #10` 
### **JSON Serialize - 중첩된 Array** <br>
```python
import json
import sys

def main(argv):
    obj1 = {
        'name': 'JongHyun Jung',
        'id': 12345678,
        'work': {
            'name': 'Myongji University',
            'address': '116 Myongji-ro'
        },
        'scores': [100, 90.5, 80.0]
    }

    s = json.dumps(obj1, indent = 2)
    print(s)

if __name__ == '__main__' :
    main(sys.argv)

# result
# {
#   "name": "JongHyun Jung",
#   "id": 12345678,
#   "work": {
#     "name": "Myongji University",
#     "address": "116 Myongji-ro"
#   },
#   "scores": [
#     100,
#     90.5,
#     80.0
#   ]
# }

```
* JSON Object 안에 JSON array 가 포함된 것을 확인할 수 있습니다.

## `Test #11` 
### **JSON Deserialize** <br>
```python
import json
import sys

def main(argv):
    obj1 = {
        'name' : 'JongHyun Jung',
        'id' : 12345678,
        'work' : {
            'name' : 'Myongji University',
            'address' : '116 Myongji-ro'
        }
    }

    s = json.dumps(obj1)
    obj2 = json.loads(s)
    print(obj2['name'], obj2['id'], obj2['work']['address'])
    print(obj1 == obj2)

if __name__ == '__main__':
    main(sys.argv)

# result
# JongHyun Jung 12345678 116 Myongji-ro
# True
```
* `json.loads()` 함수를 통해 python dict 객체를 생성하여 Deserialize 합니다. 

## `Test #12` 
### **JSON을 UDP로 전송** <br>
```python
import socket
import json
import sys

def main(argv) : 
    obj1 = {
        'name' : 'JongHyun Jung',
        'id' : 12345678,
        'work' : {
            'name' : 'Myongji University',
            'address' : '116 Myongji-ro'
        }
    }

    s = json.dumps(obj1)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
    sock.sendto(bytes(s, encoding='utf-8'), ('127.0.0.1', 20130))
    (data, sender) = sock.recvfrom(65535)

    obj2 = json.loads(data)
    print(obj2['name'], obj2['id'], obj2['work']['address'])
    print(obj1 == obj2)

if __name__ == '__main__':
    main(sys.argv)
```
* obj1 객체를 Serialize 한 s값을 lab3의 UDP 서버로 `send`합니다.
* UDP 서버로부터 `receive` 받은 data 값을 Deserialize 하여 obj2 객체에 저장합니다.