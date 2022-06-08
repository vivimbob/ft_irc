NAME    	= ircserv
CC      	= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
# CXXFLAGS  = -std=c++98

commands	= join.cpp\
			  mode.cpp\
			  nick.cpp\
			  part.cpp\
			  pass.cpp\
			  quit.cpp\
			  topic.cpp\
			  user.cpp\
			  names.cpp\
			  privmsg.cpp\
				list.cpp

srcs		= main.cpp\
			  channel.cpp\
			  client.cpp\
			  message.cpp\
			  logger.cpp\
			  server.cpp\
			  utils.cpp\
			  sendbuffer.cpp\
			  membership.cpp\
			  $(commands:%=commands/%)

SRCS    	= $(srcs:%=srcs/%)

OBJS		= $(SRCS:%.cpp=%.o)

all     	: $(NAME)

%.o     	: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

$(NAME)   	: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean   	:
	rm -rf $(OBJS)

fclean    	: clean
	rm -rf $(NAME)

re			: fclean all
