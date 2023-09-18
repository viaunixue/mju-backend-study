# lab 2 - AWS Practice

## `제출물 #1` 
- **VM의 public IP, private IP** <br>
![submit1.0](/asset/img/제출물1_IP.png)

- **VM의 inbound, outbound 규칙** <br>
![submit1.1](/asset/img/제출물1_Security.png)

    `SSH 키 등록` &rarr; `VM 생성` &rarr; `생성된 VM 확인`
    ```
    Public IPv4 address : 3.39.240.137
    Private IPv4 address : 172.31.2.124
    Public IPv4 DNS : ec2-3-39-240-137.ap-northeast-2.compute.amazonaws.com
    Instance Type : t2.micro
    ```

## `제출물 #2` 
- **서버를 껐다가 켠 이후 VM의 public IP, private IP** <br>
![submit2](/asset/img/제출물2_resetIP.png)

    `서버를 끄고 다시 구동시키면 Public 주소가` 3.39.240.137 &rarr; 3.38.192.138 `로 변하는 것을 확인할 수 있습니다.` <br>
    `Private IP는 서버를 껐다 켜도` 172.31.2.124 `그대로 유지하는 것을 확인할 수 있습니다.`
    ```
    Public IPv4 address : 3.38.192.138
    Private IPv4 address : 172.31.2.124
    Public IPv5 DNS : ec2-3-38-192-138.ap-northeast-2.compute.amazonaws.com
    Instance Type : t2.micro
    ```

## `제출물 #3`
- **Elastic IP를 설정한 후 public IP 정보** <br>
![submit3](/asset/img/제출물3_ElasticIP.png)
    `Elastic IP가 연결되게 되면, NAT의 Public IP 와 DNS 정보만 변경됩니다.`<br>
    **`서버 on/off 에 따라 동적 할당 받던 IP 주소를 고정 IP로 설정`**
    ```
    Elastic IP : 43.202.94.141 
    Private IPv4 address : 172.31.2.124
    Public IPv5 DNS : ec2-43-202-94-141.ap-northeast-2.compute.amazonaws.com
    Instance Type : t2.micro
    ```


## `제출물 #4` 
- **cat 명령어를 통해 CPU 개수 2개라는 정보**  (`t2.micro` &rarr; `t3.micro`)
![submit4](/asset/img/제출물4_t3CPU.png)
**`인스턴스 타입 변경 해보기`** <br>
`t2.micro` 일 때는 `processor`가 값 `0`으로 CPU 하나의 정보들만 있었지만, <br>
`t3.micro` 로 변경하여 서버를 시작하면 `processor` 값이 `0`, `1` 두개의 정보로 CPU 개수가 변경됨을 알 수 있었습니다.

### `제출물 #5` 
- **URL 본인 VM의 public IP** (`Elastic Public IP 통한 접속`) <br>
![submit5](/asset/img/제출물5_80port접속.png) <br>
 **`보안 그룹 적용하기`** <br>
 `HTTP port로 규약한` 80 `을 인바운드로 설정하여 첫번째 VM의 보안 그룹에 추가하였습니다.` <br>
 `VM 내부에 주는 영향 없이 방화벽에서 변경됩니다.`  
    ```
    학교 네트워크에 방화벽 차단으로 인해 DNS 를 통한 접속은 되었지만 Elastic IP 를 통한 접속이 되지 않았습니다. 
    다중 노드를 통한 접속이 특징인 Tor 웹 브라우저를 설치하여 다음과 같이 웹에 서버를 성공적으로 올릴 수 있었습니다. 
    ```

### `제출물 #6`
- **두번째 VM 접속 후 다음 명령어의 결과 값**
    > **$ ip a** 

    ![submit6.1](/asset/img/제출물6_ip_a.png)
<br><br>

- **AWS 콘솔 두번째 VM 정보 창** (`Private IP`) 
![submit6.2](/asset/img/제출물6_VM2.png)

    `서브넷 생성` &rarr; `서브넷을 지정한 VM 생성` &rarr; `가상 네트워크 카드 만들기` &rarr; `두번째 서버로 SSH 로그인`<br>
    `첫번째 가상 서버가 두번째 네트워크에도 속하게 하여 첫번째 가상 서버를 경유하는 방법으로 구현하였습니다.`<br>
    `두번째 VM 에 접속한 후 위의 명령어를 통해` **lo** `라 불리는 Loopback Interface 정보와` **eth0** `라 불리는 interface 정보를 확인할 수 있었습니다.` <br>
    eth0 `interface 를 통해 두번째 VM의 Pricate IP인` 172.31.130.95 `를 확인할 수 있습니다.`<br>

    ```
    해당 서브넷 서버 연결 과정에서 다양한 시행 착오가 있었습니다. 
    네트워크 인터페이스를 연결한 후에 첫번째 VM에도 SSH 접속이 되지 않는 문제가 발생했습니다.
    id_rsa 파일 권한도 변경해보고, instance를 종료하고 새로 만들어 보고, 보안 그룹도 다시 작성해보는 등 여러 시도를 해보았습니다.
    결국 첫번째 VM에 새로 연결한 네트워크 인터페이스를 분리하고 첫번째 VM을 구동한 후,
    eth1.sh 파일을 새롭게 설정하고 잠시 중지한 후 제거 하였던 인터페이스를 다시 추가하여 구동하니 문제없이 VM 에 접속이 되었습니다. 
    이와 같은 해결 과정을 통해 네트워크 인터페이스 접속 과정에 대한 깊은 이해를 할 수 있었습니다
    ```

## `제출물 #7`
- **생성한 스냅샷의 정보**
![submit7](/asset/img/제출물7_스냅샷.png)

    `VM의 디스크가 파일임을 확인` &rarr; `VM 스냅샵` &rarr; `생성한 리소스 삭제` 
   
### `생성한 리소스 삭제`
- ~~EC2 의 “인스턴스” 에서 자신이 만든 가상 서버들 “종료”~~
- ~~EC2 의 “네트워크 인터페이스” 에서 자신이 만든 네트워크 카드 “삭제”~~
- ~~EC2 의 “탄력적 IP” 에서 본인이 할당 받은 public IP 주소 “릴리즈”~~
- ~~EC2 의 “보안그룹” 에서 본인이 만든 그룹 “삭제”~~
- ~~EC2 의 “스냅샷”에서 본인이 만든 스냅샷 “삭제”~~
- ~~VPC 의 “서브넷”에서 본인이 만든 서브넷 “삭제”~~
```
AWS 비용 폭탄을 피하려면 사용하지 않는 서버는 위와 같이 꼭 전부 삭제 해주어야 합니다.
```

<br>
<details>
<summary>최종 산출물</summary>

![final1](/asset/img/final_instance_detail.png)
<br>

![final2](/asset/img/final_instance_security.png)
<br>

![final3](/asset/img/final_network_interface.png)

</details>