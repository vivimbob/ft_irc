#include "../includes/test.hpp"
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

void
    run_server(s_server_fd &server_fd)
{
#if TEST_IRCSERV == 1
    char *ircserv_argv[4];
    ircserv_argv[0]   = (char *)"../ircserv";
    ircserv_argv[1]   = (char *)"6668";
    ircserv_argv[2]   = (char *)"1234";
    ircserv_argv[3]   = NULL;
    server_fd.ircserv = fork();
    if (server_fd.ircserv == 0)
    {
        int log_file =
            open("./log/ircserv.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (log_file < 0)
            std::cout << "log_file open error\n";
        dup2(log_file, 1);
        dup2(log_file, 2);
        execv(ircserv_argv[0], ircserv_argv);
    }
    if (server_fd.ircserv < 0)
    {
        std::cout << "irc server run fail\n";
        exit(0);
    }
    else
        std::cout << "irc server are running\n";
#endif

#if TEST_ERGO == 1
    char *ergo_argv[3];
    ergo_argv[0]   = (char *)"./ergo";
    ergo_argv[1]   = (char *)"run";
    ergo_argv[2]   = NULL;
    server_fd.ergo = fork();
    if (server_fd.ergo == 0)
    {
        int log_file =
            open("./log/ergo.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (log_file < 0)
            std::cout << "log_file open error\n";
        dup2(log_file, 1);
        dup2(log_file, 2);
        chdir("ergo");
        execv(ergo_argv[0], ergo_argv);
    }
    if (server_fd.ergo < 0)
    {
        std::cout << "ergo server run fail\n";
        exit(0);
    }
    else
        std::cout << "ergo servers are running\n";
#endif
}

void
    kill_server(s_server_fd server_fd)
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
    (void)argc;
    (void)argv;

    s_server_fd server_fd;
    run_server(server_fd);
    sleep(2);
    test_start();
    kill_server(server_fd);
}
