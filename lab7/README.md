# lab 7 - RESTful API

## `Test #1`
### Naver에 앱 등록하기
![naver_1](/asset/img/naver_1.png)

## `Test #2`
### Naver에 앱 등록하기
```
$ curl "https://openapi.naver.com/v1/papago/n2mt" \
"-H "Content-Type: application/x-www-form-rulencoded; charset=UTF-8" \
"-H "X-Naver-Client-Id: {애플리케이션 등록 시 발급받은 클라이언트 아이디 값} \
"-H "X-Naver-Secret: {애플리케이션 등록 시 발급받은 클라이언트 시크릿 값} \
"source=en&target=zh-CN&text=Hello World" -v
```
![naver_2](/asset/img/naver_2.png)


## `Test #3`
### Naver 파파고 번역
![naver_3](/asset/img/naver_3.png)

## `Test #4`
### Naver 블로그 검색
![naver_4](/asset/img/naver_4.png)

## `Test #5`
### Kakao 앱 등록
![kakao_1](/asset/img/kakao_1.png)

## `Test #6`
### Daum 검색
![kakao_2](/asset/img/kakao_2.png)

## `Test #7`
### 네이버 로그인
> PORT 10000번 서버로 아래 링크의 Auth Code 받은 결과 화면
```
https://nid.naver.com/oauth2.0/authorize?response_type=code&client_id=bop3C73HErShvaWh5AT2&redirect_uri=http%3A%2F%2Flocalhost%3A10000&state=0115
```

![Auth_1](/asset/img/auth_5.png)

> 아래 링크에 Auth Code를 넣고 Access token 얻는 결과 화면
```
https://nid.naver.com/oauth2.0/token?grant_type=authorization_code&client_id=bop3C73HErShvaWh5AT2&client_secret=6IdFnhihKH&code=9jgfquFfnEa51MGGMP&state=0115
```
![Auth_2](/asset/img/auth_4.png)

> 다음 명령어를 통해 Aceess Token을 넣어 User Profile 정보 접근한 결과 화면

![Auth_3](/asset/img/auth_6.png)

## `Test #8`
### 구글 로그인

