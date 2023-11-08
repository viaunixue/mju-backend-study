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