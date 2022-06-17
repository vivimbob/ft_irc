#include <arpa/inet.h>
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <istream>
#include <map>
#include <pthread.h>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

int                        kq;
std::map<int, std::string> server_buffers;
#define SIZE_EVENTS 1024
std::string   server_names[3] = {" ft_ircd", " miniircd", " ergo"};
std::string   server_trims[3];
struct kevent events[SIZE_EVENTS];
std::string   buffer;
std::string   message;
int           i;
int           count;
int           pos;
int           len;
timespec      timer = {2, 0};

void
    m_update_event(int     identity,
                   short   filter,
                   u_short flags,
                   u_int   fflags,
                   int     data,
                   void*   udata)
{
    struct kevent kev;
    EV_SET(&kev, identity, filter, flags, fflags, data, udata);
    kevent(kq, &kev, 1, NULL, 0, NULL);
}

int
    client(char* port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        std::cerr << "socet create fail" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "socket create: " << fd << std::endl;

    fcntl(fd, O_NONBLOCK);

    sockaddr_in sock;

    memset(&sock, 0, sizeof(sockaddr_in));
    sock.sin_family      = AF_INET;
    sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock.sin_port        = htons(atoi(port));
    if (connect(fd, (sockaddr*)&sock, sizeof(sockaddr_in)) == -1)
    {
        std::cerr << "connect with 127.0.0.1:" << sock.sin_port << "fail"
                  << std::endl;
        exit(1);
    }
    std::cerr << "connect with 127.0.0.1:" << sock.sin_port << "success"
              << std::endl;
    m_update_event(fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    server_buffers.insert(std::make_pair(fd, ""));
    return (fd);
}

int
    main(int argc, char** argv)
{
    bool toggle = false;
    if (argc < 3)
        return (0);
    argc -= 2;
    int         fd[argc];
    std::string nick = argv[1];
    std::string register_message =
        "pass 1234\r\nnick " + nick +
        "\r\nuser testuser testhost testservername :test realname\r\n";

    if ((kq = kqueue()) == -1)
        return (1);
    for (i = 0; i < argc; ++i)
        fd[i] = client(argv[i + 2]);
    for (i = 0; i < argc; ++i)
        write(fd[i], register_message.data(), register_message.size());
    while ((count = kevent(kq, NULL, 0, events, SIZE_EVENTS, &timer)) > 0)
    {
        for (i = 0; i < count; ++i)
        {
            buffer.resize(events[i].data);
            len = read(events[i].ident, (void*)buffer.data(), events[i].data);
            buffer[len] = '\0';
            server_buffers[events[i].ident].append(buffer);
        }
    }
    for (i = 0; i < argc; ++i)
    {
        server_trims[i] = (server_buffers[fd[i]].erase(
            server_buffers[fd[i]].find(' ') + 1, server_buffers[fd[i]].size()));
        std::cout << server_trims[i] << std::endl;
        server_buffers[fd[i]].clear();
    }
    while (true)
    {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);
        std::system("clear");
        std::cout << "\n\n\"" << message << "\"\n" << std::endl;
        message += "\r\n";
        for (i = 0; i < argc; ++i)
            write(fd[i], message.data(), message.size());
        if (message == "quit\r\n")
            return (0);
        message.clear();
        fflush(stdin);
        while ((count = kevent(kq, NULL, 0, events, SIZE_EVENTS, &timer)) > 0)
        {
            for (i = 0; i < count; ++i)
            {
                buffer.resize(events[i].data);
                len =
                    read(events[i].ident, (void*)buffer.data(), events[i].data);
                buffer[len] = '\0';
                server_buffers[events[i].ident].append(buffer);
            }
            toggle = true;
        }
        if (count == -1)
            std::cout << "kevent: " << count << ": " << std::strerror(errno)
                      << std::endl;
        if (toggle == true)
        {
            for (i = 0; i < argc; ++i)
            {
                if (server_buffers[fd[i]].find("PING") != std::string::npos)
                    write(fd[i], "PONG test\r\n", 11);
                pos = 0;
                while ((pos = server_buffers[fd[i]].find(
                            server_trims[i].data(), pos)) != std::string::npos)
                    server_buffers[fd[i]].replace(pos, server_trims[i].length(),
                                                  "");
                std::cout << "---------------------------\n"
                          << "fd " << fd[i] << server_names[i] << "\n\n"
                          << server_buffers[fd[i]].data() << std::endl;
                server_buffers[fd[i]].clear();
            }
            toggle = false;
        }
    }
}
