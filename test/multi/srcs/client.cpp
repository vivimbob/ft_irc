#include "../includes/client.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

Client::Client(std::string nick, int port) : nick(nick), port(port)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        std::cerr << "socet create fail" << std::endl;
        exit(EXIT_FAILURE);
    }

    fcntl(fd, O_NONBLOCK);

    sockaddr_in sock;

    memset(&sock, 0, sizeof(sockaddr_in));
    sock.sin_family      = AF_INET;
    sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock.sin_port        = htons(port);
    if (connect(fd, (sockaddr*)&sock, sizeof(sockaddr_in)) == -1)
    {
        std::cerr << "connect with 127.0.0.1:" << sock.sin_port << "fail"
                  << std::endl;
        std::cerr << "errno " << errno << " " << strerror(errno) << std::endl;
        exit(1);
    }
}

Client::~Client()
{
}
