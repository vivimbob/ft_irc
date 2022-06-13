#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

int                        kq;
std::map<int, std::string> buffers;

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
    buffers.insert(std::make_pair(fd, ""));
    return (fd);
}

int
    main(int argc, char** argv)
{
    if (argc < 2)
        return (0);
    int           fd[--argc];
    struct kevent events[512];
    std::string   buffer;
    std::string   message;
    int           i;
    int           count;
    int           len;
    timespec      timer;
    timer.tv_sec                 = 2;
    std::string register_message = "pass 1234\r\nnick test\r\nuser testuser "
                                   "testhost testservername :test realname\r\n";

    if ((kq = kqueue()) == -1)
        return (1);
    for (i = 0; i < argc; ++i)
        fd[i] = client(argv[i + 1]);
    for (i = 0; i < argc; ++i)
        write(fd[i], register_message.data(), register_message.size());
    while ((count = kevent(kq, NULL, 0, events, 512, &timer)) > 0)
    {
        for (i = 0; i < count; ++i)
        {
            buffer.resize(events[i].data);
            len = read(events[i].ident, (void*)buffer.data(), events[i].data);
            buffer[len] = '\0';
            buffers[events[i].ident].append(buffer);
        }
    }
    if (count == -1)
        std::cout << "kevent: " << count << ": " << std::strerror(errno)
                  << std::endl;
    for (i = 0; i < argc; ++i)
    {
        std::cout << "-------------------------" << fd[i]
                  << "---------------------------" << '\n'
                  << buffers[fd[i]].data() << std::endl;
        buffers[fd[i]].clear();
    }
    while (true)
    {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);
        std::system("clear");
        std::cout << "\n\n" << message << "\n" << std::endl;
        message += "\r\n";
        for (i = 0; i < argc; ++i)
            write(fd[i], message.data(), message.size());
        while ((count = kevent(kq, NULL, 0, events, 512, &timer)) > 0)
        {
            for (i = 0; i < count; ++i)
            {
                buffer.resize(events[i].data);
                len =
                    read(events[i].ident, (void*)buffer.data(), events[i].data);
                buffer[len] = '\0';
                buffers[events[i].ident].append(buffer);
            }
        }
        if (count == -1)
            std::cout << "kevent: " << count << ": " << std::strerror(errno)
                      << std::endl;
        for (i = 0; i < argc; ++i)
        {
            std::cout << "-------------------------" << fd[i]
                      << "---------------------------" << '\n'
                      << buffers[fd[i]].data() << std::endl;
            buffers[fd[i]].clear();
        }
    }
}
