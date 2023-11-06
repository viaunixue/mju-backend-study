# lab 7 - RESTful API

## `Test #1`
### Naver에 앱 등록하기
![naver_1](/asset/img/lab7/1.png)

## `Test #2`
### Naver에 앱 등록하기
```
$ curl "https://openapi.naver.com/v1/papago/n2mt" \
"-H "Content-Type: application/x-www-form-rulencoded; charset=UTF-8" \
"-H "X-Naver-Client-Id: {애플리케이션 등록 시 발급받은 클라이언트 아이디 값} \
"-H "X-Naver-Secret: {애플리케이션 등록 시 발급받은 클라이언트 시크릿 값} \
"source=en&target=zh-CN&text=Hello World" -v
```
![naver_2](/asset/img/lab7/2.png)


## `Test #3`
### Naver 파파고 번역
```
curl "https://openapi.naver.com/v1/captcha/nkey?code=0" \
-H "X-Naver-Client-Id: {애플리케이션 등록 시 발급받은 클라이언트 아이디 값}" \
-H "X-Naver-Client-Secret: {애플리케이션 등록 시 발급받은 클라이언트 시크릿 값}" -v
```
![naver_3](/asset/img/lab7/3.png)

## `Test #4`
### Naver 블로그 검색
```
curl  "https://openapi.naver.com/v1/search/blog.xml?query={I am 신뢰에요}&display=10&sort=date" \
-H "X-Naver-Client-Id: {애플리케이션 등록 시 발급받은 클라이언트 아이디 값}" \
-H "X-Naver-Client-Secret: {애플리케이션 등록 시 발급받은 클라이언트 시크릿 값}" -v
```
![naver_4](/asset/img/lab7/4.png)

## `Test #5`
### Kakao 앱 등록
![kakao_1](/asset/img/lab7/5.png)

## `Test #6`
### Daum 검색
```
curl -v -X GET "https://dapi.kakao.com/v2/search/web" \
--data-urlencode "query=I am 신뢰에요" \
-H "Authorization: KakaoAK {REST API KEY}"
```
![kakao_2](/asset/img/lab7/6.png)

## `Test #7`
### 네이버 로그인
> PORT 10000번 서버로 아래 링크의 Auth Code 받은 결과 화면
```
https://nid.naver.com/oauth2.0/authorize?response_type=code&client_id=bop3C73HErShvaWh5AT2&redirect_uri=http%3A%2F%2Flocalhost%3A10000&state=0115
```

![Auth_1](/asset/img/lab7/7.png)

> 아래 링크에 Auth Code를 넣고 Access token 얻는 결과 화면
```
https://nid.naver.com/oauth2.0/token?grant_type=authorization_code&client_id=bop3C73HErShvaWh5AT2&client_secret=6IdFnhihKH&code=9jgfquFfnEa51MGGMP&state=0115
```
![Auth_2](/asset/img/lab7/8.png)

> 다음 명령어를 통해 Aceess Token을 넣어 User Profile 정보 접근한 결과 화면

![Auth_3](/asset/img/lab7/9.png)

## `Test #8`
### 구글 로그인
> PORT 10000번 서버로 아래 링크로 Auth Code 받기
```
https://accounts.google.com/o/oauth2/v2/auth/oauthchooseaccount?client_id=414944130313-c5dl5thhv00f2jiref2rd7fva5ijgnkt.apps.googleusercontent.com&redirect_uri=http%3A%2F%2Flocalhost%3A10000&response_type=code&scope=https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email&service=lso&o2v=2&theme=glif&flowName=GeneralOAuthFlow
```
> 아래 링크에 Auth Code를 넣고 Access token 받기
```
$ curl -X POST https://oauth2.googleapis.com/token --data client_id=414944130313-c5dl5thhv00f2jiref2rd7fva5ijgnkt.apps.googleusercontent.com --data client_secret=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 --data code=4/0AfJohXniIR-Bbv870na19DA4ac9wxAk_8u19BlWkZA3G8zgMJuRWCR8Meu2Qhzcpoo95GA --data grant_type=authorization_code --data redirect_uri=http%3A%2F%2Flocalhost%3A10000
{
  "access_token": "ya29.a0AfB_byBpfQcZSyImyhwQaznIKxoMCd77cprVBro5SoPLIsLrPMQ7OAttBt8Z49fOCpxLKqpaQGFyhyPsc9ll5BhShMMQ6tqrULw2b7HZXO_Y_CpjXP39pPcm6BjM3Gx5yxNIlSBEi4KOy1sk7nm8zZk8JvlY6sXZ1yrQaCgYKAUwSARMSFQGOcNnCRM8Pnxax94desy4_rROFgw0171",
  "expires_in": 3599,
  "scope": "https://www.googleapis.com/auth/userinfo.email openid",
  "token_type": "Bearer",
  "id_token": "eyJhbGciOiJSUzI1NiIsImtpZCI6ImY1ZjRiZjQ2ZTUyYjMxZDliNjI0OWY3MzA5YWQwMzM4NDAwNjgwY2QiLCJ0eXAiOiJKV1QifQ.eyJpc3MiOiJodHRwczovL2FjY291bnRzLmdvb2dsZS5jb20iLCJhenAiOiI0MTQ5NDQxMzAzMTMtYzVkbDV0aGh2MDBmMmppcmVmMnJkN2Z2YTVpamdua3QuYXBwcy5nb29nbGV1c2VyY29udGVudC5jb20iLCJhdWQiOiI0MTQ5NDQxMzAzMTMtYzVkbDV0aGh2MDBmMmppcmVmMnJkN2Z2YTVpamdua3QuYXBwcy5nb29nbGV1c2VyY29udGVudC5jb20iLCJzdWIiOiIxMDQ4MzM4NTAyNTg1MDg5MDE3MzYiLCJlbWFpbCI6ImpqaDE5OTYwMTE1QGdtYWlsLmNvbSIsImVtYWlsX3ZlcmlmaWVkIjp0cnVlLCJhdF9oYXNoIjoiSmszNTd1QlNOd1hqZmVvLVRTeEh5ZyIsImlhdCI6MTY5OTI1MjM1NiwiZXhwIjoxNjk5MjU1OTU2fQ.ZxJpWBjqIGi40EywNasSn1xRjMg8bfSkdYfF5N88lgJ6kfIx4Lxe1LMulDL9oyL2JDMkqan2T1QzlCa4IDqnvqraaci9yAg5bz3A0L46egVxm9fMXFDdE_soIVlQY7o-uch_5xTDDjk-BHCApuTgudzRpjs1Bz7y23IFAT0h5sdqI1zwZ-e5S-62P5-XfM2iA3gopJEx-sthPL_JhWL9852HhV6pZLb5PtkHY8fU86kcjlfgSwgkhlCs--CpvYOe1UNxHwUKbQEFs66nUaKab7fxW-k18CxTfvLwvvX0jOclL9yE9fico5YRdUzSX04wdyNH6aCPwgX86K6e3CcE2g"
}%   
```

> Aceess Token을 이용해 다음 Google User Info에 접근한 결과 화면
```
curl -X GET https://www.googleapis.com/oauth2/v3/userinfo -H "Authorization: Bearer ya29.a0AfB_byBpfQcZSyImyhwQaznIKxoMCd77cprVBro5SoPLIsLrPMQ7OAttBt8Z49fOCpxLKqpaQGFyhyPsc9ll5BhShMMQ6tqrULw2b7HZXO_Y_CpjXP39pPcm6BjM3Gx5yxNIlSBEi4KOy1sk7nm8zZk8JvlY6sXZ1yrQaCgYKAUwSARMSFQGOcNnCRM8Pnxax94desy4_rROFgw0171" 
```

![Auth_4](/asset/img/lab7/10.png)
