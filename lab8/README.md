# lab 8 - Framework - Python Flask

## `Test #1`
### Flask 이용한 Python App 작성
```python
#!/usr/bin/python3
from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello, World!'

@app.route('/bad')
def bad_world():
    return 'Bad World!'

@app.route('/good')
def good_world():
    return 'Good World!'

# 다른 모듈에 import 되는게 아니라 명령줄로 직접 실행될 때 사용됨
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```
> Python 명령 이용하기
```
첫 줄에 shebang을 추가하고 스크립트에 실행 권한이 있다면 직접 실행도 가능합니다.
첫 줄에 다음 추가 : #!/usr/bin/python3
이후 실행
$ chmod a+rx flask1.py
$ ./flask1.py
```
> Flask 명령 이용하기
```
flask 라는 개발용 명령을 통해서 실행하는 것도 가능합니다.

$ flask --app flask1 run 
```
이렇게 명령어를 입력하게 되면 코드 상 지정한 port 가 아닌 default 값인 5000번이 사용됩니다. <br>
&rarr; 이는 flask.py가 직접 실행된 것이 아니라 flask 명령어를 통해 import 되었기 때문에 port 지정 부분이 실행되지 않은 것입니다. <br><br>

> 명령어 결과 값은 다음과 같습니다.

![flask_1](/asset/img/lab8/1.png)

## `Test #2`
### GET 외에 다른 HTTP Method 추가
```python
from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello, World!'

@app.route('/bad', methods=['GET','POST'])
def bad_world():
    return 'Bad World!'

@app.route('/good')
def good_world():
    return 'Good World!'

# 다른 모듈에 import 되는게 아니라 명령줄로 직접 실행될 때 사용됨
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```

> 명령어 결과 값은 다음과 같습니다.

![flask_2](/asset/img/lab8/2.png)

## `Test #3`
### URL 에 변수 사용하기
```python
from flask import Flask

app = Flask(__name__)

@app.route('/<greeting>/<name>')
def greet(greeting, name):
    return f'{greeting}, {name}!'

# 다른 모듈에 import 되는게 아니라 명령줄로 직접 실행될 때 사용됨
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```

> 명령어 결과 값은 다음과 같습니다.

![flask_3](/asset/img/lab8/3.png)

## `Test #4`
### HTTP Response 의 Status Code 지정하기
```python
from flask import Flask
from flask import make_response
from http import HTTPStatus

app = Flask(__name__)

@app.route('/<greeting>/<name>')
def greet(greeting, name):
    resp = make_response(f'{greeting}, {name}!', HTTPStatus.NOT_FOUND)
    return resp


# 다른 모듈에 import 되는게 아니라 명령줄로 직접 실행될 때 사용됨
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```

> 명령어 결과 값은 다음과 같습니다.

![flask_4](/asset/img/lab8/4.png)

## `Test #5`
### HTTP Response 의 Header 바꾸기
```python
from flask import Flask
from flask import make_response
from http import HTTPStatus

app = Flask(__name__)

@app.route('/<greeting>/<name>')
def greet(greeting, name):
    resp = make_response(f'{greeting}, {name}!', HTTPStatus.NOT_FOUND)
    resp.headers['MY_HEADER'] = 1234
    return resp


# 다른 모듈에 import 되는게 아니라 명령줄로 직접 실행될 때 사용됨
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```

> 명령어 결과 값은 다음과 같습니다.

![flask_5](/asset/img/lab8/5.png)

## `Test #6`
### HTTP Request 객체 접근
```python
from flask import Flask
from flask import request

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    method = request.method
    name = request.args.get('name', default='이름이 없는 자')
    client = request.headers['User-Agent']
    return f'{name}, 그대는 {method} 로 호출하였구나. 그대는 {client} 검을 쓰는군.'


# 다른 모듈에 import 되는게 아니라 명령줄로 직접 실행될 때 사용됨
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```

> 명령어 결과 값은 다음과 같습니다.

![flask_6](/asset/img/lab8/6.png)

## `Test #7`
### JSON 반환
```python
from flask import Flask
from flask import request

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    return {
        'method' : request.method,
        'name' : request.args.get('name', default='이름이 없는 자'),
        'client' : request.headers['User-Agent'],
    }

# 다른 모듈에 import 되는게 아니라 명령줄로 직접 실행될 때 사용됨
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```

> 명령어 결과 값은 다음과 같습니다.

![flask_7](/asset/img/lab8/7.png)

## `Test #8`
### Request 에서 JSON 읽기
```python
from flask import Flask
from flask import request

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    return {
        'method' : request.method,
        'name' : request.args.get('name', default='이름이 없는 자'),
        'client' : request.headers['User-Agent'],
        'key1' : request.get_json().get('key1', 'No key')
    }

# 다른 모듈에 import 되는게 아니라 명령줄로 직접 실행될 때 사용됨
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```

> 명령어 결과 값은 다음과 같습니다.

![flask_8](/asset/img/lab8/8.png)

## `Test #9`
### Jinja2 Template
```python
from flask import Flask
from flask import request
from flask import render_template

app = Flask(__name__)

@app.route('/')
def index():
    name = request.args.get('name', default = None)
    return render_template('hello.html', name = name)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```
<br>

`templates` 서브 디렉터리에 다음의 Jinja2 Template을 작성합니다.
```html
<!DOCTYPE html>
{% if name %}
    <h1> Hi, 필멸자 {{ name }}!</h1>
{% else %}
    <h1> Hi, 이름없는 용사!</h1>
{% endif %}
```

> 별도의 터미널에 다음 명령어를 통해 포트 8000으로 열어줍니다. 

![flask_9.1](/asset/img/lab8/9-1.png)

> 브라우저를 통해 결과 값을 확인할 수 있습니다.

![flask_9.2](/asset/img/lab8/9-2.png)

> 이름을 직접 입력하여 결과 값이 반영되는 것을 확인할 수 있습니다.

![flask_9.3](/asset/img/lab8/9-3.png)

## `Test #10`
### 계산기 RESTful API
```python
import json
from flask import Flask, make_response, render_template, request, jsonify
from http import HTTPStatus

app = Flask(__name__)

@app.route('/<int:num1>/<string:op>/<int:num2>', methods=['GET'])
def calculate_get(num1, op, num2):
    result = do_calculation(num1, op, num2)
    if result is None:
        error_resp = make_response('{"error":"잘못된 연산자 또는 인수입니다."}', HTTPStatus.BAD_REQUEST)
        error_resp.headers['Content-Type'] = 'application/json; charset=utf-8'
        return error_resp

    resp_data = {'num1': num1, 'op' : op, 'num2': num2}
    json_resp = json.dumps(resp_data)
    resp = "인수1, 연산자, 인수2의 JSON 값은 다음과 같습니다 : %s <br> 최종 연산 결과 값은 다음과 같습니다: %s" %(json_resp, result)
    return resp

@app.route('/')
def calculate_main():
    return render_template('calculator.html')

@app.route('/post', methods=['POST'])
def calculate_post():
    arg1 = request.form['input1']
    operator = request.form['operator']
    arg2 = request.form['input2']

    try:
        arg1 = int(arg1)
        arg2 = int(arg2)
    except ValueError:
        error_resp = make_response('{"error":"POST 방식에서 JSON에 필요한 데이터 누락입니다"}', HTTPStatus.BAD_REQUEST)
        return error_resp

    result = do_calculation(arg1, operator, arg2)

    if result is None:
        error_resp = make_response('{"error":"연산자 또는 인수를 올바르게 넣어주세요."}', HTTPStatus.BAD_REQUEST)
        return error_resp

    resp_data = {'arg1': arg1, 'operator' : operator, 'arg2': arg2}
    json_resp = json.dumps(resp_data)
    resp = "인수1, 연산자, 인수2의 JSON 값은 다음과 같습니다 : %s <br> 최종 연산 결과 값은 다음과 같습니다: %s" %(json_resp, result)
    return resp

def do_calculation(arg1, operator, arg2) : 
    if operator == '+':
        return arg1 + arg2
    elif operator == '-':
        return arg1 - arg2
    elif operator == '*':
        return arg1 * arg2
    else:
        return None


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```

#### Initial Setting
> 먼저 flask10 파일을 29130 포트로 열어줍니다.
```
$ flask --app flask10 run --port 29130
```
<br>

> 터미널을 새로 열고, 다음 명령어를 통해 localhost:8000 으로 접속하면 <br> 해당 요청이 원격 서버의 localhost:29130로 전달되어 원격 서버의 서비스를 로컬에서 이용할 수 있도록 해줍니다.
```
& ssh -p 10022 -L8000:localhost:29130 sysdesignlab.mju.ac.kr
```
<br>

#### Getting Started
> 간단하게 작성한 html 파일 코드는 다음과 같습니다.
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Calculator (POST request)</title>
</head>
<body>
    <form action="/post" method="post">
        <p> 인자 1 : <input type="number" id="input1" name="input1"> </p>
        <p> 연산자 : <input type="text" id="operator" name="operator"> </p>
        <p> 인자 2 : <input type="number" id="input2" name="input2"> </p>
        <p> 인자와 연산자를 입력하고 제출 버튼을 누르세요. <input type="submit" value="제출" onclick="alert('제출 완료!')" /></p>
    </form>
</body>
</html>
```
<br>

> 브라우저에 localhost:8000을 입력하면 다음의 화면을 확인할 수 있습니다.

![flask_10.1](/asset/img/lab8/10-1.png)
<br>

> 브라우저에 localhost:8000/12/*/5 를 입력하면 GET 요청이 정상 입력 후 200 OK 인 것을 확인할 수 있습니다.

![flask_10.2](/asset/img/lab8/10-2.png)
![flask_10.3](/asset/img/lab8/10-3.png)
<br>

> 브라우저에 localhost:8000/12/**/5 를 입력하면 GET 요청이 비정상 입력 후 400 Bad Request 인 것을 확인할 수 있습니다.

![flask_10.4](/asset/img/lab8/10-4.png)
![flask_10.5](/asset/img/lab8/10-5.png)
<br>

> 브라우저 root 화면에서 html 입력창을 채우고 제출을 누르면 POST 요청이 정상 입력 후 200 OK 인것을 확인할 수 있습니다.

![flask_10.6](/asset/img/lab8/10-6.png)
![flask_10.7](/asset/img/lab8/10-7.png)
![flask_10.8](/asset/img/lab8/10-8.png)
<br>

> 브라우저 root 화면에서 html 입력창을 비우고 제출을 누르면 POST 요청이 비정상 입력 후 400 Bad Request 인것을 확인할 수 있습니다.

![flask_10.9](/asset/img/lab8/10-9.png)
![flask_10.10](/asset/img/lab8/10-10.png)
![flask_10.11](/asset/img/lab8/10-11.png)
<br>

> 브라우저 root 화면에서 html 입력창을 잘못 채우고 제출을 누르면 POST 요청이 비정상 입력 후 400 Bad Request 인것을 확인할 수 있습니다.

![flask_10.12](/asset/img/lab8/10-12.png)
![flask_10.13](/asset/img/lab8/10-13.png)
![flask_10.14](/asset/img/lab8/10-14.png)
<br>

## `TEST #11`
### WSGI가 기대하는 App Server Code 형태

> 다음은 wsgi1.py 파일 코드입니다.
```python
def application(environ, start_response):       # Callable
    print(environ['REQUEST_METHOD'])            # 첫번째 인자는 HTTP request 관련 dict 임
    print(environ['PATH_INFO'])

    status = '200 OK'
    headers = [('Content-Type', 'text/html')]

    start_response(status, headers)             # HTTP response body 반환 전 start_response()를 호출함

    return [b'Hello World wsgi1']               # HTTP body 는 iterable 이어야 함
```

> 터미널에 다음과 같은 명령어로 실행을 요청합니다.

![wsgi1_1](/asset/img/wsgi1_1.png)

> 브라우저에 다음 URL을 입력합니다.

![wsgi1_2](/asset/img/wsgi1_2.png)

<br>

`다음의 uwsgi.ini 설정 파일을 통해 명령문을 간단하게 작성하여 실행할 수 있습니다.`

```ini
[uwsgi]
plugin = python3
socket = 127.0.0.1:19130
```

<br>

> 터미널에 다음과 같은 명령어로 실행을 요청합니다.

![wsgi1_ini_1](/asset/img/wsgi1_ini_1.png)

> 브라우저에 다음 URL을 입력합니다.

![wsgi1_2](/asset/img/wsgi1_2.png)

## `TEST #12`
### HTTP Request Body 읽기

> 다음은 wsgi2.py 파일 코드입니다.
```python
import json

def application(environ, start_response):
    print(environ['REQUEST_METHOD'])
    print(environ['PATH_INFO'])

body_bytes = environ['wsgi.input'].read()
body_str = body_bytes.decode('utf-8')
body_json = json.loads(body_str)

status = '200 OK'
headers = [('Content-Type', 'text/html')]

start_response(status, headers)

response = f'Hello World {body_json["name"]}'
return [bytes(response, encoding='utf-8')]
```

> 터미널에 다음과 같은 명령어로 실행을 요청합니다.

```
uwsgi --ini uwsgi.ini --module wsgi2
```

&rarr; 터미널을 새로 열고, 다음 POST 명령어를 입력합니다.

```
curl -X POST http://localhost/60162176 \-H "Content-Type: application/json; charset=utf-8" --data '{"name":"JongHyun"}'
```

<br>

![wsgi1_ini_2](/asset/img/wsgi_ini_2.png)

### WSGI 와 Flask 연동 문제 해결

> uWSGI 호출 방식을 다음처럼 바꾸어 flask2.py를 실행합니다.
```
$ uwsgi --ini uwsgi.ini \
    --manage-script-name \
    --mount /6016217=flask:app
```

<br>

> `--mount /학번=XXX` 의 역할

이 명령은 /학번 이라는 prefix 뒤에 application server가 동작함을 의미합니다.

<br>

> `--mount /학번=flask2:app` 에서 `flask:app` 의 역할

그리고 이때 application server 가 실행할 내용은 `flask2.py` 이고, <br>
application 이라는 이름의 callable 대신 app 이라는 이름을 사용하게 지정합니다.

<br>

> `--manage-script-name` 역할

`/학번` 이 prefix로 붙어서 요청이 들어오더라도 이걸 제거하고 application server를 호출하게 합니다.

<br>

`다음의 uwsgi2.ini 설정 파일을 통해 명령문을 간단하게 작성하여 실행할 수 있습니다.`
```ini
[uwsgi]
plugin = python3
socket = 127.0.0.1:19130
manage-script-name = true
mount = /60162176=flask2:app
```

> 터미널에 다음과 같은 명령어로 실행을 요청합니다.

```
uwsgi --ini uwsgi2.ini
```

> 터미널을 새로 열고, flask2.py 를 29130 포트에 열어줍니다.
```
flask --app flask2 run --port 29130
```

![flask_ini_1](/asset/img/flask2_ini_1.png)

> 터미널을 새로 열고, 다음 명령어를 통해 localhost:8000 으로 접속하면 <br> 해당 요청이 원격 서버의 localhost:29130로 전달되어 원격 서버의 서비스를 로컬에서 이용할 수 있도록 해줍니다.
```
& ssh -p 10022 -L8000:localhost:29130 sysdesignlab.mju.ac.kr
```

<br>

> 브라우저를 통해 flask2.py 와 WSGI 연동이 정상적으로 이루어지는 것을 확인할 수 있습니다.

![flask_ini_2](/asset/img/flask2_ini_2.png)


## `TEST #13`
### 멀티 쓰레드, 멀티 프로세스

> 다음은 flask11.py 파일 코드입니다.

```python
from flask import Flask

app = Flask(__name__)

count = 0

@app.route('/increase')
def on_increase():
    global count
    count += 1
    return str(count)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=29130)
```
`flask11.py`를 다음 3가지 방법으로 실행하고, 실습 서버에서 대응되는 curl 명령들을 10회 반복합니다.
#### 방법 #1
> 터미널에 다음과 같은 명령어로 실행을 요청합니다.
```
$ flask --app flask11 run --port 29130
```
&rarr; 터미널을 새로 열고, 다음 GET 명령어를 입력합니다.
```
$ curl -X GET http://localhost:29130/increase
```

![flask11_multi_1](/asset/img/flask11_multi_1.png)

#### 방법 #2
> 터미널에 다음과 같은 명령어로 실행을 요청합니다.
```
$ uwsgi --ini uwsgi.ini --manage-script-name \
--mount /60162176=flask11:app --processes=1 --threads=2
```
&rarr; 터미널을 새로 열고, 다음 GET 명령어를 입력합니다.
```
$ curl -X GET http://localhost/60162176/increase
```

![flask11_multi_2](/asset/img/flask11_multi_2.png)

#### 방법 #3
> 터미널에 다음과 같은 명령어로 실행을 요청합니다.
```
$ uwsgi --ini uwsgi.ini --manage-script-name \
--mount /60162176=flask11:app --processes=2 --threads=1
```
&rarr; 터미널을 새로 열고, 다음 GET 명령어를 입력합니다.
```
$ curl -X GET http://localhost/60162176/increase
```

![flask11_multi_3](/asset/img/flask11_multi_3.png)

### Python Application Server 작성 시 주의점

* `Python application server`는 `WSGI`를 통해서 실행됩니다.

* production 환경에서 WSGI가 몇 개의 processes/threads로 실행될 지 모릅니다.

* 만일 모든 요청이 동일한 WSGI process에 의해 실행된다면 앞에서 처럼 전역 변수 값은 유지될 것입니다.

* 그러나 각 요청은 서로 다른 WSGI process에 의해 실행될 것이고 <br>
이는 `전역 변수를 쓰는 것 같은 코드 패턴은 써서는 안된다`는 뜻입니다.

* 즉, application server 에 어떤 state 를 저장해서는 안된다는 뜻입니다.

<br>

> 앞의 수업에서 `Request-Response` 방식이면서 `stateless`인 경우 RESTful API 방식을 사용한다고 했던 것을 기억할 것!

#### 그럼 state는 어디에 있어야 할까?
&rarr; Application server code 가 아니라 외부 DB

#### 만일 state 접근을 많이 해야 하는 app 이라면? (ex. 실시간 게임 서버)
&rarr; RESTful API 방식은 stateless 한 응용에 적합. socket server 방식으로 고려할 것