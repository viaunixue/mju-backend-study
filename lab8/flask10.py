import json
from flask import Flask, make_response, render_template, request, jsonify
from http import HTTPStatus

app = Flask(__name__)

@app.route('/<num1>/<op>/<num2>', methods=['GET'])
def calculate_get(num1, op, num2):   
    result = do_calculation(num1, op, num2)
    if result is None:
        error_resp = make_response('{"error":"잘못된 연산자 또는 인수입니다."}', HTTPStatus.BAD_REQUEST)
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

