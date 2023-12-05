# lab9 - Java Spring Boot

## `Test #1`
### 새 프로젝트 생성

![sprintboot_1](/asset/img/sprintboot_1.png)

## `Test #2`
### build.gradle 수정 후 Spring Boot 로 만들기

![sprintboot_2](/asset/img/sprintboot_2.png)

* `plugins` 는 gradle 빌드 시스템 자체 또는 프로젝트 자체의 기능 추가를 의미합니다. <br>
&rarr; https://central.sonatype.com/ <br>

* `dependencies` 는 우리가 프로그래밍 시 사용할 모듈입니다. <br>
build.gradle 내의 repositories 부분은 dependencies에 의존성으로 기재된 module을 자동으로 설치 받는 저장소를 기재합니다. <br>
&rarr; https://central.sonatype.com/

### 의존성 옵션

#### compileOnly : compile-time에만 사용하고 run-time에는 사용하지 않습니다.

#### runtimeOnly : compile-time 할 때는 쓰지 않고 run-time에서만 사용하빈다.

#### implementation
    C 는 B 에 의존적이고, B 는 A 에 의존적이라고 하자
    B 가 implementation ‘A’ 라고 기재할 경우
    compile 할 때 A 가 변경되면 B 는 다시 빌드 하지만, C 는 하지 않는다.
    run-time 에는 C 를 띄우면 B 가 의존하고 있는 A 역시 사용됨

#### testImplementation : test가 없는 경우 main, test가 있는 경우 test 소스에 적용됩니다.


## `Test #3`
### @SpringBootApplication 로 태깅 된 클래스

![sprintboot_3](/asset/img/sprintboot_3.png)

*  `@SpringBootApplication` 태깅 된 클래스는 메인 클래스입니다.


## `Test #4`
### Spring Boot 실행하기

![sprintboot_4](/asset/img/sprintboot_4.png)

## `Test #5`
### @SpringBootApplication 따라 들어가보기

![sprintboot_5](/asset/img/sprintboot_5.png)

`@SpringBootConfiguration` 어노테이션을 들어가면 `@Configuration`이 있고 여기 안에 `@Component`가 있음을 확인할 수 있습니다.

## `Test #6`
### @RestController 사용해서 Hello World 만들어보기

![sprintboot_6](/asset/img/sprintboot_6.png)

![sprintboot_7](/asset/img/sprintboot_7.png)

```
프로젝트를 열고 실행한 후 다음과 같이 정상 접속이 되는 것을 확인할 수 있습니다. 
```

`@RestController` 또한 따라 들어가보면 `@Controller`에 속해있고 이는 `@Component`가 나오는 것을 확인할 수 있습니다. <br>
> **즉, 우리가 작성한 `BlogController`가 `Bean`으로 등록되어 Spring Container가 생명주기를 관리함을 알 수 있습니다.**

## `Test #7`
### application.yaml 로 앱 동작 방식 설정하기

* 각 모듈의 동작은 `yaml`이나 `properties`파일로 변경할 수 있습니다. <br>
&rarr; `@value` 에노테이션을 통해 우리가 작성하는 코드 역시 이 파일을 읽게할 수 있습니다.

* 두 파일은 표현 방식만 다르므로 취향에 맞게 선택하면 됩니다.

![sprintboot_8](/asset/img/sprintboot_8.png)

![sprintboot_9](/asset/img/sprintboot_9.png)

```
yaml 파일을 통해 port 번호를 기재하고 다시 서버를 띄우면 다음과 같이 8081번을 bind 했음을 확인할 수 있습니다.
```

## `Test #8`
### application.yaml 에 커스텀 설정 쓰고 읽기

![sprintboot_11](/asset/img/sprintboot_11.png)

![sprintboot_10](/asset/img/sprintboot_10.png)

![sprintboot_12](/asset/img/sprintboot_12.png)

```
@value 에노테이션을 통해 yaml 커스텀 설정을 읽게하면 다음처럼 활용할 수 있습니다.
```

## `Test #9`
### JSON 반환

![sprintboot_13](/asset/img/sprintboot_13.png)

![sprintoot_14](/asset/img/sprintoot_14.png)

> 여기서는 여백 때문에 Object를 반환하게 했으니 `ArrayList<String>` 처럼 구체적인 타입을 명시하는 것이 더 좋습니다.

> 앞의 GET /articles/1 은 JSON을 반환하긴 했지만, HashMap<String, String>이므로 Value가 모두 String 이어야 합니다. <br>

&rarr; 실제로는 Value가 다양한 타입이 될 수 있으므로, HashMap 보다는 직접 정의하는 class 객체를 반환하는 경우가 더 많을 것입니다. <br>
(앞의 "title"에 대응되는 value는 문자열이 아니라 숫자가 더 자연스럽습니다.)


## `Test #10`
### Custom Object 반환

#### 새로 추가 할 클래스

![sprintoot_15](/asset/img/sprintoot_15.png)

<br>

#### BlogController 의 `getArticle()` 수정

![springboot_16](/asset/img/springboot_16.png)

> GET /article/1 의 숫자 부분은 article 번호를 의도했습니다.

&rarr; 이 숫자는 가변적이어야 하는데 이처럼 하드코딩하여 핸들러를 등록해서는 안됩니다.

> Flask에서 /`<username>`/cart 처럼 path에 변수를 사용할 수 있었습니다.

&rarr; Spring Boot도 아래와 같이 유사한 방식을 사용할 수 있습니다.

## `Test #11`
### @PathVariable

![springboot_17](/asset/img/springboot_17.png)

![springboot_18](/asset/img/springboot_18.png)

> URL에서 변수 부분은 /article/{number} 처럼 중괄호 안에 변수 이름을 지정합니다.

> 핸들러 매개 변수는 `@PathVariable`로 태깅하고 대개 path 변수와 같은 이름을 사용합니다.

<br>
 
```java
@GetMapping("/article/{number})
public Object getArticle(@PathVariable("number") int articleNumber)
```
&rarr; 명시적으로 path 변수 이름을 지정하고 싶으면 다음처럼 합니다.

## `Test #12`
### @RequestBody

RESTful API에서 POST나 PUT은 대개 `request body`에 JSON 문자열을 담고 있습니다. <br>

> 예: POST /article 은 {"title":"요리", "author": {"name":"DK"}} 같은 내용을 포함합니다.

앞에서 보듯 `request` 객체를 통해 body 내용을 `String`으로 읽고 이를 다시 `JSON` 객체로 만드는 것은 복잡합니다. <br>

그리고 `JSON` 객체를 만든 후에 문자열인 key 를 통해 값을 얻어내는 것은 key 에 오타를 내서 실수하기도 좋고 코드도 읽기 어렵습니다.

> 예: Pseudo code로 써봐도 System.out.println(parsedJson["title"]) 이런 식이 됩니다. <br>
> 더욱이 value가 다시 JSON 객체라면? (parsedJson["author"]["name"]) <br>

이 경우 JSON 객체가 아니라 우리가 다루는 데이터 타입을 반영하는 커스텀 클래스 객체면 더 좋을 것입니다. <br>

> 예: System.out.println(parsedMyObject.author.name) 

![springboot_19](/asset/img/springboot_19.png)

![springboot_20](/asset/img/springboot_20.png)

```
@RequestBody를 통해 PostArticleRequest의 객체를 매개변수로 Post 값을 받아옵니다.
```

![springboot_21](/asset/img/springboot_21.png)

![springboot_22](/asset/img/springboot_22.png)

```
$ curl -X POST -H "Content-Type: application/json; charset=utf-8" --data "{\"title\" : \"aaa\"}" http://localhost:8081/article -v 

다음 명령어를 통해 호출된 {"title" : "aaa"} 값의 PostArticleRequest의 객체의 특정 값(body.title)으로 저장되어 반환됩니다.
```

![springboot_23](/asset/img/springboot_23.png)

![springboot_24](/asset/img/springboot_24.png)

```
반환 타입을 PostArticleRequest 이 아닌 String으로 한다면 JSON 변환 값 그대로 반환해줍니다.
```

## `Test #13`
### Response Status Code 지정

![springboot_25](/asset/img/springboot_25.png)

<br>

```
error 1 : HttpServletResponse 객체의 메서드를 이용 (매개변수 위치 관계 X)
```
![springboot_26](/asset/img/springboot_26.png)

```
error 2 : handler가 ResponseEntity를 반환하는 방법
```
![springboot_27](/asset/img/springboot_27.png)

```
error 3 : ResponseStatusException을 throw 하는 방법
```
![springboot_28](/asset/img/springboot_28.png)

## `Test #14`
### template 디렉터리 추가

1. main → resources 아래 templates 라는 서브디렉터리를 만듭니다.
2. 만들어진 templates 에 우클릭 후 open in → terminal 을 선택합니다.
3. 터미널이 열리면 다음 명령을 실행합니다.
    ```
    $ wget http://sysdesignlab.mju.ac.kr/spring/index.mustache
    ```

4. build.gradle 의 dependencies 에 다음을 추가 후 gradle 을 reload 한다.
    ```
    $ implementation 'org.springframework.boot:spring-boot-starter-mustache:3.2.0'
    ```
5. 이제 @Controller 로 태깅된 클래스의 핸들러에서 mustache 확장자를 빼고 파일 이름을 반환하면 <br>
그 mustache template 을 이용해서 HTML 이 생성되게 된다.

## `Test #15`
### @Controller

@RestController 는 @Controller 를 상속한 것입니다. <br>

![springboot_30](/asset/img/springboot_30.png)

<details>
<summary> @RestController로 태깅된 BlogController에 추가된 위 코드는 의도한 것처럼 동작하지 않습니다. 왜일까요? </summary>

Mustache 파일은 일반적으로 템플릿 엔진으로 사용되어 HTML 등의 문서를 동적으로 생성하는 데 쓰입니다. <br>
`@RestController`를 사용해 해당 컨트롤러의 메소드가 데이터를 반환하면, 기본적으로 데이터가 직렬화되어 클라이언트에게 전송됩니다.<br> 
반면에 `index.mustache`파일은 HTML이나 다른 문서 형식으로 렌더링 됩니다. <br> 
따라서 일반적으로 @Controller 어노테이션을 사용한 클래스에서 다루는 것이 더 적절합니다.<br>

![springboot_31](/asset/img/springboot_31.png)
</details>

![springboot_32](/asset/img/springboot_32.png)

![springboot_33](/asset/img/springboot_33.png)
```
@Controller 어노테이션이 붙은 ViewController 클래스에서 정상 작동하는 것을 확인할 수 있습니다.
```

## `Test #16`
### View 에 Model 적용하기

`Test #15` 의 경우 고정된 View를 반환하였습니다. <br>

대개의 경우 business logic 에 의해 생성되는 결과를 View 에 반영해야 합니다. <br>

Spring 에서는 이 business logic 결과물이 Model 입니다. <br>

```
index.mustache 안 Hello 뒤에 {{ name }} 을 추가합니다. 
그리고 getIndex() 를 다음과 같이 수정합니다.
```

![springboot_34](/asset/img/springboot_34.png)

![springboot_35](/asset/img/springboot_35.png)

```
mustache 파일의 name 위치에 model을 통해 주입한 MJU 가 정상 출력됨을 확인할 수 있습니다.
```

## `Test #17`
### @Service

![springboot_37](/asset/img/springboot_37.png)

```
@Service 는 Business 계층의 @Component 라는 의미입니다.
@PostConstruct 는 Bean 이 만들어 진 후 초기화 함수 지정할 때 사용합니다.
```

## `Test #18`
### @Autowired

@Autowired 는 해당 타입의 bean 을 찾아서 연결하라는 의미입니다.

* Bean 은 Spring Container 에 의해 관리되며, singleton (= 오직 한 객체만 존재) 입니다.

* @Autowired 로 지정하면 의존성을 명시하는 것이고, <br>
Dependency Injection 으로 해당 객체 값이 singleton 으로 채워집니다. <br>
(**`주의! 나를 위해 새로 만들어 주는 것이 아니라 프로세스에 하나 뿐인 공유 객체임`**) 

<br>

> BlogController 에 다음 멤버를 추가합니다.
```java
@RestController
public class  BlogController {
    @Autowired
    ArticleManager articleManager;
    ...

    @GetMapping("/article/titles")
    public Object getArticleTitles() {
        return articleManager.getTitles();
    }

    @GetMapping("/article/{number}")
    public Object getArticle(@PathVariable int number) {
        GetArticleResponse r = new GetArticleResponse();
        r.number = number;
        r.title = articleManager.getTitleAt(number);
        return r;
    }

    @PostMapping("/article")
    public void postArticle(@RequestBody PostArticleRequest body){
        articleManager.getTitles().add(body.title);
    }
```

![springboot_38](/asset/img/springboot_38.png)
```
Service 의 List 값이 올바르게 출력되는 것을 확인할 수 있습니다.
```
<br>

![springboot_39](/asset/img/springboot_39.png)
```
POST 명령을 통해 새로운 글이 정상적으로 추가되는 것을 확인할 수 있습니다.
```

## `Test #19`
### Lombok 의 @Getter

```java
@Service
public class ArticleManager {
    @Getter
    private List<String> titles;
    ...
//    public List<String> getTitles() {
//        return titles;
//    }
    ...
}
```
```
curl 예제가 정상적으로 잘 작동하는 것을 확인할 수 있습니다.
```

## `Test #20`
### Spring 에서의 Logging

#### Logger 객체를 생성하고 이를 이용합니다
logger 를 만들 때 이름을 부여할 수 있으므로 보통은 bean 별로 logger 를 만듭니다. <br>
이 이름은 해당 logger 로 로그를 남길 때 같이 남게 됩니다.

```java
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@RestController
public class  BlogController {
    Logger logger = LoggerFactory.getLogger(BlogController.class);
    ...
    @GetMapping("/hello")
    public String hello() {
        logger.debug("hello");
        logger.info("hello");
        logger.warn("hello");
        logger.error("hello");

        return "world" + articlesPerPage;
    }
    ...
}
```

```
$ curl -X GET http://localhost:8081/hello  
```

![springboot_41](/asset/img/springboot_41.png)

&rarr;  로그에 남은 `o.mjubackend.controller.BlogController` 는 logger 이름이고, logger 생성 시 전달한 class 에서 추출한 것입니다.

&rarr; 로그의 레벨이 INFO, WARN, ERROR 로 구분된 것을 알 수 있습니다.

&rarr; `DEBUG는 표시되지 않았습니다. ` 이는 설정에서 DEBUG가 무시되게 되어 있어서 입니다. ( = 로그 레벨을 INFO로 설정)

## `Test #21`
### Spring Logging 설정 &rarr; 전체 로그 수준 변경

application.yaml 에 다음 내용을 추가합니다.
```java
logging:
  level:
    root: DEBUG
```

![springboot_42](/asset/img/springboot_42.png)
```
yaml 파일 수정을 통해 DEBUG 로그가 출력되는 것을 확인할 수 있습니다.
```

참고: 로그 레벨을 X 로 지정하면 X 이상의 로그들이 모두 출력됩니다. <br>

> 예: DEBUG 로 지정 → DEBUG, INFO, WARN, ERROR 출력 <br>
&nbsp; &nbsp; &nbsp;   WARN 로 지정  → WARN, ERROR 출력

```
이 예제에서는 root (= 기본) 로그 레벨을 DEBUG 로 설정했으므로,
모든 logger의 DEBUG 로그가 출력되어 로그가 너무 많이 나옵니다.
```

## `Test #22`
### Spring Logging 설정 &rarr; 특정 로거의 로그 수준 변경

![springboot_43](/asset/img/springboot_43.png)

```
BlogController logger 의 DEBUG 로그는 출력되지만,
다른 Logger들은 이전과 동일하게 INFO 이상만 출력됨을 알 수 있습니다.
```

## `Test #23`
### Spring Logging 설정 &rarr; 파일로 로그 남기기

![springboot_44](/asset/img/springboot_44.png)

```
logging 섹션에 다음처럼 file 서브 섹션을 추가하면,
GET /hello 를 연속으로 접근하여 로그 파일이 1KB를 넘게 될 때 아래와 같이 log rotation 이 됨을 알 수 있습니다.
```

![springboot_45](/asset/img/springboot_45.png)

## `Test #24`
### DOCKER 이미지 굽기 - jar 복사 및 실행

프로젝트 루트에 다음과 같이 Dockerfile 을 작성합니다. 

```docker
FROM amazoncorretto:17-alpine
COPY build/libs/blog-1.3.jar /app/
CMD ["java", "-jar", "/app/blog-1.3.jar"]
```
> COPY <src>… <dest> → 파일들을 docker image 내 대상 디렉터리로 복사 <br>

> CMD ["명령", "인자1", "인자2"…] → Docker container 가 수행할 시작 명령 지정

다음 명령어를 통해 Docker에 mjublog 파일을 등록 합니다.
```
$ docker build -t mjublog:1
$ docker run -d -p 8080:8081 mjublog:1
```

![springboot_47](/asset/img/springboot_47.png)
```
docker에 정상적으로 등록된 것을 확인할 수 있습니다.
```

![springboot_48](/asset/img/springboot_48.png)

![springboot_49](/asset/img/springboot_49.png)

```
docker에서 정상적으로 구동되어 8080 포트에서 잘 작동하고 있는 것을 확인할 수 있습니다.
```

## `Test #25`
### DOCKER 이미지 굽기 - log 디렉터리 처리

* Docker container 는 종료 시 그 내용이 사라집니다.

* 따라서 log file 처럼 영구적으로 남길(= persistent) 데이터를 유실 할 수 있습니다.

* 또한 container 안에 log 를 생성하게 될 경우 log file 의 증가에 따라 container 가 할당 받은 디스크가 부족해질 수도 있습니다.
    ```
    Q) log 를 쓸 디스크 용량이 부족하면 서버 프로세스에 어떤 일이 발생할까?
    이 때문에 docker run -v 옵션을 써서 host 의 volume 으로 mapping 합니다.
    ```
* 다음처럼 ①application.yaml 의 로그파일 경로를 수정한 후 <br>
  ②다시 gradle bootJar 를 실행하고, ③docker image 를 2번 태그로 생성합니다.

    ```
    file:
        name: /mylogdir/mylog.txt
    logback:
        rolling policy:
        max-file-size: 1KB
    ```

    <br>

    ![springboot_50](/asset/img/springboot_50.png)

    ```
    $ ./gradlew bootJar
    $ docker build -t mjublog:2 .
    $ docker run -d -p 8080:8081 -v /Users/jonghyunjung/temp/logs:/mylogdir -d mjublog:2
    ```
    <br>

    ![springboot_51](/asset/img/springboot_51.png)
    ![springboot_52](/asset/img/springboot_52.png)
    ```
    docker를 통해 2번 태그로 등록된 mjublog JAR 파일이 잘 구동되었습니다.
    -v 옵션을 통해 host의 volumn으로 정상 mapping 되어 파일이 저장되고 있음을 확인할 수 있습니다.
    ```

### DOCKER HUB 최종 제출

![springboot_54](/asset/img/springboot_54.png)

![springboot_53](/asset/img/springboot_53.png)

```
mjublog:2 image를 almond0115/lab9-mjublog 주소로 정상적으로 push 하였습니다. 
```