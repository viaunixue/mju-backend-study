# AWS의 ELB 통한 Memo Service 배포

## 실행 환경 구성 제출물

* 실습용 AWS 에 “mjubackend2023” 이라는 AMI 를 만들어 뒀음
    * 이 이미지는 Nginx + uWSGI 가 설치되어있고, <br>
    uWSGI 는 ~ubuntu/mjubackend 아래 memo.py 를 로드함 <br>
    **URL 경로는 http://.../memo 로 접근 가능**

1. 본인이 개발한 미니프로젝트를 이 디렉터리 위에 올리고 <br>
    memo.py 외에 templates/ 와 static/ 역시 올려야 함
2. 인스턴스 2개짜리 대상 그룹을 만들고
3. 이 대상 그룹을 사용하는 로드 밸런서를 설정해서
4. 브라우저에서 본인 http://내LB이름/memo 를 입력해서 접근 가능 해야 함

## 실행 환경 구성 시 주의 점
* 아래 사항을 지키지 않을 경우 감점임
    * 비용 문제로 인스턴스 타입은 t2.micro 를 이용할 것
    * 인스턴스 생성시 반드시 “본인학번-1”, “본인학번-2” 형태로 이름을 부여할 것
    * 대상 그룹 생성시 반드시 “본인학번-group” 형태로 이름을 부여할 것
    * 로드 밸런서 생성시 반드시 “본인학번-lb” 형태로 이름을 부여할 것
    * 인스턴스 2개, 대상 그룹 1개, 로드 밸런서 1개 이상을 만들어서 방치하는 경우
* 아래 사항을 위반하면 이번 과제 0점 처리됨
    * mjubackend2023 AMI 를 삭제하는 행위
    * 다른 학생의 인스턴스, 대상그룹, 로드 밸런서를 삭제하거나 조작하는 행위

<br>

```
[수정 사항]
가상 서버를 3개 사용하는 것으로 하고 제3의 가상 서버는 DB 목적의 docker container 를 실행하는 서버로 해주세요.
가상 서버 인스턴스트를 "학번-1", "학번-2" 로 한 것처럼 그 가상 서버는 "학번-DB" 로 해주시기 바랍니다.
"학번-1", "학번-2" 가상 서버의 Flask app 은 이 "학번-DB" 의 IP 주소를 사용하게끔 Flask app 을 만들어주세요.
"학번-DB" 가상 서버는 Elastic IP 를 부여해도 되고, private IP 로 통신이 가능하면 private IP 를 사용해도 됩니다.

이런 목적으로 security group 중에 "db" 라는 것을 만들어뒀으니 "학번-DB" 서버는 이 db 보안 그룹으로 설정해주세요. 
Redis, MongoDB, MySQL 포트가 열려 있습니다.

이렇게 설정하면 로드 밸런서에 의해 "학번-1" 로 접속을 해서 가입을 했든
"학번-2" 로 접속을 해서 가입을 했든 언제나 동일하게 서비스를 사용할 수 있을 겁니다.
```

## 구현 요구 조건

* [X] 네이버 OAuth 가 제대로 구현
* [X] `GET /memo` 가 제대로 구현
* [X] `POST /memo` 가 제대로 구현
* [X] 유저를 바꿔서 사용하는 경우 사용자 구분
* [X] 개발 서버 2개, DB 서버 1개로 가상 서버 총 3개 생성
* [X] 로드 밸런싱 인스턴스 대상 그룹 생성 후 개발 서버 대상 등록
* [X] 로드 밸런싱 로드밸런서 생성
* [X] 네이버 애플리케이션 API 설정

### 기본 아키텍처 구상도

![memo_proj_13](/asset/img/memo_proj_14.png)

### 개발 서버 2개, DB 서버 1개로 가상 서버 총 3개 생성

![memo_proj_1](/asset/img/memo_proj_1.png)

```
개발 서버, DB의 퍼블릭 IPv4 IP 주소는 각각 다음과 같습니다.
60162176-1 : 52.79.93.254
60162176-2 : 43.202.51.102
60162176-DB : 3.36.50.200
```

### 로드 밸런싱 인스턴스 대상 그룹 생성 후 개발 서버 대상 등록

![memo_proj_2](/asset/img/memo_proj_2.png)

### 로드 밸런싱 로드밸런서 생성

![memo_proj_3](/asset/img/memo_proj_3.png)

### 네이버 로그인 API 설정

![memo_proj_4](/asset/img/memo_proj_4.png)

```
서비스 URL은 로드 밸런서 DNS 이름과 prefix URL인 /memo를 붙여 다음과 같습니다.
http://60162176-lb-446869379.ap-northeast-2.elb.amazonaws.com/memo

Callback URL은 개발 서버 2개의 IP 주소와 prefix URL 인 /memo 에 
auth 함수를 호출하기 위한 URL인 /memo/auth를 작성해주었습니다.
http://52.79.93.254:8000/memo/memo/auth
http://43.202.51.102:8000/memo/memo/auth
```

### 개발 서버 내 환경 구축

생성한 2개의 개발 서버 안에서 memo.py가 올바르게 작동할 수 있도록 아래의 파일들을 동일하게 구축해 주었습니다.
```
/static, /templates, memo.py
```

### DB 서버 환경 구축

서버에 접속한 후 다음의 docker 명령어를 통해 redis를 실행해 주었습니다.
```
$ ssh ubuntu@3.36.50.200
$ sudo docker run -d -p 6379:6379 redis:latest
```
<br>

![memo_proj_5](/asset/img/memo_proj_5.png)

아래 명령어를 통해 해당 redis 이미지가 올바르게 등록되어 있음을 확인했습니다.
```
$ sudo docker images
$ sudo docker ps -a
$ redis-cli -p 6379:6379 monitor
```

### 개발 서버 및 DB 테스트

![memo_proj_9](/asset/img/memo_proj_9.png)

![memo_proj_10](/asset/img/memo_proj_10.png)

![memo_proj_6](/asset/img/memo_proj_6.png)

![memo_proj_7](/asset/img/memo_proj_7.png)

![memo_proj_8](/asset/img/memo_proj_8.png)

```
개발 서버 2개 모두 해당 서비스가 올바르게 동작하고 DB에 잘 저장되는 것을 모니터링 할 수 있습니다.
```

### 로드 밸런서 테스트

![memo_proj_11](/asset/img/memo_proj_11.png)

![memo_proj_7](/asset/img/memo_proj_7.png)

```
네이버 로그인 버튼을 누르면 로드밸런서를 통해 개발 서버 2개 중 하나를 임의로 배정하여 진입하는 것을 확인할 수 있습니다.
```