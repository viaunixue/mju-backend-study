# Python Flask

* Micro-framework
    * 핵심 기능만 제공 (Flask 자체는 외부 모듈에 대한 의존성이 낮음)
    * 복잡한 기능들 (ex.ORM)이 필요할 때는 개발자 취향에 맞게 외부 모듈 선택

## 장단점

* 장점
    * 코드 패턴 간단해 빠른 개발 가능
    * 작은 단위 프로젝트에 적합
    * 바닥부터 구조 설계할 때 적합

* 단점
    * Flask 자체 제공하는 보안 기능 부재 (개발자가 직접 구축해야 함)
    * 기본 제공되는 관리 사이트 부재
    * 여러 페이지로 구성되는 웹앱 개발 시 불편함

## Python Django
> 다양한 기본 내장 기능 제공 (`자칭 "배터리 포함" 철학의 프레임 워크`)
<br>

* 장점
    * 다양한 내장으로 빠른 개발 가능 
        * 보안 관련 기능, ORM, 관리 페이지..

* 단점
    * 작은 프로젝트에는 배보다 배꼽이 커짐
    * 작은 단위로 쪼개서 여러 프로젝트로 만드는게 불가능
    <br> 모든 기능을 하나의 Django로 구현하는 monolithic한 방식이 될 수 밖에 없음
    * Django 식으로 개발해야 되는 것들이 많아서 개발자 자유도는 다소 떨어짐
    * 러닝 커브가 다소 있는 편

## 상용 단계에서 서버 구성

* 상용 단계에서 `고성능 소켓 처리`, `작업 큐 관리`, `쓰레드 풀(프로세스 풀)` 등이 필요하므로, <br>
이런 것들이 없는 상태인 앞의 방식대로 app을 띄울 수 없습니다.

* Python 을 이용해서 web application server 를 만드는 경우, <br>
  `web server + WSGI + python script` 구조로 작성하게 됩니다. 

* 개발 단계에서 이것들을 모두 설정하는 것이 번잡하므로, <br>
`Flask` 는 이처럼 개발 단계에서 간단하게 실행할 수 있는 방법을 제공합니다.

## "Flusk fun" 명령의 --debug 기능
```
$ flask --app flask1 run --port 19310 -debug
```
flask1 핸들러에 일부러 오류를 내고 다음 명령을 실행하면, 서버 측에 로그도 남고, 호출하는 측에서도 디버깅 정보를 받습니다.


## Status Code만 반환하고 처리 중단
* Status code를 "return"할 수도 있지만, 이 경우 body에 해당하는 내용을 반드시 같이 반환해야 합니다.
* 대개 오류의 경우 반환할 body가 없으므로 `abort()`함수를 사용해 Status Code만 바로 반환할 수 있습니다.
```python
from flask import abort

@app.route('/login')
def login():
    abort(401)
    this_is_never_executed()
```

## 에러 핸들러 등록
* 몇가지 특수한 오류는 발생 즉시 Flask가 처리합니다. ex) 없는 URL 접근
* 이럴 때 에러 핸들러 등록으로 Flask의 기본 처리를 override 할 수 있습니다.
* 아래 예는 404 Not Found가 발생할 때 page_not_found() 함수를 호출하게 합니다.
```python
@app.errorhandler(404)
def page_not_found(error):
    return 'page_not_found, 404'
```

## SSH -L 또는 -R 옵션
* Flask app 에 curl을 할 수 있다는 점에서 HTML 웹 페이지를 보여주는데도 쓸 수 있을 것 같다는 느낌이 들 것입니다.

* 컴퓨터의 브라우저에서 서버에 돌고 있는 내 Flask app에 바로 접근할 수 있다면 좋겠지만, <br>
보통 서버는 방화벽이 걸려있습니다. 

* SSH 연결은 일종의 터널로 간주됩니다. 따라서 이 터널을 통해 다른 데이터를 주고 받는 것도 가능합니다. 

### 사용법
```
$ ssh -L{내쪽포트}:{상대기준호스트이름}:{상대기준포트} {상대호스트}
ex) $ ssh -L8000:localhost:29130 60162176@sysdesignlab.mju.ac.kr
```
* 의미 : 내쪽 포트로 들어온 트래픽을 
    1. 터널을 통해 실어 보내서 상대 호스트로 전달하고
    2. 상대 기준 호스트 및 상대 기준 포트로 전달함

```
$ ssh -R{상대포트}:{내기준호스트이름}:{내기준포트} {상대호스트}
ex) $ ssh -L8000:localhost:29130 60162176@sysdesignlab.mju.ac.kr
```
* 의미 : 상대 포트로 들어온 트래픽을 
    1. 터널을 통해 실어 와서 내 호스트로 전달하면
    2. 내 기준 호스트 및 내 기준 포트로 전달함

#### SSH -L
```
$ ssh -L8000:another:80 myserver
```
* 의미 : 내쪽 8000번 포트로 들어온 트래픽을
    1. 터널을 통해 실어 보내서 myserver에 도달하면
    2. another 서버의 80번 포트로 전달함 (another 서버는 myserver에서 연결)

![ssh-l](/asset/img/ssh-l.png)

#### SSH -R
```
$ ssh -R8000:another:80 myserver
```
*  의미 : myserver 쪽 8000번 포트로 들어온 트래픽을
    1. 터널을 통해 실어 보내서 나한테 도달하면
    2. another 서버의 80번 포트로 전달함 (another 서버는 내가 연결)

![ssh-r](/asset/img/ssh-r.png)