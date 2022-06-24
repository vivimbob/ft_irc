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
			  ircd/invite.cpp\
			  ircd/join.cpp\
			  ircd/kick.cpp\
			  ircd/list.cpp\
			  ircd/mode.cpp\
			  ircd/names.cpp\
			  ircd/nick.cpp\
			  ircd/notice.cpp\
			  ircd/part.cpp\
			  ircd/pass.cpp\
			  ircd/privmsg.cpp\
			  ircd/quit.cpp\
			  ircd/topic.cpp\
			  ircd/user.cpp\
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
