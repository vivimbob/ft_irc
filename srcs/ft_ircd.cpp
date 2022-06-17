#include "../includes/ft_ircd.hpp"

void
    FT_IRCD::m_create_kqueue()
{
    _kqueue = kqueue();
    if (_kqueue == -1)
    {
        Logger().error() << "Failed to allocate kqueue. errno: " << errno << ":"
                         << strerror(errno);
        exit(EXIT_FAILURE);
    }
    Logger().info() << "Allocate kqueue " << _kqueue;
    m_set_event(FT_IRCD::_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    Logger().info() << "Listen socket(" << FT_IRCD::_fd
                    << ") assign read event to kqueue";
}

int
    main(int argc, char** argv)
{
    int port;

    if (argc != 3)
    {
        Logger().error() << "Usage :" << argv[0] << " <port> <password>";
        return EXIT_FAILURE;
    }
    if (PORT_MAX < (unsigned)(port = atoi(argv[1])))
    {
        Logger().error() << port << "is out of Port range (0 ~ 65535)";
        return EXIT_FAILURE;
    }

    Server(port, argv[2]).run();
}
