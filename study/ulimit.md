## TCP 동시에 열 수 있는 descriptor 수 조정

* `ulimit` 명령어를 이용하면 동시에 열 수 있는 숫자를 조정 가능합니다.
    * 제약을 더 줄이는 것은 모든 유저가 가능합니다.
    * 제약을 늘리는 것은 root 권한이 필요합니다.

* **다음은 제약을 줄인 후 / 늘린 후의 실행 결과입니다**
![ulimit2](/asset/img/ulimit2.png)
<br>

* 해당 명령어는 현재 열려있는 shell 에서만 적용됩니다.  <br>
즉, 새로 터미널을 열면 다음과 같이 영향을 받지 않습니다. <br>
(VS Code의 remote에서 terminal을 열 경우 숫자가 상향된 shell이 실행됩니다.)
![ulimit3](/asset/img/ulimit3.png)
![ulimit4](/asset/img/ulimit4.png)