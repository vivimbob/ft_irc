NAME    	= ircserv
CC      	= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98
LIBFLAGS	= -L lib -l logger
# CXXFLAGS  = -std=c++98

srcs		= main.cpp\
			  channel.cpp\
			  client.cpp\
			  message.cpp\
			  server.cpp\
			  utils.cpp\
			  sendbuffer.cpp\
			  membership.cpp

SRCS    	= $(srcs:%=srcs/%)

OBJS		= $(SRCS:srcs/%.cpp=objs/%.o)

all     	: $(NAME)

objs/%.o     	: srcs/%.cpp
	@mkdir -p $(dir ./objs/$*)
	$(CC) $(CXXFLAGS) -c $< -o $@

$(NAME)   	: $(OBJS)
	$(CC) $(CXXFLAGS) $(LIBFLAGS) $(OBJS) -o $(NAME)

clean   	:
	rm -rf ./objs

fclean    	: clean
	rm -rf $(NAME)

re			: fclean all
