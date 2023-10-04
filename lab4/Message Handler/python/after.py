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
