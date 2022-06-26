#include "../../includes/ircd.hpp"

void
    IRCD::quit()
{
    std::string message = "Quit";
    if (_request->parameter.size())
        message += " :" + _request->parameter[0];
    m_disconnect(message);
}
