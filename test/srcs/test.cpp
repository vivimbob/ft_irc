#include "../includes/test.hpp"
#include <unistd.h>
#include <iostream>

int execute(char *command)
{
	bash_argv[0] = (char *)"/bin/bash";
	bash_argv[1] = (char *)"-c";
	bash_argv[2] = command;
	bash_argv[3] = NULL;
	int fd = fork();
	if (fd == 0)
		execv(bash_argv[0], bash_argv);
	if (fd < 0)
	{
		std::cout << bash_argv[2] << ": execute fail\n";
		exit(1);
	}
	return fd;
}

int execute(std::string command)
{
	bash_argv[0] = (char *)"/bin/bash";
	bash_argv[1] = (char *)"-c";
	bash_argv[2] = (char *)command.data();;
	bash_argv[3] = NULL;
	int fd = fork();
	if (fd == 0)
		execv(bash_argv[0], bash_argv);
	if (fd < 0)
	{
		std::cout << bash_argv[2] << ": execute fail\n";
		exit(1);
	}
	return fd;
}

void test_start()
{
	test_register();
	//test_join();
	//test_part();
	//test_names();
	//test_list();
	//test_topic();
	//test_mix();
	//test_regular_user();
	//test_operator_user();
	//test_send_message();
	//test_private_message();
	//test_notice_message();
	//test_channel_commands();
	//test_invite();

	//test_user_mode();
	//test_channel_mode();
}
