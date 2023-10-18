## 에러 관련 함수 및 주요 에러 코드

### 에러 관련 함수들
* 많은 Linux 함수들은 오류 발생 시 `-1`을 반환합니다. <br> `errno`라는 환경 변수에 에러 코드를 저장합니다.<br>
이 에러 코드를 문자화 하는 함수들을 제공합니다.
<br>

* 전역 변수 `errno`
    * `#include <errno.h>` 필요
    * `thread-safe` 합니다. (즉, 멀티 쓰레드 환경에서도 정상 작동)
* 에러 코드 문자화 함수 `strerror()`
    * `#include <string.h>` 필요
    * 매개 변수로 주어지는 에러 코드에 대응되는 문자열을 반환합니다.
* 에러 코드 출력 함수 `perror()`
    * `#include <stdio.h>` 필요
    * 표준 출력으로 `errno`에 대응되는 에러 문자열을 출력합니다.

### send() 함수 관련 주요 에러 코드
**`send()` 가 반환할 수 있는 에러 코드는 `man send` 혹은 `man 2 send`를 통해 확인이 가능합니다**

* `EBADF` - Socket descriptor 가 아닐 때
* `EAGAIN` or `EWOULDBLOCK` - 보낼 데이터를 저장하기 위한 kernel 공간이 없을 때 (`Non-blocking socket`인 경우 발생)
* `ECONNRESET` or `EPIPE` - 소켓이 연결 안된 상태 (애당초 연결이 안되었거나, 상대가 연결을 끊었거나)