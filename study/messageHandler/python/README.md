# Python Message Handler

## `Before` 
UDP echo server 로 JSON 을 돌려 받은 것을 가지고 if 문을 통해 메시지 타입을 구분해서 핸들러 호출합니다. <br> debug 모드로 들어가는 것을 매 핸들러에서 flag 를 확인해야됨을 보여줍니다.
```python
import json
import socket
import sys

debug_flag = False

def onVersionCheck(msg):
    print('onVersionCheck')
    if debug_flag:
        print(msg)

def onLoginRequest(msg):
    print('onLoginRequest')
    if debug_flag:
        print(msg)

def onProfileRequest(msg):
    print('onProfileRequest')
    if debug_flag:
        print(msg)

def onMessage(data):
    msg = json.loads(data)
    t = msg['type']
    if t == 'version_check':
        onVersionCheck(msg)
    elif t == 'login_req':
        onLoginRequest(msg)
    elif t == 'profile_req':
        onProfileRequest(msg)
    else:
        raise Exception('Invalid type: ' + t)

def main(argv):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)

    for line in sys.stdin:
        line = line.strip()
        fields = line.split(' ')
        t = fields[0]
        d = ' '.join(fields[1:])

        # Debug 모드 변경인지 확인
        if t == 'debug':
            global debug_flag
            debug_flag = (d == 'on')
            continue

        msg = json.dumps({'type' : t, 'data' : d})
        sock.sendto(bytes(msg, encoding='utf-8'), ('127.0.0.1', 10001))
        (data, sender) = sock.recvfrom(65536)
        onMessage(data)

if __name__ == '__main__':
    main(sys.argv)
```

## `After` 
메시지 핸들러를 등록한 뒤 debug 모드가 실행되면 핸들러를 바꿔치기 할 수 있음을 보여 줍니다.
```python
import json
import socket
import sys

def onVersionCheck(msg):
    print('onVersionCheck')

def onLoginRequest(msg):
    print('onLoginRequest')

def onProfileRequest(msg):
    print('onProfileRequest')

def debug(msg):
    print(msg)

# Message map 임 : type 문자열 -> handler 맵핑 관리
org_msg_map = {
    'version_check': onVersionCheck,
    'login_req': onLoginRequest,
    'profile_req': onProfileRequest
}

msg_map = org_msg_map.copy()

def onMessage(data):
    msg = json.loads(data)
    t = msg['type']

    if t not in msg_map:
        raise Exception('Invalid type: ' + t)
    else :
        msg_map[t](msg)

def main(argv):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)

    for line in sys.stdin:
        line = line.strip()
        fields = line.split(' ')
        t = fields[0]
        d = ' '.join(fields[1:])

        # Debug 모드 변경인지 확인한다.
        if t == 'debug':
            global msg_map
            if d == 'on':
                if msg_map == org_msg_map:
                    for (t, h) in msg_map.items():
                        msg_map[t] = lambda msg: [h(msg), debug(msg)]
            else:
                if msg_map != org_msg_map:
                    msg_map = org_msg_map.copy()
            continue
        
        msg = json.dumps({'type' : t, 'data' : d})
        sock.sendto(bytes(msg, encoding='utf-8'), ('127.0.0.1', 10001))
        (data, sender) = sock.recvfrom(65536)
        onMessage(data)

if __name__ == '__main__':
    main(sys.argv)
```
* `msg_map[t] = lambda msg: [h(msg), debug(msg)]` 는 각 메세지 유형('t')에 대한 처리기 함수를 수정합니다. <br>
처리기 함수는 원래 처리기 함수('h')와 `debug` 함수의 조합인 람다 함수로 대체됩니다. <br> 
첫 번째 함수 호출은 `h(msg)`로, 이것은 원래의 메세지 처리기 함수('h')를 호출합니다. <br>
두 번째 함수 호출은 `debug(msg)`로, 이것은 디버그 함수인 `debug`를 호출하여 메시지를 출력합니다. <br>
디버그 모드가 활성화되면, 메시지를 처리하는 동시에 디버그 모드를 출력하기 위해 `debug` 함수가 호출됩니다.

## `After 2`
메시지 핸들러가 연속으로 chain 으로 연결되는 경우를 보여줍니다.
```python
import json
import socket
import sys

def onVersionCheck(msg):
    print('onVersionCheck')

def onLoginRequest(msg):
    print('onLoginRequest')

def onProfileRequest(msg):
    print('onProfileRequest')

def debug(msg):
    print(msg)

msg_map = {
    'version_check' : [onVersionCheck],
    'login_req' : [onLoginRequest],
    'profile_req' : [onProfileRequest],
}

def append_handler(t, h):
    msg_map[t].append(h)

def remove_handler(t, h):
    msg_map[t].remove(h)

def onMessage(data):
    msg = json.loads(data)
    t = msg['type']

    if t not in msg_map:
        raise Exception('Invalid type: ' + t)
    else:
        for h in msg_map[t]:
            h(msg)

def main(argv):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)

    for line in sys.stdin:
        line = line.strip()
        fields = line.split(' ')
        t = fields[0]
        d = ' '.join(fields[1:])
    
        if t == 'debug':
            global msg_map
            if d == 'on':
                for (t, h) in msg_map.items():
                    if debug not in h:
                        h.append(debug)
            else:
                for (t, h) in msg_map.items():
                    if debug in h:
                        h.remove(debug)
            continue

    msg = json.dumps({'type': t, 'data': d})
    sock.sendto(bytes(msg, encoding='utf-8'), ('127.0.0.1', 10001))
    (data, sender) = sock.recvfrom(65536)
    onMessage(data)

if __name__ == '__main__':
    main(sys.argv)
```
