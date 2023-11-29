# 실행환경

memo.py 는 Python3 Flask 로 되어있습니다.

**본 과제는 실습 서버가 아니라 본인 컴퓨터에서 작업하기 바랍니다.**

# 필요 패키지 설치

필요한 패키지 목록은 `requirements.txt` 에 있습니다. `pip` 을 이용해 필요 패키지를 설치합니다.
(필요시 `virtualenv` 환경을 이용하세요.)

```
$ pip install -r requirements.txt
```

# 실행 예시

일반적인 flask 실행 방식대로 실행하면 됩니다.

```
$ flask --app memo run --port 포트 번호
```
또는
```
$ python3 memo.py
```

후자의 경우 memo.py 안에서 port 번호 5000번을 기본값으로 사용하고 있으니 필요시 수정하세요.

# 동작 설명

## index.html 읽어 오기

memo.py 를 실행하고 브라우저에서 `http://localhost:포트번호` 처럼 접근할 경우 `index.html` 을 읽어오게 됩니다.

이는 `Flask` 의 `template` 기능을 사용하고 있으며, 사용되고 있는 `index.html` 의 template file 은 `templates/index.html` 에 위치하고 있습니다.

이 template 은 현재 `name` 이라는 변수만을 외부 변수 값으로 입력 받습니다. 해당 변수는 유저가 현재 로그인 중인지를 알려주는 용도로 사용되며 `index.html` 은 그 값의 유무에 따라 다른 내용을 보여줍니다.

## index.html 이 호출하는 REST API 들

`index.html` 은 `memo.py` 에 다음 API 들을 호출합니다.

* `GET /login` : authorization code 를 얻어오는 URL 로 redirect 시켜줄 것을 요청합니다. (아래 설명)

* `GET /memo` : 현재 로그인한 유저가 작성한 메모 목록을 JSON 으로 얻어옵니다. 결과 JSON 은 다음과 같은 형태가 되어야 합니다.
  ```
  {"memos": ["메모내용1", "메모내용2", ...]}
  ```

* `POST /memo` : 새 메모를 추가합니다. HTTP 요청은 다음과 같은 JSON 을 전송해야 됩니다.
  ```
  {"text": "메모내용"}
  ```
  새 메모가 생성된 경우 memo.py 는 `200 OK` 를 반환합니다.


## 네이버 로그인 API 호출

수업 시간에 설명한대로 authorization code 를 얻어오는 동작은 클라이언트에서부터 시작하게 됩니다.

그런데 코드를 보면 `index.html` 에서 해당 API 동작을 바로 시작하는 것이 아니라 `GET /login` 을 통해서 서버에게 해당 REST API 로 redirect 시켜달라고 하는 방식으로 브라우저가 API 를 호출합니다. 이는 Chrome 계열의 브라우저의 `CORS` 문제 때문에 그렇습니다.

비록 서버가 redirect 해주는 방식을 사용하고는 있지만, 클라이언트인 브라우저가 그 API 를 직접 호출한다는 점은 동일합니다.

## 로그인 혹은 가입 처리

네이버 OAuth 과정을 마무리 한 뒤에 네이버의 profile API 를 통해 얻은 유저의 고유 식별 번호를 갖는 유저가 DB 에 없는 경우 새 유저로 취급하고 DB 에 해당 유저의 이름을 포함하는 레코드를 생성합니다.

만일 같은 네이버 고유 식별 번호의 유저가 있다면 그냥 로그인 된 것으로 간주합니다.

어떤 경우든 DB 에서 해당 유저의 정보를 얻어낼 수 있도록 `userId` 라는 `HTTP cookie` 를 설정합니다.


# 과제 내용

`memo.py` 의 내용을 채워서 메모장이 동작하게 하는 것이 과제의 목표입니다.

## `def home()`

`userId` 쿠키가 설정되어 있는 경우 DB 에서 해당 유저의 이름을 읽어와서 `index.html` template 에 반영하는 동작이 누락되어 있습니다.

## Redirect URI 에 대응되는 함수 (예: `def onOAuthAuthorizationCodeRedirected()`)

본인이 네이버 앱 등록시 설정한 Redirect URI 에 대응되는 주소의 handler 를 `memo.py` 에 구현해야됩니다. 현재 `memo.py` 에는 `memo.py` 가 8000 번 포트에 뜰 것이라고 가정하고 `http://localhost:8000/auth` 와 같은 형태로 네이버 앱 등록에 Redirect URI 가 지정된 것으로 가정해서 `def onOAuthAuthorizationCodeRedirected()` 에 `@app.route('/auth')` 라는 태깅이 되어있습니다. 만일 본인이 등록한 Redirect URI 와 이와 다르다면 `@app.route()` 을 적절히 수정해야 됩니다.

그리고 현재 `def onOAuthAuthorizationCodeRedirected()` 의 내용은 비어있습니다. 코멘트로 표시된 대로 단계 1 ~ 4 까지를 채워 넣어야 합니다.

## `def getMemos()`

메모 목록을 DB 로부터 읽어오는 부분이 빠져있습니다. 현재 로그인한 유저의 메모들을 읽어올 수 있어야 합니다.

## `def post_new_memo()`

새 메모를 DB 에 저장하는 부분이 빠져있습니다. 현재 로그인한 유저의 메모로 저장되어야 합니다.


# DB 사용

DB 는 본인이 원하는 DB 중 어떤 것이라도 쓸 수 있습니다. (예: MySQL, MariaDB, Redis, MongoDB) DB 를 하나만 써도 되고 필요하다면 여러 DB 를 사용해도 무방합니다.

단 DB 설치는 docker 를 통해서 이루어져야 합니다. Docker 에 대한 설명은 수업 시간 강의 내용을 참고해주세요.

# 평가 항목

* 네이버 OAuth 가 제대로 구현되었는가?
* `GET /memo` 가 제대로 구현되었는가?
* `POST /memo` 가 제대로 구현되었는가?
* 유저를 바꿔서 사용하는 경우 사용자 구분이 잘 되는가?

# 제출물

본인 github repo 에 `memo_server` 라는 서브폴더를 만들어서 다음 파일들을 제출하세요.
* 완성된 `memo.py`
* 기본으로 주어진 `templates/index.html`
* 실행 방법 및 코드 설명을 담고 있는 `readme.md` 파일

* `AWS` 상에 `ELB` 를 통해서 실제 배포된 실행 환경 (**이 부분은 곧 이어지는 수업 시간에서 다루게 됩니다.**)

# 테스트용 서버

실습 서버(= `sysdesignlab.mju.ac.kr`) 의 `10011`번 포트에 테스트해볼 수 있는 서버가 동작하고 있습니다. 개인 컴퓨터에서 `ssh -L` 을 이용해서 접속하세요.

```
$ ssh -L8000:localhost:10011 -p 10022 본인아이디@sysdesignlab.mju.ac.kr
```

그 뒤에 브라우저에 `http://localhost:8000` 을 입력해서 테스트 해볼 수 있습니다.