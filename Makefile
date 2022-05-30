NAME    = ircserv
CC      = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
# CXXFLAGS  = -std=c++98

SRCS    = maaain.cpp\
		srcs/channel.cpp\
		srcs/client.cpp\
		srcs/ircmessage.cpp\
    srcs/logger.cpp\
		srcs/server.cpp\
		srcs/utils.cpp\
		srcs/reply.cpp

OBJS    = $(SRCS:%.cpp=%.o)

all     : $(NAME)

%.o     : %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

$(NAME)   : $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean   :
	rm -rf $(OBJS)

fclean    : clean
	rm -rf $(NAME)

re      : fclean all
