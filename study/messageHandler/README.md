# Message Handler

 [[C++] Before / After 코드](https://github.com/almond0115/mju-backend-dev/blob/main/study/messageHandler/c++/README.md) 
, [[Python] Before / After 코드](https://github.com/almond0115/mju-backend-dev/blob/main/study/messageHandler/python/README.md)

## 문제 상황
* if문이나 switch문을 사용해도 메세지 타입이 많아질 수록 코드가 비대해집니다.
* 코드가 복잡해지는 이유는메시지 타입이 늘어남에 따라 “타입 &rarr; 핸들러” 맵핑이 늘어나기 때문이다.
* 핸들러 호출이 코드로 확정되므로동적으로 핸들러 호출을 변경해야될 경우 문제가 있다.
* 핸들러가 몇 개 안될 때는 굳이 필요 없을 수도 있습니다.

## Handler Map (Message Map)
* “메시지 타입 &rarr; 메시지 핸들러” 관계를 저장하는 map (= dictionary, associative array, …) <br> 각 프로그래밍 언어 별로 제공되는 map 이나 dict 형태를 활용 가능합니다.
* 동적으로 처리 코드를 변경하는데 유리합니다. <br> 단순히 함수 하나를 교체하는 것도 가능하고 처리 함수를 리스트 형태로 연결하여 복잡한 동작을 구현하는 것도 가능합니다. 
* 이 때문에 서버의 기능을 동적으로 변경하는 데 사용이 가능합니다.


