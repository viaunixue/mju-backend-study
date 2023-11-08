from flask import Flask, make_response, render_template, request
from http import HTTPStatus

app = Flask(__name__)

@app.route('/<int:num1>/<string:op>/<int:num2>', methods=['GET'])
def calculate_get(num1, op, num2):
    result = do_calculation(num1, op, num2)
    if result is None:
        error_resp = make_response({'error':'잘못된 연산자 또는 인수입니다.'}, HTTPStatus.BAD_REQUEST)
        return error_resp
    resp = make_response({'result': result}, HTTPStatus.OK)
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
        error_resp = make_response({'error':'POST 데이터 누락입니다.'}, HTTPStatus.BAD_REQUEST)
        return error_resp

    result = do_calculation(arg1, operator, arg2)
    if result is None:
        error_resp = make_response({'error':'잘못된 연산자 또는 인수입니다.'}, HTTPStatus.BAD_REQUEST)
        return error_resp
    # resp = make_response({'result': result}, HTTPStatus.OK)
    resp = "결과 값은 %s 입니다." %result
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

