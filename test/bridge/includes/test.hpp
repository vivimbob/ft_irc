#ifndef TEST_HPP
#define TEST_HPP
#include <string>

#define TEST_ERGO    1
#define TEST_IRCSERV 1

struct s_server_fd
{
    int ergo;
    int ircserv;
};

int execute(char* command);
int execute(std::string command);

void test_register();
void test_private_message();
void test_list_message();
void test_notice_message();
void test_invite_message();
void test_kick_message();

void test_start();

static char* bash_argv[4];
#endif /* TEST_HPP */
