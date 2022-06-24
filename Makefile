NAME    	= ircserv
CC      	= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -g
RM			= rm -rf

srcs		= ft_ircd.cpp\
			  channel.cpp\
			  client.cpp\
			  event.cpp\
			  socket.cpp\
			  irc.cpp\
			  ircd.cpp\
			  ircd/bot.cpp\
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
	$(RM) ./objs

fclean    	: clean
	$(RM) $(NAME)

re			: fclean all

.PHONY		: all clean fclean re
