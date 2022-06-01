#include "../includes/ircmessage.hpp"
#include "../includes/logger.hpp"
#include "../includes/client.hpp"

IRCMessage::IRCMessage(Client *client, const std::string &message)
    : m_message(message),
      m_valid_message(true),
      m_position(0),
	  m_client(client)
{
}

size_t
    IRCMessage::next_position(void)
{
    m_position = m_message.find(' ', m_position);
    if (m_position == static_cast<size_t>(std::string::npos))
        m_position = m_message.size();
    return m_position;
}

void
    IRCMessage::parse_message(void)
{
    if (m_message.size() > m_position && m_message[0] == ':')
    {
        m_prefix.assign(m_message.begin() + 1, m_message.begin() + next_position());
        m_position = m_message.find_first_not_of(' ', m_position);
    }
    if (m_message.size() > m_position)
    {
        m_command.assign(m_message.begin() + m_position, m_message.begin() + next_position());
        m_position = m_message.find_first_not_of(' ', m_position);
        std::string::iterator it = m_command.begin();
        std::string::iterator ite = m_command.end();
        for (; it != ite; ++it)
          if ((unsigned)*it - 'a' < 26)
            *it ^= 0b100000;
    }
    //command가 유효한지 체크
    while (m_message.size() > m_position)
    {
        if (m_message[m_position] == ':')
        {
            std::string s(m_message.begin() + m_position + 1, m_message.begin() + next_position());
            while (m_message.size() > m_position)
            {
                s.append(" ");
                m_position = m_message.find_first_not_of(' ', m_position);
                s.append(m_message.begin() + m_position, m_message.begin() + next_position());
            }
            m_parameters.push_back(s);
        }
        else
        {
            m_parameters.push_back(std::string(m_message.begin() + m_position, m_message.begin() + next_position()));
            m_position = m_message.find_first_not_of(' ', m_position);
        }
    }
}

const std::string   &IRCMessage::get_message(void) const
{
    return m_message;
}

const std::string   &IRCMessage::get_prefix(void) const
{
    return m_prefix;
}

const std::string   &IRCMessage::get_command(void) const
{
   return m_command;
}

const std::vector<std::string>  &IRCMessage::get_params(void) const
{
    return m_parameters;
}

const bool    &IRCMessage::is_valid_message(void) const
{
    return m_valid_message;
}

IRCMessage::~IRCMessage(void)
{
}

std::string
  IRCMessage::build_message(void)
{
    std::string param = "";
    std::string msg;

    if (m_command == "QUIT")
    {
        if (!m_parameters.empty())
            param = m_parameters[0];
        msg = m_client->m_get_nickname() + '!' + m_client->m_get_username() + '@' + m_client->m_get_hostname();
        msg += " QUIT :" + param + "\r\n";
    }
    else if (m_command == "PART")
    {
        if (m_parameters.size() == 2)
            param = m_parameters[1];
        else
            param = m_client->m_get_nickname();
        msg = ':' + m_client->m_get_nickname() + '!' + m_client->m_get_username() + '@' + m_client->m_get_hostname();
        msg += " PART " + m_parameters[0] + " :" + param + "\r\n"; 
    }
    return msg;
}
