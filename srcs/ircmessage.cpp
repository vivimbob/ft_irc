#include "../includes/ircmessage.hpp"
#include "../includes/logger.hpp"
#include "../includes/client.hpp"

Message::Message(Client *client, const std::string &message)
    : m_message(message),
      m_valid_message(true),
      m_position(0),
	  m_client(client)
{
}

size_t
    Message::next_position(void)
{
    m_position = m_message.find(' ', m_position);
    if (m_position == static_cast<size_t>(std::string::npos))
        m_position = m_message.size();
    return m_position;
}

void
    Message::parse_message(void)
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

const std::string   &Message::get_message(void) const
{
    return m_message;
}

const std::string   &Message::get_prefix(void) const
{
    return m_prefix;
}

const std::string   &Message::get_command(void) const
{
   return m_command;
}

const std::vector<std::string>  &Message::get_params(void) const
{
    return m_parameters;
}

const bool    &Message::is_valid_message(void) const
{
    return m_valid_message;
}

Message::~Message(void)
{
}
