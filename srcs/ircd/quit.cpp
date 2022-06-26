#include "../../includes/ircd.hpp"

/* ircd class quit function begin */

void
    IRCD::quit()
{
    std::string message = "Quit";
    if (_request->parameter.size())
        message += " :" + _request->parameter[0];
    m_disconnect(message);
}

/* ircd class quit function end */