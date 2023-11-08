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