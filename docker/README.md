## Docker image 빌드

### Dockerfile 파일 작성 및 명령 실행

![docker_1](/asset/img/lab9/1.png)

```
$ docker build -t mju:1
```

* `docker build` 는 새 docker image 를 만드는 명령어입니다.
* Dockerfile 에 있는 `FROM` 명령어는 존재하는 image 기반으로 image를 생성합니다.

* http 라는 이미지는 docker 에서 공식적으로 배포하는 기본 이미지로 apache 가 설치되어 있습니다.

* `docker build` 명령어의 `-t 옵션`은 **새로 만드는 이미지에 이름을 부여하는 것**입니다.

* `docker build` 명령어 마지막에 입력한 "." 은 현재 디렉터리에 있는 Dockerfile 을 이용하라는 의미입니다.

* `COPY` 명령어를 Dockerfile 에 **html 폴더를 이미지 안에 복사하게끔 추가합니다.**

## 현재 Docker Image 목록 보기

![docker_2](/asset/img/lab9/2.png)

## Docker 컨테이너

* `Docker Image`와 `Container` 관계는 실행 파일과 프로세스 관계임

* 즉, docker image 는 아직 실행되지 않은 상태이고, <br>
    docker container 가 실제 실행 중인 상태입니다. <br>
    &rarr; 실행 파일 하나로 여러 프로세스를 실행하는 것처럼, 하나의 docker image로 여러 컨테이너를 만들 수 있습니다.

```
컨테이너 집을 옮겨다 놓으면 집이 되는 것처럼 Docker Container 역시 실행하면 그 안의 프로그램들을 바로 활용할 수 있습니다.
컨테이너 집에 전기, 상하수도를 연결해서 동작하게 하듯 Docker Container 역시 몇가지만 호스트와 연결해주면 됩니다.
```

## 컨테이너 띄울 때 연결되는 것들
* 네트워크
    ex) 포트번호 &rarr; `-p 옵션`

* 볼륨 (= 폴더)
    ex) 컨테이너 안의 프로세스가 읽고 쓸 디렉터리 &rarr; `-v 옵션`

## Docker 컨테이너 띄우기 

### docker run 실행 예시
* 실행파일을 실행하면 프로세스가 뜨고 PID 가 부여되듯, container 가 뜨고 container ID를 출력해줍니다.

![docker_3](/asset/img/lab9/3.png)

```
$ docker run -d -p 8080:80 mju:2
```

* `옵션 -d` 는 그라운드로 돌리고 바로 반환하라는 의미입니다. ( = detach ) <br>
&rarr; 이 옵션이 없으면 docker 컨테이너가 종료될 때까지 기다림

* `옵션 -p` 는 호스트의 8080 포트 &rarr; 컨테이너의 80 포트로 맵핑합니다. <br>
&rarr; 현재 기계의 8080 포트로 들어오는 트래픽이 컨테이너의 80번 포트로 들어감

* 마지막 인자는 실행할 이미지 이름과 tag 입니다.

### 실행 중인 Docker Container 보기

![docker_4](/asset/img/lab9/4.png)

```
$ docker ps -a 명령을 통해 실행 중인 container 목록을 볼 수 있습니다.
```

### 실행 중인 Docker Conatainer 접속

![docker_5](/asset/img/lab9/5.png)

### Docker Conatainer 정지 및 삭제

![docker_6](/asset/img/lab9/6.png)

```
$ docker stop <container id>
$ docker rm <container id>
```

* 정지된 container는 혹시 container 출력 결과가 필요할지도 모르게 때문에 바로 지워지지 않고 남아있게 됩니다.

## 볼륨 맵핑하기

![docker_7](/asset/img/lab9/7.png)

> 대부분 log 를 쓸 때 이 방식을 사용합니다.

## Redis 를 Docker 로 띄워보기

![docker_8](/asset/img/lab9/8.png)

> 참고 : redis image가 존재하지 않으므로 docker hub 에서 자동 다운로드 합니다.

<br>

![docker_9](/asset/img/lab9/9.png)
 ![docker_10](/asset/img/lab9/10.png)

> 다운로드 받은 redis image를 실행하여 16379 포트가 포워딩 됨을 알 수 있습니다.

## macOS 에서의 Linux Docker Image

* Docker 는 OS 가상화를 하는 것이 아니므로 OS를 설치하는 것이 아니고, <br>
**같은 OS 상에서 process를 띄우는 방식으로 container를 띄웁니다.** <br>

    &rarr; 그리고 여기서 process id, user id, port 등 충돌 날 수 있는 것들을 namespace 라고 칭하고 이들을 가상화합니다.

* Window 에서는 WSL 이라는 Linux 커널을 subsystem으로 내장하는 방식으로 Linux 프로그램을 실행할 수 있습니다.

#### macOS 에서는 어떻게 docker container 가 실행되는 것일까?

 &rarr; macOS 에서는 Linux VM 을 띄우고 그 안에서 docker container 가 도는 방식입니다.