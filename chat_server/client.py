#!/usr/bin/python3

import errno
import json
import select
import socket
import sys

from absl import app, flags

import message_pb2 as pb


FLAGS = flags.FLAGS

flags.DEFINE_string(name='ip', default='127.0.0.1', help='서버 IP 주소')
flags.DEFINE_integer(name='port', default=None, required=True, help='서버 port 번호')
flags.DEFINE_enum(name='format', default='json', enum_values=['json', 'protobuf'], help='메시지 포맷')
#flags.DEFINE_integer(name='verbosity', default=0, required=False, help='디버그용 로그 메시지 출력 정도. 0, 1, 2 가능')


class SocketClosed(RuntimeError):
  pass


class NoTypeFieldInMessage(RuntimeError):
  pass


class UnknownTypeInMessage(RuntimeError):
  def __self__(self, _type):
    self.type = _type

  def __str__(self):
    return str(self.type)


def make_connection_to_server(ip, port):
  '''
  서버에 접속한 후 socket 객체를 반환한다.

  :param ip: IP 주소 문자열
  :param port: port 숫자

  :returns: socket 객체
  '''
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
  sock.connect((ip, port))
  print(f'Connected to {ip}:{port}')

  return sock


def send_messages_to_server(sock, messages):
  '''
  TCP socket 상으로 message 를 전송한다.
  앞에 길이에 해당하는 2 byte 를 network byte order 로 전송한다.

  :param sock: 서버와 맺은 TCP socket
  :param messages: 전송할 message list.각 메시지는 dict type 이거나 protobuf type 이어야 한다.
  '''
  assert isinstance(messages, list)

  for msg in messages:
    msg_str = None
    if FLAGS.format == 'json':
      serialized = bytes(json.dumps(msg), encoding='utf-8')
      msg_str = json.dumps(msg)
    else:
      serialized = msg.SerializeToString()
      msg_str = str(msg).strip()

    # TCP 에서 send() 함수는 일부만 전송될 수도 있다.
    # 따라서 보내려는 데이터를 다 못 보낸 경우 재시도 해야된다.
    to_send = len(serialized)
    to_send_big_endian = int.to_bytes(to_send, byteorder='big', length=2)

    # 받는 쪽에서 어디까지 읽어야 되는지 message boundary 를 알 수 있게끔 2byte 길이를 추가한다.
    serialized = to_send_big_endian + serialized

    if FLAGS.verbosity >= 1:
      print(f'[C->S:총길이={len(serialized)}바이트] 0x{to_send:04x}(메시지크기) {"+ " + msg_str if msg_str else ""}')

    offset = 0
    attempt = 0
    while offset < len(serialized):
      num_sent = sock.send(serialized[offset:])
      if num_sent <= 0:
        raise RuntimeError('Send failed')
      offset += num_sent
      if FLAGS.verbosity >= 2:
        attempt += 1
        print(f'  - send() 시도 #{attempt}: {num_sent}바이트 전송 완료')


def on_help(sock, argv):
  '''
  입력 가능한 문자열을 도움말로 출력한다.

  :param sock: 사용되지 않음
  :param argv: 사용되지 않음
  '''
  for command, _tuple in command_handlers.items():
    handler, help_string = _tuple
    print(f'{command}: {help_string}')


def on_cs_name(sock, argv):
  '''
  서버에 대화명을 지정하게 한다.

  :param sock: 서버 와 연결된 TCP socket
  :param argv: 문자열 또는 문자열 list. list 인 경우 첫번째 인자가 대화명이 됨
  '''
  name = None
  if isinstance(argv, str):
    name = argv
  elif isinstance(argv, list):
    name = ' '.join(argv)
  else:
    raise Exception(f'Invalid argv type: {type(argv)}')

  messages = []
  if FLAGS.format == 'json':
    message = {
      'type': 'CSName',
      'name': name,
    }
    messages.append(message)
  else:
    message = pb.Type()
    message.type = pb.Type.MessageType.CS_NAME
    messages.append(message)

    message = pb.CSName()
    message.name = name
    messages.append(message)

  send_messages_to_server(sock, messages)


def on_cs_rooms(sock, argv):
  '''
  서버에 방 목록을 요청한다.

  :param sock: 서버와 연결된 TCP socket
  :param argv: 사용되지 않음
  '''
  messages = []
  if FLAGS.format == 'json':
    message = {
      'type': 'CSRooms',
    }
    messages.append(message)
  else:
    message = pb.Type()
    message.type = pb.Type.MessageType.CS_ROOMS
    messages.append(message)

    message = pb.CSRooms()
    messages.append(message)

  send_messages_to_server(sock, messages)


def on_cs_create_room(sock, argv):
  '''
  서버에 대화방 생성을 요청한다.

  :param sock: 서버와 연결된 TCP socket
  :param argv: 문자열 또는 문자열 list. list 인 경우 첫번째 인자가 방제목이 됨
  '''
  title = None
  if isinstance(argv, str):
    title = argv
  elif isinstance(argv, list):
    title = ' '.join(argv)
  else:
    raise Exception(f'Invalid argv type: {type(argv)}')

  if not title:
    print('방 제목을 지정해야 됩니다.')
    return

  messages = []
  if FLAGS.format == 'json':
    message = {
      'type': 'CSCreateRoom',
      'title': title,
    }
    messages.append(message)
  else:
    message = pb.Type()
    message.type = pb.Type.MessageType.CS_CREATE_ROOM
    messages.append(message)

    message = pb.CSCreateRoom()
    message.title = title
    messages.append(message)

  send_messages_to_server(sock, messages)


def on_cs_join_room(sock, argv):
  '''
  서버에 대화방 참여를 요청합니다.

  :param sock: 서버와 연결된 TCP socket
  :param argv: 문자열 또는 문자열 list. list 인 경우 첫번째 인자가 방 ID가 됨
  '''
  roomId = None
  if isinstance(argv, str):
    roomId = int(argv)
  elif isinstance(argv, list):
    roomId = int(argv[0]) if argv else None
  else:
    raise Exception(f'Invalid argv type: {type(argv)}')

  if not roomId:
    print('방 번호를 입력해야 됩니다.')
    return

  messages = []
  if FLAGS.format == 'json':
    message = {
      'type': 'CSJoinRoom',
      'roomId': roomId,
    }
    messages.append(message)
  else:
    message = pb.Type()
    message.type = pb.Type.MessageType.CS_JOIN_ROOM
    messages.append(message)

    message = pb.CSJoinRoom()
    message.roomId = roomId
    messages.append(message)

  send_messages_to_server(sock, messages)


def on_cs_leave_room(sock, argv):
  '''
  서버에 대화방 나가기를 통보합니다.

  :param sock: 서버와 연결된 TCP socket
  :param argv: 사용되지 않음
  '''
  messages = []
  if FLAGS.format == 'json':
    message = {
      'type': 'CSLeaveRoom',
    }
    messages.append(message)
  else:
    message = pb.Type()
    message.type = pb.Type.MessageType.CS_LEAVE_ROOM
    messages.append(message)

    message = pb.CSLeaveRoom()
    messages.append(message)

  send_messages_to_server(sock, messages)


def on_cs_chat(sock, text):
  '''
  서버에 채팅 메시지를 전송합니다.

  :param sock: 서버와 연결된 TCP socket
  :param text: 전송할 채팅 메시지
  '''
  messages = []
  if FLAGS.format == 'json':
    message = {
      'type': 'CSChat',
      'text': text,
    }
    messages.append(message)
  else:
    message = pb.Type()
    message.type = pb.Type.MessageType.CS_CHAT
    messages.append(message)

    message = pb.CSChat()
    message.text = text
    messages.append(message)

  send_messages_to_server(sock, messages)


def on_cs_shutdown(sock, argv):
  '''
  서버를 종료합니다.

    :param sock: 서버와 연결된 TCP socket
  :param argv: 사용되지 않음
  '''
  messages = []
  if FLAGS.format == 'json':
    message = {
      'type': 'CSShutdown',
    }
    messages.append(message)
  else:
    message = pb.Type()
    message.type = pb.Type.MessageType.CS_SHUTDOWN
    messages.append(message)

    message = pb.CSLeaveRoom()
    messages.append(message)

  send_messages_to_server(sock, messages)


def on_sc_rooms_result(message):
  '''
  /rooms 명령어에 대한 서버 처리 결과를 받는다.

  :param message: JSON 이거나 SCRoomsResult 의 객체
  '''
  print('---')
  print('[방목록]')
  print('방ID | 방제목 | 멤버들')
  if FLAGS.format == 'json':
    rooms = message.get('rooms', [])
    if rooms:
      for room in rooms:
        print(f"{room['roomId']} | {room['title']} | {','.join([str(name) for name in room['members']])}")
    else:
      print("(없음)")
  else:
    rooms = message.rooms
    if rooms:
      for room in rooms:
        print(f"{room.roomId} | {room.title} | {','.join([str(name) for name in room.members])}")
    else:
      print("(없음)")
  print('---')


def on_sc_chat(message):
  '''
  방에 있는 누군가가 /chat 을 한 경우 서버가 SCChat 을 보내는데 이를 처리한다.

  :param message: JSON 이거나 SCChat 의 객체
  '''
  if FLAGS.format == 'json':
    print(f'[{message["member"]}]', message['text'])
  else:
    print(f'[{message.member}]', message.text)


def on_sc_system_message(message):
  '''
  시스템에서 보내오는 메시지를 처리한다.

  :param message: JSON 이거나 SCSystemMessage 의 객체
  '''
  if FLAGS.format == 'json':
    print('[시스템 메시지]', message['text'])
  else:
    print('[시스템 메시지]', message.text)


command_handlers = {
  '/help': (on_help, '사용 가능 명령어를 나열한다.'),
  '/name': (on_cs_name, '채팅 이름을 지정한다.'),
  '/rooms': (on_cs_rooms, '채팅 방 목록을 출력한다.'),
  '/create': (on_cs_create_room, '채팅 방을 만든다.'),
  '/join': (on_cs_join_room, '채팅 방에 들어간다.'),
  '/leave': (on_cs_leave_room, '채팅 방을 나간다.'),
  '/shutdown': (on_cs_shutdown, '채팅 서버를 종료한다.'),
}

json_message_handlers = {
  'SCRoomsResult': on_sc_rooms_result,
  'SCChat': on_sc_chat,
  'SCSystemMessage': on_sc_system_message,
}

protobuf_message_handlers = {
  pb.Type.MessageType.SC_ROOMS_RESULT: on_sc_rooms_result,
  pb.Type.MessageType.SC_CHAT: on_sc_chat,
  pb.Type.MessageType.SC_SYSTEM_MESSAGE: on_sc_system_message,
}

protobuf_message_parsers = {
  pb.Type.MessageType.SC_ROOMS_RESULT: pb.SCRoomsResult.FromString,
  pb.Type.MessageType.SC_CHAT: pb.SCChat.FromString,
  pb.Type.MessageType.SC_SYSTEM_MESSAGE: pb.SCSystemMessage.FromString,
}


def process_stdin(sock):
  '''
  표준 입력으로부터 한 줄을 읽어서 처리한다.
  '''
  # 명령어를 한 줄 읽는다.
  line = sys.stdin.readline()

  # EOF 인지 확인한다.
  if not line:
    print('=== EOF 에 따라 프로그램을 종료합니다. ===')
    # 관례적으로 정상 종료는 종료 값 0 을 쓴다.
    sys.exit(0)

  # 끝 부분에 따라 들어온 whitespace 를 삭제한다.
  line = line.strip()

  # 앞 부분에 따라 들어온 whitespace 도 삭제한다.
  line = line.rstrip()

  if line:
    # 만일 / 로 시작하지 않는다면 그냥 채팅 메시지인 것처럼 처리한다.
    if not line.startswith('/'):
      on_cs_chat(sock, line)
    else:
      # 공백을 기준으로 잘라서 명령어와 인자들을 구분한다.
      fields = line.split()
      command, args = fields[0], fields[1:]

      # Handler map 에서 해당 명령어의 handler 를 찾아서 호출한다.
      if command in command_handlers:
        command_handlers[command][0](sock, args)
      else:
        print(f'Invalid command: {command}')


# TCP socket 으로부터 현재 읽고 있는 메시지의 크기
# 각 메시지 앞에 newtork byte 로 encoding 한 2byte 정수 크기를 저장해 두는데 그 값에 해당된다.
# 아직 그 정보를 얻지 못했다면 None 을 저장한다.
current_message_len = None


# 메시지 디코딩할 수 있는 만큼의 data 가 없는 경우 임시 저장하는 공간
socket_buffer = None


# Protobuf 의 경우 현재 어떤 메시지를 디코딩하는지 타이입을 저장한다.
# Type 정보가 알려지지 않았을 경우 None 을 저장하고, type 을 알 경우 pb.Type.MessageType enum 을 가리킨다.
current_protobuf_type = None


def process_socket(sock):
  '''
  소켓으로부터의 입력을 처리한다.

  :param sock: 서버와의 TCP 연결 소켓
  '''
  global current_message_len
  global socket_buffer
  global current_protobuf_type

  # Socket 으로부터 데이터가 있기 때문에 이 함수가 불린 것이다.
  # 따라서 recv() 를 실행하고 이를 마지막에 읽었던 내용들에 계속 이어 붙인다.
  # 이렇게 이어 붙인 데이터가 메시지를 만들기에 충분해지면 이후 코드가 실행된다.
  # recv() 에 인자루 주는 숫자는 읽을 bytes 길이가 아니라 최대 길이다.
  received_buffer = sock.recv(65535)
  if not received_buffer:
    raise SocketClosed()

  if not socket_buffer:
    socket_buffer = received_buffer
  else:
    socket_buffer += received_buffer

  if FLAGS.verbosity >= 2:
    print(f'  - recv(): {len(received_buffer)}바이트 읽음')

  # 여태까지 읽은 데이터가 여러 메시지를 포함하고 있을 수 있다.
  # 메시지를 디코딩 할 수 있는 한 계속 반복한다.
  while True:
    # 아직 읽어야될 길이 정보를 모른다면 이번 라운드는 스킵한다.
    if current_message_len is None:
      if len(socket_buffer) < 2:
        return

      # 읽어야 될 길이를 확인했다.
      current_message_len = int.from_bytes(socket_buffer[0:2], byteorder='big')
      socket_buffer = socket_buffer[2:]

    # 현재 가지고 있는 데이터가 메시지를 decoding 하기에 충분하지 않다면 다음을 기약한다.
    if len(socket_buffer) < current_message_len:
      return

    # 처리할 메시지 길이만큼을 잘라낸다.
    serialized = socket_buffer[:current_message_len]
    socket_buffer = socket_buffer[current_message_len:]
    current_message_len = None

    # JSON 은 그 자체로 바로 처리 가능하다.
    if FLAGS.format == 'json':
      if FLAGS.verbosity >= 1:
        print(f'[S->C:총길이={len(serialized) + 2}바이트] 0x{len(serialized):04x}(메시지크기) + {serialized.decode("utf-8")}')

      msg = json.loads(serialized)
      # 'type' 이라는 field 가 있어야 한다.
      msg_type = msg.get('type', None)
      if not msg_type:
        raise NoTypeFieldInMessage()

      # 대응되는 message handler 를 찾아서 호출한다.
      if msg_type in json_message_handlers:
        json_message_handlers[msg_type](msg)
      else:
        raise UnknownTypeInMessage(msg_type)
    else:
      # 현재 type 을 모르는 상태다. 먼저 TypeMessage 를 복구한다.
      if current_protobuf_type is None:
        msg = pb.Type.FromString(serialized)
        if FLAGS.verbosity >= 1:
          str_msg = str(msg).strip()
          print(f'[S->C:총길이={len(serialized) + 2}바이트] 0x{len(serialized):04x}(메시지크기) + {str_msg}')
        if msg.type in protobuf_message_parsers and msg.type in protobuf_message_handlers:
          current_protobuf_type = msg.type
        else:
          raise UnknownTypeInMessage(msg.type)
      else:
        # type 을 알고 있으므로 parser 를 이용해서 메시지를 복구한다.
        msg = protobuf_message_parsers[current_protobuf_type](serialized)
        if FLAGS.verbosity >= 1:
          str_msg = str(msg).strip()
          print(f'[S->C:총길이={len(serialized) + 2}바이트] 0x{len(serialized):04x}(메시지크기) {"+ " + str_msg if str_msg else ""}')

        # type 에 따른 message handler 를 찾아서 호출한다.
        try:
          protobuf_message_handlers[current_protobuf_type](msg)
        finally:
          # 다음에 type 부터 decoding 하게끔 초기화 한다.
          current_protobuf_type = None


def main(argv):
  '''
  메인 함수

  :param argv: 명령줄로 넘긴 인자들
  '''
  if not FLAGS.ip:
    print('서버의 IP 주소를 지정해야 됩니다.')
    # 관례적으로 오류인 경우 0 이 아닌 종료 값을 쓴다.
    # 에러 케이스에 따라 서로 다른 에러코드를 사용할 수도 있다.
    sys.exit(1)

  if not FLAGS.port:
    print('서버의 Port 번호를 지정해야 됩니다.')
    # 에러 케이스에 따라 서로 다른 에러코드를 사용할 수도 있다.
    sys.exit(2)

  sock = make_connection_to_server(FLAGS.ip, FLAGS.port)

  # 종료에 관련된 입력이 들어올 때까지 반복한다.
  while True:
    try:
      # 입력으로 받을 것들을 read set 에 추가한다.
      # C++ 에서는 fd_set 타입을 썼으나 python 은 list 형태를 쓰는 것이 다를 뿐 기본 사용 방법은 비슷하다.
      # 또한 C++ 에서는 인자로 넘긴 fd_set 을 select() 가 변경했으나, python 에서는 반환값으로 알려준다.
      inputs = [sock, sys.stdin]
      readable, writable, exceptionable = select.select(inputs, [], [], None)

      # 표준 입력으로 입력이 주어졌는지를 확인한다.
      if sys.stdin in readable:
        try:
          process_stdin(sock)
        except UnicodeDecodeError:
          # 한글 타이핑 중 어중간하게 삭제 한 경우 decode 오류가 나는 경우가 있는데,
          # 이 때는 입력을 아예 무시함.
          pass

      if sock in readable:
        process_socket(sock)

    except SocketClosed:
      print('서버가 소켓을 닫았음')
      # while 문을 종료한다.
      break

    except NoTypeFieldInMessage:
      print('서버가 보낸 메시지에 type 필드가 없음')
      # while 문을 종료한다.
      break

    except UnknownTypeInMessage as err:
      print(f'서버가 핸들러에 등록되지 않은 메시지 타입을 보내옴: 타입={err}')
      # while 문을 종료한다.
      break

    except socket.error as err:
      if err.errno == errno.ECONNRESET:
        print('서버가 소켓을 닫았음')
      else:
        print(f'소켓 에러: {err}')
      # while 문을 종료한다.
      break

  # 소켓을 닫아준다.
  if sock:
    sock.close()


if __name__ == '__main__':
  app.run(main)