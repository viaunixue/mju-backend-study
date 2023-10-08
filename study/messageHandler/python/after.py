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
        
                    
