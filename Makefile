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
			  user.cpp

srcs		= channel.cpp\
			  client.cpp\
			  ircmessage.cpp\
			  logger.cpp\
			  server.cpp\
			  utils.cpp\
			  reply.cpp\
			  $(commands:%=commands/%)

SRCS    	= main.cpp\
			  $(srcs:%=srcs/%)

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
