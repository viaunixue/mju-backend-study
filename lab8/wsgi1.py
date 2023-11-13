def application(environ, start_response):       # Callable
    print(environ['REQUEST_METHOD'])            # 첫번째 인자는 HTTP request 관련 dict 임
    print(environ['PATH_INFO'])

    status = '200 OK'
    headers = [('Content-Type', 'text/html')]

    start_response(status, headers)             # HTTP response body 반환 전 start_response()를 호출함

    return [b'wsgi1.py : Hello World']          # HTTP body 는 iterable 이어야 함