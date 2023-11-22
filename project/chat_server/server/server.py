#!/usr/bin/python3

import enum
import errno
import json
import select
import socket
import sys
import threading

from absl import app, flags

import message_pb2 as pb


FLAGS = flags.FLAGS

flags.DEFINE_integer(name='port', default=None, required=True, help='서버 port 번호')
flags.DEFINE_enum(name='format', default='json', enum_values=['json', 'protobuf'], help='메시지 포맷')
flags.DEFINE_integer(name='workers', default=2, help='작업 쓰레드 숫자')


shutdown_requested = False


class Receiver(enum.Enum):
  ALL, ONLY_ME, EXCEPT_ME = range(3)


class SocketClosed(RuntimeError):
  pass


class NoTypeFieldInMessage(RuntimeError):
  pass


class UnknownTypeInMessage(RuntimeError):
  def __self__(self, _type):
    self.type = _type

  def __str__(self):
    return str(self.type)


class Client:
  def __init__(self, sock: socket.socket, addr):
    self.sock = sock
    self.addr = addr
    self.to_flush : list[bytes] = []
    self.socket_buffer : bytes = None
    self.current_message_len : int = None
    self.current_protobuf_type : pb.Type.MessageType = None
    self._name : str = None
    self.current_room : Room = None

  def __str__(self):
    return f'{self.addr}:{self._name}'

  @property
  def name(self):
    return self._name or str(self.addr)

  def clean_up(self):
    if self.current_room:
      with rooms_mutex:
        self.current_room.members.remove(self)
        if not self.current_room.members:
          print(f'방[{self.current_room.room_id}]: 접속 종료로 인한 방폭')
          del rooms[self.current_room.room_id]
    self.current_room = None

    if self.sock:
      self.sock.close()
    self.sock = None

  def recv_from_socket(self):
    received_buff = self.sock.recv(65536)
    if not received_buff:
      raise SocketClosed()

    if FLAGS.verbosity >= 2:
      print(f'  - 클라이언트 [{self}]: recv(): {len(received_buff)}바이트 읽음')

    if not self.socket_buffer:
      self.socket_buffer = received_buff
    else:
      self.socket_buffer += received_buff

    if self.current_message_len is None:
      if len(self.socket_buffer) < 2:
        return False

      self.current_message_len = int.from_bytes(self.socket_buffer[0:2], byteorder='big')
      if FLAGS.verbosity >= 2:
        print(f'  - 클라이언트 [{self}] 다음 메시지 길이: {self.current_message_len}')
      self.socket_buffer = self.socket_buffer[2:]

    if len(self.socket_buffer) < self.current_message_len:
      print(f'Wait more: {len(self.socket_buffer)} < {self.current_message_len}')
      return False

    return True

  def send_messages(self, messages):
    assert isinstance(messages, list)

    for msg in messages:
      msg_as_str = None
      if FLAGS.format == 'json':
        serialized = bytes(json.dumps(msg), encoding='utf-8')
        msg_as_str = json.dumps(msg)
      else:
        serialized = msg.SerializeToString()
        msg_as_str = str(msg).strip()

      # TCP 에서 send() 함수는 일부만 전송될 수도 있다.
      # 따라서 보내려는 데이터를 다 못 보낸 경우 재시도 해야된다.
      to_send = len(serialized)

      # 받는 쪽에서 어디까지 읽어야 되는지 message boundary 를 알 수 있게끔 2byte 길이를 추가한다.
      serialized = int.to_bytes(to_send, byteorder='big', length=2) + serialized
      self.to_flush.append(serialized)
      if FLAGS.verbosity >= 1:
        print(f'클라이언트 [{self}]: [S->C:총길이={len(serialized)}바이트] 0x{to_send:04x}(메시지크기) + {msg_as_str}')

  def flush(self):
    for serialized in self.to_flush:
      offset = 0
      count = 0
      while offset < len(serialized):
        count += 1
        num_sent = self.sock.send(serialized[offset:])
        if num_sent <= 0:
          raise RuntimeError('Send failed')
        if FLAGS.verbosity >= 2:
          print(f'  - 클라이언트 [{self}] send() 시도 #{count}: {num_sent}바이트 전송 완료')
        offset += num_sent
    self.to_flush.clear()

  def process_messages(self):
    assert self.current_message_len <= len(self.socket_buffer)
    serialized = self.socket_buffer[:self.current_message_len]
    self.socket_buffer = self.socket_buffer[self.current_message_len:]
    self.current_message_len = None

    # JSON 은 그 자체로 바로 처리 가능하다.
    if FLAGS.format == 'json':
      if FLAGS.verbosity >= 1:
        print(f'클라이언트 [{self}]: [C->S:총길이={len(serialized) + 2}바이트] 0x{len(serialized):04x}(메시지크기) + {serialized.decode("utf-8")}')

      msg = json.loads(serialized)
      # 'type' 이라는 field 가 있어야 한다.
      msg_type = msg.get('type', None)
      if not msg_type:
        raise NoTypeFieldInMessage()

      # 대응되는 message handler 를 찾아서 호출한다.
      if msg_type in json_message_handlers:
        if msg_type != 'CSShutdown':
          json_message_handlers[msg_type](self, msg)
        else:
          json_message_handlers[msg_type]()
      else:
        raise UnknownTypeInMessage(msg_type)
    else:
      # 현재 type 을 모르는 상태다. 먼저 TypeMessage 를 복구한다.
      if self.current_protobuf_type is None:
        msg = pb.Type.FromString(serialized)
        if FLAGS.verbosity >= 1:
          str_msg = str(msg).strip()
          print(f'클라이언트 [{self}] [C->S:총길이={len(serialized) + 2}바이트] 0x{len(serialized):04x}(메시지크기) + {str_msg}')
        if msg.type in protobuf_message_parsers and msg.type in protobuf_message_handlers:
          self.current_protobuf_type = msg.type
        else:
          raise UnknownTypeInMessage(msg.type)
      else:
        # type 을 알고 있으므로 parser 를 이용해서 메시지를 복구한다.
        msg = protobuf_message_parsers[self.current_protobuf_type](serialized)
        if FLAGS.verbosity >= 1:
          str_msg = str(msg).strip()
          print(f'클라이언트 [{self}] [C->S:총길이={len(serialized) + 2}바이트] 0x{len(serialized):04x}(메시지크기) {"+ " + str_msg if str_msg else ""}')

        # type 에 따른 message handler 를 찾아서 호출한다.
        try:
          if self.current_protobuf_type != pb.Type.MessageType.CS_SHUTDOWN:
            protobuf_message_handlers[self.current_protobuf_type](self, msg)
          else:
            protobuf_message_handlers[self.current_protobuf_type]()
        finally:
          # 다음에 type 부터 decoding 하게끔 초기화 한다.
          self.current_protobuf_type = None

  def send_system_message(self, text, receiver=Receiver.ALL):
    messages = []
    if FLAGS.format == 'json':
      msg = {
        'type': 'SCSystemMessage',
        'text': text,
      }
      messages.append(msg)
    else:
      msg = pb.Type()
      msg.type = pb.Type.MessageType.SC_SYSTEM_MESSAGE
      messages.append(msg)

      msg = pb.SCSystemMessage()
      msg.text = text
      messages.append(msg)

    if receiver == Receiver.ONLY_ME:
      self.send_messages(messages)
    else:
      assert self.current_room
      with rooms_mutex:
        for msg in self.current_room.members:
          if msg != self or receiver == Receiver.ALL:
            msg.send_messages(messages)

  def on_cs_name(self, message):
    prev = self.name

    if FLAGS.format == 'json':
      self._name = message['name']
    else:
      self._name = message.name

    if self.current_room:
      text = f'{prev} 의 이름이 {self._name} 으로 변경되었습니다.'
      self.send_system_message(text, receiver=Receiver.ALL)
    else:
      text = f'이름이 {self._name} 으로 변경되었습니다.'
      self.send_system_message(text, receiver=Receiver.ONLY_ME)

  def on_cs_rooms(self, message):
    rooms_info = []
    with rooms_mutex:
      for room in rooms.values():
        if FLAGS.format == 'json':
          room_info = {
            'roomId': room.room_id,
            'title': room.title,
            'members': [m.name for m in room.members],
          }
          rooms_info.append(room_info)
        else:
          room_info = pb.SCRoomsResult.RoomInfo()
          room_info.roomId = room.room_id
          room_info.title = room.title
          room_info.members.extend([m.name for m in room.members])
          rooms_info.append(room_info)

    messages = []
    if FLAGS.format == 'json':
      msg = {
        'type': 'SCRoomsResult',
        'rooms': rooms_info,
      }
      messages.append(msg)
    else:
      msg = pb.Type()
      msg.type = pb.Type.MessageType.SC_ROOMS_RESULT
      messages.append(msg)

      msg = pb.SCRoomsResult()
      msg.rooms.extend(rooms_info)
      messages.append(msg)
    self.send_messages(messages)

  def on_cs_create_room(self, message):
    if self.current_room:
      text = '대화 방에 있을 때는 방을 개설 할 수 없습니다.'
      self.send_system_message(text, receiver=Receiver.ONLY_ME)
      return

    if FLAGS.format == 'json':
      title = message['title']
    else:
      title = message.title

    with rooms_mutex:
      global next_room_id
      if not next_room_id:
        next_room_id = 1
      else:
        next_room_id += 1
      room = Room(next_room_id, title)
      room.members.append(self)
      rooms[room.room_id] = room
      self.current_room = room

      print(f'방[{room.room_id}]: 생성. 방제 {room.title}')

    text = f'방제[{title}] 방에 입장했습니다.'
    self.send_system_message(text, receiver=Receiver.ONLY_ME)

  def on_cs_join_room(self, message):
    if self.current_room:
      text = '대화 방에 있을 때는 다른 방에 들어갈 수 없습니다.'
      self.send_system_message(text, receiver=Receiver.ONLY_ME)
      return

    if FLAGS.format == 'json':
      room_id = int(message['roomId'])
    else:
      room_id = int(message.roomId)

    not_found = False
    title = None
    with rooms_mutex:
      if room_id in rooms:
        room = rooms[room_id]
        room.members.append(self)
        self.current_room = room
        title = room.title
      else:
        not_found = True

    if not_found:
      text = '대화방이 존재하지 않습니다.'
      self.send_system_message(text, receiver=Receiver.ONLY_ME)
      return

    text = f'방제[{title}] 방에 입장했습니다.'
    self.send_system_message(text, receiver=Receiver.ONLY_ME)

    text = f'[{self.name}] 님이 입장했습니다.'
    self.send_system_message(text, receiver=Receiver.EXCEPT_ME)

  def on_cs_leave_room(self, message):
    if not self.current_room:
      text = '현재 대화방에 들어가 있지 않습니다.'
      self.send_system_message(text, receiver=Receiver.ONLY_ME)
      return

    text = f'[{self.name}] 님이 퇴장했습니다.'
    self.send_system_message(text, receiver=Receiver.EXCEPT_ME)

    title = None
    with rooms_mutex:
      title = self.current_room.title
      self.current_room.members.remove(self)
      if len(self.current_room.members) == 0:
        del rooms[self.current_room.room_id]
        print(f'방[{self.current_room.room_id}]: 명시적 /leave 명령으로 인한 방폭', )
      self.current_room = None

    text = f'방제[{title}] 대화 방에서 퇴장했습니다.'
    self.send_system_message(text, receiver=Receiver.ONLY_ME)

  def on_cs_chat(self, message):
    if not self.current_room:
      text = '현재 대화방에 들어가 있지 않습니다.'
      self.send_system_message(text, receiver=Receiver.ONLY_ME)
      return

    messages = []
    if FLAGS.format == 'json':
      msg = {
        'type': 'SCChat',
        'member': self.name,
        'text': message['text']
      }
      messages.append(msg)
    else:
      msg = pb.Type()
      msg.type = pb.Type.MessageType.SC_CHAT
      messages.append(msg)

      msg = pb.SCChat()
      msg.member = self.name
      msg.text = message.text
      messages.append(msg)

    with rooms_mutex:
      for member in self.current_room.members:
        if member != self:
          member.send_messages(messages)


class Room:
  def __init__(self, room_id, title):
    self.room_id = room_id
    self.title = title
    self.members : list[Client] = []


clients_after_message_processing : list[Client] = []
clients_after_message_processing_mutex = threading.Lock()

clients_for_message_processing : list[Client] = []
clients_for_message_processing_mutex = threading.Lock()
clients_for_message_processing_cv = threading.Condition(clients_for_message_processing_mutex)

next_room_id : int = None
rooms : dict[str, Room] = {}
rooms_mutex = threading.Lock()


def on_cs_shutdown():
  print('서버 중지가 요청됨')
  global shutdown_requested
  shutdown_requested = True
  with clients_for_message_processing_mutex:
    clients_for_message_processing_cv.notify_all()


json_message_handlers = {
  'CSName': Client.on_cs_name,
  'CSRooms': Client.on_cs_rooms,
  'CSCreateRoom': Client.on_cs_create_room,
  'CSJoinRoom': Client.on_cs_join_room,
  'CSLeaveRoom': Client.on_cs_leave_room,
  'CSChat': Client.on_cs_chat,
  'CSShutdown': on_cs_shutdown,
}

protobuf_message_handlers = {
  pb.Type.MessageType.CS_NAME: Client.on_cs_name,
  pb.Type.MessageType.CS_ROOMS: Client.on_cs_rooms,
  pb.Type.MessageType.CS_CREATE_ROOM: Client.on_cs_create_room,
  pb.Type.MessageType.CS_JOIN_ROOM: Client.on_cs_join_room,
  pb.Type.MessageType.CS_LEAVE_ROOM: Client.on_cs_leave_room,
  pb.Type.MessageType.CS_CHAT: Client.on_cs_chat,
  pb.Type.MessageType.CS_SHUTDOWN: on_cs_shutdown,
}

protobuf_message_parsers = {
  pb.Type.MessageType.CS_NAME: pb.CSName.FromString,
  pb.Type.MessageType.CS_ROOMS: pb.CSRooms.FromString,
  pb.Type.MessageType.CS_CREATE_ROOM: pb.CSCreateRoom.FromString,
  pb.Type.MessageType.CS_JOIN_ROOM: pb.CSJoinRoom.FromString,
  pb.Type.MessageType.CS_LEAVE_ROOM: pb.CSLeaveRoom.FromString,
  pb.Type.MessageType.CS_CHAT: pb.CSChat.FromString,
  pb.Type.MessageType.CS_SHUTDOWN: pb.CSShutdown.FromString,
}


def message_worker_func(args):
  print(f'메시지 작업 쓰레드 #{args} 생성')

  while not shutdown_requested:
    with clients_for_message_processing_mutex:
      while not shutdown_requested and not clients_for_message_processing:
        clients_for_message_processing_cv.wait()

      if shutdown_requested:
        break
      client = clients_for_message_processing.pop(0)

    try:
      client.process_messages()

      with clients_after_message_processing_mutex:
        clients_after_message_processing.append(client)

    except RuntimeError as err:
      print('Exception', err)
      client.clean_up()

  print(f'메시지 작업 쓰레드 #{args} 종료')


def main(argv):
  '''
  메인 함수

  :param argv: 명령줄로 넘긴 인자들
  '''
  global shutdown_requested

  if not FLAGS.port:
    print('서버의 Port 번호를 지정해야 됩니다.')
    # 에러 케이스에 따라 서로 다른 에러코드를 사용할 수도 있다.
    sys.exit(2)

  worker_threads : list[threading.Thread] = []
  for i in range(FLAGS.workers):
    thread = threading.Thread(target=message_worker_func, args=[i])
    thread.start()
    worker_threads.append(thread)

  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
  sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
  sock.bind(('0.0.0.0', FLAGS.port))
  sock.listen()

  clients : list[Client] = []

  print(f'Port 번호 {FLAGS.port}에서 서버 동작 중')
  while not shutdown_requested:
    inputs = [sock]
    outputs = []

    with clients_after_message_processing_mutex:
      if clients_after_message_processing:
        clients.extend(clients_after_message_processing)
        clients_after_message_processing.clear()

    for client in clients:
      inputs.append(client.sock)
      outputs.append(client.sock)

    try:
      readable, writable, exceptionable = select.select(inputs, outputs, [], 0.1)

      if sock in readable:
        client_sock, addr = sock.accept()
        client = Client(client_sock, addr)
        clients.append(client)
        print(f'새로운 클라이언트 접속 [{client}]')

      to_remove : list[Client] = []
      for client in clients:
        try:
          if client.sock in writable and client.to_flush:
            client.flush()

          if client.sock in readable:
            if client.recv_from_socket():
              with clients_for_message_processing_mutex:
                clients_for_message_processing.append(client)
                clients_for_message_processing_cv.notify()
              to_remove.append(client)

        except SocketClosed:
          print(f'클라이언트 [{client}]: 상대방이 소켓을 닫았음')
          client.clean_up()
          to_remove.append(client)

        except NoTypeFieldInMessage:
          print(f'클라이언트 [{client}]: 메시지에 타입 필드가 없음')
          client.clean_up()
          to_remove.append(client)

        except UnknownTypeInMessage as err:
          print(f'클라이언트 [{client}]: 핸들러에 등록되지 않은 메시지 타입: {err}')
          client.clean_up()
          to_remove.append(client)

        except socket.error as err:
          if err.errno == errno.ECONNRESET:
            print(f'클라이언트 [{client}]: 상대방이 소켓을 닫았음')
          else:
            print(f'소켓 에러: {err}')
          client.clean_up()
          to_remove.append(client)

      for client in to_remove:
        clients.remove(client)

    except KeyboardInterrupt:
      print('키보드로 프로그램 강제 종료 요청')
      on_cs_shutdown()

  print('Main thread 종료 중')

  for thread in worker_threads:
    print('작업 쓰레드 join() 시작')
    thread.join()
    print('작업 쓰레드 join() 완료')

  for client in clients:
    client.sock.close()
    print(f'접속 중인 클라이언트 {client} 소켓 close() 완료')


if __name__ == '__main__':
  app.run(main)
