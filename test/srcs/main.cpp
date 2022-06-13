#include <arpa/inet.h>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int kq;

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
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        std::cerr << "socet create fail" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "socket create: " << socket_fd << std::endl;

    fcntl(socket_fd, O_NONBLOCK);

    sockaddr_in sock;

    memset(&sock, 0, sizeof(sockaddr_in));
    sock.sin_family      = AF_INET;
    sock.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock.sin_port        = htons(atoi(port));
    if (connect(socket_fd, (sockaddr*)&sock, sizeof(sockaddr_in)) == -1)
    {
        std::cerr << "connect with 127.0.0.1:" << sock.sin_port << "fail"
                  << std::endl;
        exit(1);
    }
    std::cerr << "connect with 127.0.0.1:" << sock.sin_port << "success"
              << std::endl;
    m_update_event(socket_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    return (socket_fd);
}

int
    main(int argc, char** argv)
{
    int fd[argc - 1];
    kq = kqueue();

    if (kq == -1)
        exit(1);
    for (int i = 0; i < argc - 1; ++i)
        fd[i] = client(argv[i + 1]);

    std::string   buffer;
    char          recv_buffer[512];
    int           read_len;
    int           event_count;
    struct kevent kev[1024];
    timespec      timer;
    timer.tv_sec = 1;
    while (true)
    {
        std::cout << "Enter message: ";
        std::getline(std::cin, buffer);
        buffer += "\r\n";
        for (int i = 0; i < argc - 1; ++i)
            write(fd[i], buffer.data(), buffer.size());
        event_count = kevent(kq, NULL, 0, kev, 1024, &timer);
    }
    // 전송 후
}
