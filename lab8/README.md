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