NAME    	= ircserv
CC      	= c++
CXXFLAGS	= #-Wall -Wextra -Werror -std=c++98
LIBFLAGS	= -L lib -l logger
LIBRARY		= lib/liblogger.a
# CXXFLAGS  = -std=c++98

srcs		= ft_ircd.cpp\
			  buffer.cpp\
			  channel.cpp\
			  client.cpp\
			  event.cpp\
			  membership.cpp\
			  socket.cpp\
			  utils.cpp\
			  irc.cpp\
			  stringbuilder.cpp

lib		= logger.cpp

SRCS    	= $(srcs:%=srcs/%)

OBJS		= $(SRCS:srcs/%.cpp=objs/%.o)

all     	: $(LIBRARY) $(NAME)

objs/%.o   	: srcs/%.cpp
	@mkdir -p $(dir ./objs/$*)
	$(CC) $(CXXFLAGS) -c $< -o $@

$(NAME) 	: $(OBJS)
	$(CC) $(CXXFLAGS) $(LIBFLAGS) $(OBJS) -o $(NAME)

$(LIBRARY)	: lib/logger.cpp
	$(CC) $(CXXFLAGS) -c lib/logger.cpp -o lib/logger.o
	ar -r $(LIBRARY) lib/logger.o


clean   	:
	rm -rf ./objs ./lib/logger.o

fclean    	: clean
	rm -rf $(NAME) $(LIBRARY)

re			: fclean all
