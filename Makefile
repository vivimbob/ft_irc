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

lib		= logger.cpp

SRCS    	= $(srcs:%=srcs/%)

LIB				= $(lib:%=lib/%)

OBJS		= $(SRCS:%.cpp=%.o)

all     	: $(NAME)

%.o     	: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

$(NAME)   	: $(OBJS)
	$(CC) $(CXXFLAGS) $(LIBFLAGS) $(OBJS) -o $(NAME)

clean   	:
	rm -rf $(OBJS)

fclean    	: clean
	rm -rf $(NAME)

re			: fclean all