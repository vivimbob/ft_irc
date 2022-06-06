#include "../includes/test.hpp"
#include <unistd.h>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include <sys/wait.h>

void run_server(s_server_fd &server_fd)
{
#if TEST_IRCSERV == 1
	//server_fd.ircserv = execute((char *)"../ircserv 6668 1234");
	server_fd.ircserv = execute((char *)"../ircserv 6668 1234 > ./log/ircserv.log 2>&1");
	if (server_fd.ircserv < 0)
	{
		std::cout << "irc server run fail\n";
		exit(0);
	}
	else
		std::cout << "irc server are runnig\n";
#endif

#if TEST_ERGO == 1
	chdir("ergo");
//	server_fd.ergo = execute((char *)"./ergo run");
	server_fd.ergo = execute((char *)"./ergo run > ../log/ergo.log 2>&1");
	chdir("..");
	if (server_fd.ergo < 0)
	{
		std::cout << "ergo server run fail\n";
		exit(0);
	}
	else
		std::cout << "ergo servers are runnig\n";
#endif
}

void kill_server(s_server_fd server_fd)
{
#if TEST_IRCSERV == 1
	kill(server_fd.ircserv, 9);
	std::cout << server_fd.ircserv << ": kill irc server\n";
#endif
#if TEST_ERGO == 1
	kill(server_fd.ergo, 9);
	std::cout << server_fd.ergo << ": kill ergo server\n";
#endif
}

int
	main(int argc, char **argv)
{
	(void)argc; (void)argv;

	s_server_fd server_fd;
	run_server(server_fd);
	sleep(2);
	test_start();
	kill_server(server_fd);
}
