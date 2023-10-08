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