# OAuth 2.0

## 과거 로그인 처리 관행
* 서비스 가입 시 사용자로부터 정보를 입력 받습니다.
* 이를 직접 저장하고 관리합니다. <br>

    &rarr; 보안 사고가 터지면 사용자에게 손해배상을 해주어야 합니다.(?)

* 유저 입장에서 사이트 별도 가입과 비밀 번호 관리에 부담을 느낄 수 있습니다.

* 개발자 입장에서 개인 정보를 직접 처리하는게 부담됩니다.
    * 대한민국 개인정보보호법
    * EU의 GDPR (개인정보 보호법)
    * US의 Data privacy law

## OAuth (Open Authorization)
* 권한 대리 (access delegation)을 위한 개방형 표준
    * 사실상 현재 de facto, 업계 표준
* 사용자가 자신의 비밀번호를 노출하지 않으면서 제 3자(우리 앱)에게 자신의 정보 접근을 허용하는 방식
    * 보통은 platform 을 이용 하는 사용자가 platform 내 자신의 정보에 접근하고 싶어하는 제 3의 서비스 제공자(=우리 앱) 에게 권한을 부여하는데 사용함
* 통신 규약(프로토콜)이 아니라 `권한 대리를 위한 방식에 대한 표준`이므로 OAuth 구현 주체에 따라 세부적인 방식들이 다름

## OAuth의 목표
OAuth가 달성하려는 것은 `신원을 노출하지 않고 권한을 부여하는 것` 입니다. <br>
```
호텔에 체크인을 하고 카드 키를 받으면
카드 키는 그 방에 체류하는 사람의 정보는 노출하지 않은 채로
오로지 투숙 기간동안 그 방에 들어갈 수 있는 "권한"을 의미하는 것과 같습니다.
```

## 인증 &rarr; 인가 &rarr; 인가 코드
* `Authentication` (인증) : 사용자 id / password 확인 <br> 
    &rarr; 신원 체크 (권한 체크가 아님) <br>

* `Authorization` (인가)  : 사용자가 우리 앱에 (플랫 폼에 있는) 개인 정보를 사용 <br> 
    &rarr; 권한 부여

* `Authorization Code` : Authorization 단계 후 auth server가 우리 앱에 전달하는 중간 결과물
    &rarr; REST API에 바로 쓸 수 없고 반드시 `access token` 으로 교환해서 써야 합니다.

## OAuth 2.0 이해 당사자
* Resource owner : 개인 정보 주체. 즉, 사용자

* Resource server (REST API)
    * 사용자의 개인 정보를 가지고 있는 주체
    * 즉, Google, Facebook, 카카오, 네이버의 API 서버
    * 서비스의 제공자로 번역

* Authorization server (서비스 제공자)
    * 인증, 인가를 수행해주는 서버
    * Google, Facebook, 카카오, 네이버에서 운영하는 서버이지만 `역할에 따라 분리`해서 resource server와 구분함
    * API 서버와 동일한 경우도 있음

* Client (우리 앱)
    * 사용자의 개인 정보를 쓰려는 주체. 즉, 우리가 만드는 앱
    * (주의) 앱 프론트엔드 / 백엔드 구분할 필요가 없음 -> OAuth 관점에서는 다 Client

## OAuth 2.0 권한 부여 (Grant) 타입들
* Authorization code grant
* Refresh token
* PKCE
* Client credential
* Device code

## OAuth 2.0 주요 용어
* Access token
    * (사용자 정보에 접근하는) REST API를 호출할 수 있는 권한을 의미하는 토큰
    * 우리 앱 입장에서는 해석 불가 (= opaque)
    * REST API 에 실어서 보냄
    * REST API 서버는 access token 을 통해 `접근 가능한 정보 범위`와 `기간` 등을 해석함

* Refresh token
    * Access token 을 가지고 있으면 API 를 호출할 수 있는데 (= 호텔 방 키) <br>
      Access token 은 일정 시간 동안만 유효함 (= 호텔 투숙 기간) <br>
    * 기간이 만료되면 다시 새 access token 을 받아야 하는데, <br> 다시 authorization 을 수행하는 것이 번잡하므로 새 access token 을 받을 수 있는 토큰

## OAuth의 장점
* 보안성
    * 민감한 로그인 정보를 다루지 않아도 됩니다.
    * 접근 권한을 세분화 하고 기간 제한이 가능합니다.
* 편의성
    * 사용자는 서비스마다 별도로 개인 정보를 입력할 필요가 없습니다.
    * 타 서비스와 연동이 쉽습니다.

## RESTful API 호출 시 OAuth Token 전달
* REST 구조의 `stateless 원칙`에 의해 각 RESTful API 요청들은 요청 자체로 완전한 정보를 포함해야 됩니다. <br>
&rarr; 이 때문에 각 API 요청은 access token 을 HTTP header 에 포함합니다.
* HTTP header 에는 다음과 같은 key-value 를 포함시킵니다.
    * Key : Authorization
    * Value : Bearer <토큰 문자열>