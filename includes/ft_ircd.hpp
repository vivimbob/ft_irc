#ifndef FT_IRCD_HPP
#define FT_IRCD_HPP

#include "server.hpp"
#include "socket.hpp"

class FT_IRCD : public
{
    int  _kqueue;
    void m_create_kqueue();

    Server server(int kqueue);
};

#endif /* FT_IRCD_HPP */
