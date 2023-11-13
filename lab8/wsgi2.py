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

    response = f'wsgi2.py : Hello World {body_json["name"]} \n'
    return [bytes(response, encoding='utf-8')]

    <br>

    > ㅇㄴㅁㅇㄹ