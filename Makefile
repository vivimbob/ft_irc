NAME    	= ircserv
CC      	= c++
CXXFLAGS	= #-Wall -Wextra -Werror -std=c++98
# CXXFLAGS  = -std=c++98

srcs		= ft_ircd.cpp\
			  channel.cpp\
			  client.cpp\
			  event.cpp\
			  socket.cpp\
			  irc.cpp\
			  ircd.cpp\
			  log.cpp

SRCS    	= $(srcs:%=srcs/%)

OBJS		= $(SRCS:srcs/%.cpp=objs/%.o)

all     	: $(NAME)

objs/%.o   	: srcs/%.cpp
	@mkdir -p $(dir ./objs/$*)
	$(CC) $(CXXFLAGS) -c $< -o $@

$(NAME) 	: $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean   	:
	rm -rf ./objs

fclean    	: clean
	rm -rf $(NAME)

re			: fclean all
