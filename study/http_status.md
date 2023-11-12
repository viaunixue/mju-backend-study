## 많이 쓰는 HTTP Status Code 

### 1XX : 정보 전달

> 100 Continue : 데이터 계속 요청

> 101 Switching Protocols : 프로토콜 변경 (예: HTTP &rarr; WebSocket)

### 2XX : 성공

> 200 : OK

> 201 Created : (POST 등으로 새로운 리소스 만들어짐)

> 202 Accepted : 요청을 성공적으로 받음 But 아직 처리는 못함

### 3XX : Redirection

> 301 Moved Permanently : 리소스 위치가 변경되었으니 다음부터는 거기로 요청할 것

### 4XX : 클라이언트 오류

> 400 Bad Request : 필요한 매개변수 등이 채워지지 않았거나 하는 경우

> 401 Unauthorized : 인증이 필요한데 인증 정보(auth credential)가 없는 경우

> 403 Forbidden : 권한이 없거나 중복 데이터 생성 등 허용되지 않는 동작

> 404 Not Founded : 리소스 없음

> 405 Method Not Allowed : ex. GET만 되는데 POST를 한다거나 하는 경우

> 406 Conflict : 같은 리소스에 동시에 업데이트 요청을 보낸다거나 하는 경우

### 5XX : 서버 오류

> 500 Internal Server Erro : 서버 연산 중 오류가 발생했다는 일반적인 내용

> 501 Not Implemented : 아직 구현되지 않은 REST API 등의 경우

> 502 Bad Gateway : 웹 서버는 application server를 위한 gateway 로 동작 중인데 <br> 
application server가 응답하지 않거나 하는 등의 문제가 발생