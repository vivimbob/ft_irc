NAME    	= tester 
CC      	= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
# CXXFLAGS  = -std=c++98

testcase	= register.cpp\
			  private_message.cpp\
				list_message.cpp\
				notice_message.cpp\
				invite_message.cpp\
				kick.cpp

srcs		= main.cpp\
			  client.cpp\
			  test.cpp\
			  bulk_client.cpp\
			  $(testcase:%=testcase/%)

SRCS    	= $(srcs:%=srcs/%)

OBJS		= $(SRCS:%.cpp=%.o)

all     	: $(NAME)
	make -C ..

%.o     	: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

$(NAME)   	: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean   	:
	rm -rf $(OBJS) ./fifo/* ./log/*

fclean    	: clean
	rm -rf $(NAME)

re			: fclean all

#살아 있는 서버있는지 체크
check		:
	lsof -i :6667,6668
