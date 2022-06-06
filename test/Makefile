NAME    	= tester 
CC      	= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
# CXXFLAGS  = -std=c++98

testcase	= register.cpp

srcs		= main.cpp\
			  client.cpp\
			  test.cpp\
			  $(testcase:%=testcase/%)

SRCS    	= $(srcs:%=srcs/%)

OBJS		= $(SRCS:%.cpp=%.o)

all     	: $(NAME)

%.o     	: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

$(NAME)   	: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean   	:
	rm -rf $(OBJS) ./fifo/* ./log/*

fclean    	: clean
	rm -rf $(NAME)

re			: fclean all
