# Spring Boot 

## IoC (Inversion of Control; 제어의 역전)

* 일반적인 contorl flow는
&rarr; 내 코드가 library를 부르고 어떤 클래스를 생성하고 소멸합니다.

* IoC 는 Framework가 내 코드를 부르고 객체를 만들고 소멸시킵니다.

#### Spring 에서의 IoC
* 설정 파일, Code, Annotation 등 필요한 객체 정보를 파악합니다.
* 각 객체들의 의존성을 파악합니다.
* Spring Container 가 객체들의 수명을 관리합니다. &rarr; 이 객체들을 `bean` 이라고 합니다. 

## DI (Dependency Injection; 의존성 주입)
* 특정 개체가 필요한 곳에 객체를 넣어줍니다.

    > 아래 코드에서 우리는 B의 객체를 만들지 않았습니다. (즉 **B b = new B() 이런게 없음**) <br>
    즉, 내가 객체를 만든 것이 아니라 프레임워크가 만들어줬습니다. (= Inversion of Control) <br>
    이제 내가 객체가 필요하다고 하면 (= 내가 그 객체에 의존적이라면) 그 객체를 자동으로 주입해줄 것입니다.

    ```java
    public class A {
        @Autowired
        B b;
    }
    ```
    <details>
    <summary> 어디에 어떤 객체가 필요하다는 것을 알까? </summary>
    애노테이션으로 태깅합니다. (예: `@Autowire`)
    </details> <br>

    ```
    애노테이션은 일종의 decorator 패턴입니다.
    어떤 decorator 패턴은 원래 있던 기능을 확장해주는 것입니다.
    ```

## Spring Container

![boot_1](/asset/img/boot_1.png)

Bean의 생명 주기를 관리하고 `@Autowire` 애노테이션을 사용해 bean 주입하는 기능을 지원합니다.

## Bean

* Spring Container가 생명 주기를 관리하는 객체

* Spring에서 제공하는 클래스 타입만 되는 것이 아니라, 우리가 작성하는 클래스 역시 Bean이 될 수 있음 (많은 경우 그렇게 해야 함) <br>
&rarr; 즉, 우리가 많든 클래스 객체를 `new MyClass(())`처럼 생성하게 하는 것이 아니라, Spring Containe이 알아서 관리해야 함

* **기본적으로 `Singleton` 입니다** (즉, `클래스당 객체 하나만 존재함`) <br>

    &rarr; 고려해야 할 문제 : thread-safe 한가? (nulti-thread 상에서 이 객체는 상호 베제되는가?) <br>
    &rarr; 만일 여러 객체를 만들어야 한다면 proto-type 패턴을 이용해 복제에 써야 합니다.

    ```
    전체 프로그램 안에서 하나가 공유되는 것입니다.
    멀티 스레드로 만들었다고 가정할 때 thread-safe 하게 못만들 것 같다고 한다면
    껍데기만 bean으로 만들고 내부만 safe한 구조로 만듭니다.
    보통 bean으로 되어있다면 thread-safe 하다고 생각하면 됩니다.
    예를 들어 redis connection pool을 만든다면 하나만 존재하면 싱글톤이지만
    pool을 만든다면 proto-type으로 복제, 복제, 복제 하면서 만드는 것입니다.
    ```

## @Component : 클래스를 Bean으로 만들기

* **`클래스`를 Bean으로 만들기 위해서** `@Component` 애노테이션을 사용해야 합니다.
    ```java
    @Component
    public class B {}
    ```
* 또는 `@Component` 포함하고 있는 애노테이션을 사용합니다.
    ```java
    예시: @SpringBootApplication, @Controller, @RestController, @Service, @Configuration
    ```

    <details>
    <summary> 스프링은 리플렉션을 사용합니다 </summary>
    리플랙션은 내가 무언가를 성찰하고 돌아보는 것을 의미합니다. <br>
    자기 자신에게 무엇이 있는지 찾아보는 것을 뜻하는 것입니다. <br>
    `이 객체에 이런게 있니 없니~ 한번 살펴봐줘` 살펴보는게 리플랜션입니다. <br><br>
    리플랙션 쓰는 또 하나는 구글의 `Protocol buffer`입니다.
    무슨 멤버가 존재하는지 확인하고 있다면 무조건 그 멤버를 위해 공간을 할당하고 그 값이 null 인지 아닌지 세팅합니다. <br>
    근데 Protocol buffer은 용량을 더 줄이기 위해 이 멤버가 있는지 없는지 조차도 리플랙션으로 동작합니다.
    </details>

## @RestController 가 하는 일

* @Controller + @ResponseBody 

* 이것으로 태깅된 클래스 안의 메서드에 핸들러를 맵핑할 수 있습니다.

    > GET    → @GetMapping 으로 태깅된 메서드 <br>

    > POST   → @PostMapping 으로 태깅된 메서드 <br>

    > PUT    → @PutMapping 으로 태깅된 메서드 <br>

    > DELETE → @DeleteMapping 으로 태깅된 메서드 <br>

* 해당 핸들러의 반환 값을 JSON 문자열로 HTTP Response body 에 포함됩니다.

    > 예: 1, “result”, {“name”: “DK”, “id”: 1234567}, [“aaa”, “bbb”, “ddd”]

* @RestController 로 태깅된 클래스가 여럿 존재할 수 있다.

## HTTP Request/Response 를 직접 다뤄야 할 때

* 경우에 따라 request 내용을 알아야 하거나 (예: POST 인 경우 request body) <br>
response 를 직접 통제해야될 수도 있음 (예: HTTP status code 지정)

* 이때는 각 handler 에 `HttpServletRequest`, `HttpServletResponse` 객체를 추가 매개 변수로 지정하면 됨

    ```java
    @PostMapping("/article")
    public void postArticle(HttpServletRequest request, HttpServletResponse response) throws IOException {
        System.out.println(request.getMethod());

        // request body 부분을 읽음
        BufferedReader reader = new BufferedReader(
            new InputStreamReader(request.getInputStream(), StandardCharsets.UTF_8));
        String payload = in.lines().collect(Collectors.joining());
        System.out.println(payload);

        response.addHeader("MY_HEADER", "MY_VALUE");
        response.getOutputStream().println("Hello");
    }
    ```
    &rarr; request의 body 부분을 읽기 위해서 이런 과정을 거치는건 비효율적입니다. 

## @RequestBody

RESTful API에서 POST나 PUT은 대개 `request body`에 JSON 문자열을 담고 있습니다. <br>

> 예: POST /article 은 {"title":"요리", "author": {"name":"DK"}} 같은 내용을 포함합니다.

앞에서 보듯 `request` 객체를 통해 body 내용을 `String`으로 읽고 이를 다시 `JSON` 객체로 만드는 것은 복잡합니다. <br>

그리고 `JSON` 객체를 만든 후에 문자열인 key 를 통해 값을 얻어내는 것은 key 에 오타를 내서 실수하기도 좋고 코드도 읽기 어렵습니다.

> 예: Pseudo code로 써봐도 System.out.println(parsedJson["title"]) 이런 식이 됩니다. <br>
> 더욱이 value가 다시 JSON 객체라면? (parsedJson["author"]["name"]) <br>

이 경우 JSON 객체가 아니라 우리가 다루는 데이터 타입을 반영하는 커스텀 클래스 객체면 더 좋을 것입니다. <br>

> 예: System.out.println(parsedMyObject.author.name) 

## Spring MVC (Model-View-Controller)

`@RestController` 는 `@Controller`를 상속받고 `@ResponseBody` 기능을 제공합니다. <br>

`@ResponseBody`는 핸들러 반환 값을 HTTP Response의 body 로 쓰라는 의미입니다. <br>
즉, 객체를 반환해도 이를 JSON 문자열로 serialize하여 response body로 사용합니다. <br>

@Controller 는 MVC에서의 Controller를 의미합니다.
```
Controller -> HTTP 요청을 받는 end point. (handler라고 생각하면 됩니다)
View       -> 클라이언트에 보여질 리소스입니다.
Model      -> Logic 처리 결과. View를 커스터마이징 하는데 쓰입니다.
```

![springboot_29](/asset/img/springboot_29.png)

### Template Engine

Thymeleaf, Mustache, Freemarker, Grooy, JSP ...

## 계층형 아키텍처로서의 Spring

* MVC 는 UI 관련 패턴이다. <br>
    > 따라서 “Spring MVC framework” 은 Spring 이 제공하는 UI framework 이다.

* 이 때 Model 은 앞에서 살펴본 것처럼 View 를 rendering(= 그리기) 하기 위해 어디선가 <br> 채워줘야 하고, 보통 business logic (혹은 줄여서 logic) 이 이를 담당한다.

* 아주 단순한 logic 은 Controller 가 처리할 수도 있지만, <br>
많은 경우 복잡한 logic 은 별도 component 가 담당하는 것이 바람직할 것이다. <br>

* 또한 View 를 쓰지 않고 RESTful API 목적으로 쓴다고 하더라도
API 가 반환하는 결과 값 역시 logic 에 의해 계산된 것이어야 한다.

* Spring 은 이를 계층형 아키텍처로 해결한다.

    ![springboot_36](/asset/img/springboot_36.png)

## Lombok

### 보일러 플레이트 (boilerplate) 코드를 줄이기 위한 Java 라이브러리

* 편리한 애노테이션을 제공함
    * `@Getter`                  → Getter 자동 생성
    * `@Setter`                  → Setter 자동 생성

    <br>

    * `@NoArgsConstructor`       → 기본생성자(=인자 없음) 자동 생성
    * `@RequiredArgsConstructor` → final 이나 @NonNull 로 표시된 멤버들 입력받는 생성자 자동생성
    * `@AllArgsConstructor`      → 모든 멤버를 입력 받는 생성자 자동 생성

    <br>

    * `@ToString`                → toString() 함수 자동생성
    * `@EqualsAndHashCode`       → equals() 와 hashCode() 함수 자동 생성
    
    <br>

    * `@Data` (= @`Getter` + `@Setter` + `@RequiredArgsConstructor` + `@ToString` + `@EqualsAndHashCode`)

### Constructor 관련 Lombok 에노테이션

```java
public class User {
   private Long id;

   @NonNull
   private String username;

   private final String password;
}
```

```
@NoArgsConstructor       → User()
@RequiredArgsConstructor → User(username, password)
@AllArgsConstructor      → User(id, username, password) 
```

> class 정의 앞에 사용하며, 여러 개를 겹쳐서 쓸 수 있음

## Logging

* 상용 프로그램에서는 표준출력에 찍는 함수 (예: System.out.println()) 대신
`logging 모듈`을 사용하는 것이 바람직합니다.

* 표준 출력 함수는 ①redirect 가 없다면 화면, ②redirect 가 있다면 고정된 파일 사용 <br>
    &rarr; 남기고 싶은 로그를 선택할 수 없음 <br>
    &rarr; 로그 파일의 크기가 한정 없이 커져도 방법이 없음 <br>

* Logging 모듈을 사용하면 어떤 로그가 출력될지, 어디로 출력될지에 대한 통제가 가능함 <br>
    &rarr; 무시, 파일로 출력, 화면으로 출력, 네트워크로 전송, … <br>
    &rarr; 파일로 출력시 파일 이름이 일정 크기 이상이 되면 새 파일로 작성 (= log rotation 이라고 함)

    > 예: mylog.txt 가 10MB 가 되면 이 파일을 mylog.20231206.txt 로 바꾸고 새 mylog.txt 사용

### Logging과 일반 표준 출력 함수 비교 예시

![springboot_40](/asset/img/springboot_40.png)

## 배포

### JAR 생성 - IDE 이용

![deploy_1](/asset/img/deploy_1.png)

![deploy_2](/asset/img/deploy_2.png)

```
build/libs/ 아래에 다음과 같이 jar 생성됨을 확인할 수 있습니다.
```

### JAR 생성 - CLI 이용

![springboot_45](/asset/img/springboot_45.png)

```
CLI 에 gradle 또는 gradlew 라는 스크립트를 확인할 수 있습니다.
(w 는 wrapper를 의미합니다)
```

&rarr; 이 스크립트를 직접 실행하여 IDE 메뉴를 이용하는 것과 같은 효과를 낼 수 있습니다. <br>

> CI(Continueous Integration) 에서 빌드 스크립트 작성할 때 이 방식을 사용합니다.

<br>

```
$ ./gradlew bootJar
```

### JAR 생성 - 버전 번호

> jar 를 생성하면 mjublog-1.0-SNAPSHOT.jar 와 같은 이름으로 생성됩니다. <br>
> 이 때 `mjublog` 는 base name, 1.0-SNAPSHOT 은 version 입니다. <br>

```
이 둘은 build.gradle 에 bootJar 라는 섹션을 만들어서 수정할 수 있습니다.
```

```java
...
group = 'org.mjubackend'
version = '1.0-SNAPSHOT'

bootJar {
    archiveBaseName = 'blog'
    archiveVersion = '1.3'
}
...
```

&rarr; 그런데 bootJar.archiveVersion 은 생략할 경우 최상위 version 이 사용됩니다. 그리고 version 문자열이 두개 있으면 이상합니다.

```java
...
group = 'org.mjubackend'
version = '1.3'

bootJar {
    archiveBaseName = 'blog'
}
...
```

&rarr; bootJar 섹션의 내용이 하나뿐이라면 bootJar.archiveBaseName 처럼 한 줄로 쓰는 것이 나을 수 있습니다.

```java
...
group = 'org.mjubackend'
version = '1.3'

bootJar.archiveBaseName = 'blog'
...
```

&rarr; 그런데 archive 이름을 변경하는 것은 자주 있는 일은 아닙니다.
```
대개는 CI 에서 빌드 때 마다 version 만 치환해서 jar 를 생성합니다.
```
```java
...
group = 'org.mjubackend'
version = '1.3'
...
```

#### Spring Boot 버전 번호 컨벤션

Spring Boot 의 버전 번호 뒤에 다음과 같은 글자들이 올 수 있습니다.
```
SNAPSHOT → 보통 daily build 결과물
M        → 마일스톤. 뒤에 숫자를 붙이며, M1 은 첫번째 마일스톤 결과물이란 뜻
RC       → Release Candidate. 정식 릴리즈 직전 버전. 숫자를 붙여 RC2 처럼 씀
GA       → General Availability. 또는 RELEASE 라는 표기하기도 함. 정식 릴리즈
```

* 우리가 개발하는 프로젝트에 이미 버전 번호 컨벤션이 있다면 그걸 따릅니다.

    &rarr; 기존 프로젝트에 투입된다면 뭐든 기존 컨벤션을 따른다 (코딩 컨벤션, 빌드 컨벤션, …)

* 만일 그런 것이 없다면 위의 표현들을 이용해 보는 것도 좋을 것입니다. 

#### JAR 파일의 구조 확인하기 

* jar 는 Java ARchive 의 약자다.
* tar 파일이 tar 라는 프로그램으로 압축이 풀리듯 jar 파일은 jar 라는 프로그램으로 압축을 풀 수 있다.

    ```
    파일 내용 보기→ $ jar tvf mjublog-1.0-SNAPSHOT.jar
    파일 압축 풀기→ $ jar xvf mjublog-1.0-SNAPSHOT.jar
    ```

    > 주의: 압축을 푸는 것은 내용 분석을 위해서 하는 것이지, <br>
        내용을 수정해서 다시 압축 후 배포하려는 목적이 아닙니다. <br>
        이런 식으로 수정 버전을 배포하는 것은 버전 관리가 되지 않으므로 지양합니다. 

### 배포 : JAR 실행

* jar 는 Java class 외에 resources/ 아래 있는 파일들도 모두 포함합니다.
 
    > 예: templates/, static/, application.yaml, …

* 따라서 jar 결과물만 복사함으로써 서버를 실행할 수 있습니다.

* 생성된 jar 는 다음처럼 실행합니다.
    ```
    $ java -jar mjublog-1.0-SNAPSHOT.jar
    ```

### 배포 : JAR 실행 - application.yaml 오버라이드

* jar 는 필요한 모든 파일을 포함하고 있으므로,
  이상적으로는 아무것도 손대지 않고 jar 만 실행하면 되게끔 배포하는 것이 좋습니다.

* 그러나 경우에 따라 application.yaml (혹은 application.properties) 를 수정 후 실행해야 될 수도 있습니다.

* 이런 경우 jar의 압축을 풀고 수정 후 다시 압축을 해서는 절대 안됩니다!

* 대신 --spring.config.location 옵션을 활용합니다.
    ```
    $ java -jar myjar.jar --spring.config.location=/…/application.yaml
    ```