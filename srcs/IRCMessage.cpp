#include "../includes/IRCMessage.hpp"
#include "../includes/logger.hpp"

IRCMessage::IRCMessage(unsigned int clientfd, const std::string &message)
    : m_message(message),
      m_position(0),
      m_valid_message(true)
{
}

int
    IRCMessage::next_position(void)
{
    m_position = m_message.find(' ', m_position);
    if (m_position == std::string::npos)
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