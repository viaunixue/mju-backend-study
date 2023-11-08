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



