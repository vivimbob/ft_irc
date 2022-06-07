기본적인흐름은 
ergo, ircserv를 포트를 달리해서 실행하고
nc로 서버 각각에 같은 메시지를 보낸 후
서버에서 보낸 메시지가 일치하는지 확인 하는 것입니다.

테스트 하고 싶은 서버를 고를 수 있습니다.
TEST_ERGO, TEST_IRCSERV 매크로 변수를 0으로 해서 컴파일 하면,
그 서버는 빼고 테스트 할 수 있습니다. 
기본 값은 둘 다 1 입니다.
서버 하나만 테스트 하면, 출력 결과를 비교하는 작업은 생략됩니다.

![image](https://user-images.githubusercontent.com/54225321/172152650-15e884e6-a545-45e8-bfed-f9067d51edad.png)
테스트 케이스들은 srcs/testcase에 적어주시면 됩니다.
위 코드는 srcs/testcase/register.cpp입니다.
서버에 등록하는 케이스들을 static하게 작성하고,
그 케이스들을 모두 함수를 하나 만들어서 test.hpp에 넣어주세요.

![image](https://user-images.githubusercontent.com/54225321/172152913-0d113786-67d0-46a1-8e64-e61758cd010b.png)
모든 케이스들을 실행하는 함수들은 srcs/test.cpp에 있는
test_start()함수에 집어 넣어주시면 됩니다.

![image](https://user-images.githubusercontent.com/54225321/172153099-a3066201-12db-419f-9197-abbd5847283b.png)
테스트 케이스를 작성하는 방식은
BulkClient 생성자 첫 번째 인자에 만들 클라이언트 수를 집어넣으시고,
두 번째 인자에는 테스트 하는 케이스의 이름을 집어 넣어주시면 됩니다.
그러면 생성자에서 nc를 1초 간격을 두고 실행시킵니다.

클라이언트에게 보낼 내용은
BulkClietn write함수를 사용하시면 됩니다.
첫 번째 인자는 메시지를 쓰고자 하는 클라이언트 번호입니다.
두 번째 인자는 보낼 메시지 입니다. 
