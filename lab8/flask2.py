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