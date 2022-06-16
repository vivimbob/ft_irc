#include "../includes/message.hpp"
#include "../includes/client.hpp"
#include "../lib/logger.hpp"

/* message class constructor begin */

Message::Message(Client* client, const std::string& message)
    : m_message(message),
      m_valid_message(true),
      m_position(0),
      m_client(client)
{
}

Message::~Message()
{
}

/* message class constructor end */

/* message class main function begin */

size_t
    Message::next_position()
{
    m_position = m_message.find(' ', m_position);
    if (m_position == static_cast<size_t>(std::string::npos))
        m_position = m_message.size();
    return m_position;
}

void
    Message::parse_message()
{
    if (m_message.size() > m_position && m_message[0] == ':')
    {
        m_prefix.assign(m_message.begin() + 1,
                        m_message.begin() + next_position());
        m_position = m_message.find_first_not_of(' ', m_position);
    }
    if (m_message.size() > m_position)
    {
        m_command.assign(m_message.begin() + m_position,
                         m_message.begin() + next_position());
        m_position               = m_message.find_first_not_of(' ', m_position);
        std::string::iterator it = m_command.begin();
        std::string::iterator ite = m_command.end();
        for (; it != ite; ++it)
            if ((unsigned)*it - 'a' < 26)
                *it ^= 0b100000;
    }
    // command가 유효한지 체크
    while (m_message.size() > m_position)
    {
        if (m_message[m_position] == ':')
        {
            std::string s(m_message.begin() + m_position + 1, m_message.end());
            m_position = m_message.size() + 1;
            m_parameters.push_back(s);
        }
        else
        {
            m_parameters.push_back(
                std::string(m_message.begin() + m_position,
                            m_message.begin() + next_position()));
            m_position = m_message.find_first_not_of(' ', m_position);
        }
    }
}

/* message class main function end */

/* message class getter begin */

const std::string&
    Message::get_message() const
{
    return m_message;
}

const std::string&
    Message::get_prefix() const
{
    return m_prefix;
}

const std::string&
    Message::get_command() const
{
    return m_command;
}

const std::vector<std::string>&
    Message::get_params() const
{
    return m_parameters;
}

/* message class getter end */

/* message class is_function begin */

const bool&
    Message::is_valid_message() const
{
    return m_valid_message;
}

/* message class is_function end */

/* message class reply function begin */

std::string
    Message::reply_servername_prefix(std::string numeric_reply)
{
    std::string msg;
    msg = msg + ":ft_irc " + numeric_reply + " " + m_client->get_nickname();
    if (m_client->get_nickname().empty())
        msg += "*";
    return msg;
}

std::string
    Message::reply_nickmask_prefix(std::string command)
{
    std::string msg;
    msg = msg + ":" + m_client->make_nickmask() + " " + command;
    return msg;
}

std::string
    Message::err_no_such_nick(const std::string& nickname)
{
    return reply_servername_prefix("401") + " " + nickname +
           " :No such nick/channel\r\n";
}

std::string
    Message::err_no_such_channel(const std::string& channel_name)
{
    return reply_servername_prefix("403") + " " + channel_name +
           " :No such channel\r\n";
}

std::string
    Message::err_too_many_channels(const std::string& channel_name)
{
    return reply_servername_prefix("405") + " " + channel_name +
           " :You have joined too many channels\r\n";
}

std::string
    Message::err_too_many_targets(const std::string& target)
{
    return reply_servername_prefix("407") + " " + target +
           " :Duplicate recipients. No message delivered\r\n";
}

std::string
    Message::err_no_recipient()
{
    return reply_servername_prefix("411") + " :No recipient given (" +
           m_command + ")\r\n";
}

std::string
    Message::err_no_text_to_send()
{
    return reply_servername_prefix("412") + " :No text to send\r\n";
}

std::string
    Message::err_unknown_command()
{
    return reply_servername_prefix("421") + " " + m_command +
           " :Unknown command\r\n";
}

std::string
    Message::err_file_error(const std::string& file_op, const std::string& file)
{
    return reply_servername_prefix("424") + " :File error doing " + file_op +
           " on " + file + "\r\n";
}

std::string
    Message::err_no_nickname_given()
{
    return reply_servername_prefix("431") + " :No nickname given\r\n";
}

std::string
    Message::err_erroneus_nickname(const std::string& nick)
{
    return reply_servername_prefix("432") + " " + nick +
           " :Erroneus nickname\r\n";
}

std::string
    Message::err_nickname_in_use(const std::string& nick)
{
    return reply_servername_prefix("433") + " " + nick +
           " :Nickname is already in use\r\n";
}

std::string
    Message::err_user_not_in_channel(const std::string& nick,
                                     const std::string& channel)
{
    return reply_servername_prefix("441") + " " + nick + " " + channel +
           " :They aren't on that channel\r\n";
}

std::string
    Message::err_not_on_channel(const std::string& channel)
{
    return reply_servername_prefix("442") + " " + channel +
           " :You're not on that channel\r\n";
}

std::string
    Message::err_user_on_channel(const std::string& user,
                                 const std::string& channel)
{
    return reply_servername_prefix("443") + " " + user + " " + channel +
           " :is already on channel\r\n";
}

std::string
    Message::err_not_registered()
{
    return reply_servername_prefix("451") + " :You have not registered\r\n";
}

std::string
    Message::err_need_more_params()
{
    return reply_servername_prefix("461") + " " + m_command +
           " :Not enough parameters\r\n";
}

std::string
    Message::err_already_registred()
{
    return reply_servername_prefix("462") + " :You may not reregister\r\n";
}

std::string
    Message::err_passwd_mismatch()
{
    return reply_servername_prefix("464") + " :Password incorrect\r\n";
}

std::string
    Message::err_channel_is_full(const std::string& channel)
{
    return reply_servername_prefix("471") + " " + channel +
           " :Cannot join channel (+l)\r\n";
}

std::string
    Message::err_unknown_mode(const std::string& flag)
{
    return reply_servername_prefix("472") + " " + flag +
           " :Unknown MODE flag\r\n";
}

std::string
    Message::err_chanoprivs_needed(const std::string& channel)
{
    return reply_servername_prefix("482") + " " + channel +
           " :You're not channel operator\r\n";
}

std::string
    Message::err_u_mode_unknown_flag()
{
    return reply_servername_prefix("501") + " :Unknown MODE flag\r\n";
}

std::string
    Message::err_users_dont_match(const std::string& action)
{
    return reply_servername_prefix("502") + " :Can't " + action +
           " modes for other users\r\n";
}

std::string
    Message::rpl_list(const std::string  channel,
                      const std::string& visible,
                      const std::string  topic)
{
    return reply_servername_prefix("322") + " " + channel + " " + visible +
           " :" + topic + "\r\n";
}

std::string
    Message::rpl_listend()
{
    return reply_servername_prefix("323") + " :End of LIST\r\n";
}

std::string
    Message::rpl_channel_mode_is(const std::string& channel)
{
    return reply_servername_prefix("324") + " " + channel + " +\r\n";
}

std::string
    Message::rpl_notopic(const std::string& channel)
{
    return reply_servername_prefix("331") + " " + channel +
           " :No topic is set\r\n";
}

std::string
    Message::rpl_topic(const std::string& channel, const std::string& topic)
{
    return reply_servername_prefix("332") + " " + channel + " :" + topic +
           "\r\n";
}

std::string
    Message::rpl_inviting(const std::string& nick, const std::string& channel)
{
    return reply_servername_prefix("341") + " " + nick + " " + channel + "\r\n";
}

std::string
    Message::rpl_namreply(const std::string&             channel,
                          std::queue<const std::string>& nick)
{
    std::string message = reply_servername_prefix("353") + " " + channel + " :";

    while (!nick.empty())
    {
        message += nick.front();
        nick.pop();
        if (!nick.empty())
            message += " ";
    }
    return message + "\r\n";
}

std::string
    Message::rpl_endofnames(const std::string& channel)
{
    return reply_servername_prefix("366") + " " + channel +
           " :End of NAMES list\r\n";
}

std::string
    Message::rpl_user_mode_is()
{
    return reply_servername_prefix("221") + " +\r\n";
}

std::string
    Message::rpl_welcome()
{
    return reply_servername_prefix("001") +
           " Welcome to Internet Relay Network\n" + m_client->make_nickmask() +
           "\r\n";
}

std::string
    Message::build_quit_reply(const std::string& reason)
{
    return reply_nickmask_prefix("QUIT") + " :" + reason + "\r\n";
}

std::string
    Message::build_part_reply(const std::string& channel)
{
    std::string param;

    if (m_parameters.size() == 2)
        param = m_parameters[1];
    else
        param = m_client->get_nickname();
    return reply_nickmask_prefix(m_command) + " " + channel + " :" + param +
           "\r\n";
}

std::string
    Message::build_message_reply(const std::string& target)
{
    return reply_nickmask_prefix(m_command) + " " + target + " :" +
           m_parameters[1] + "\r\n";
}

std::string
    Message::build_invite_reply(const std::string& nick,
                                const std::string& channel)
{
    return reply_nickmask_prefix(m_command) + " " + nick + " " + channel +
           "\r\n";
}

std::string
    Message::build_kick_reply(const std::string& channel,
                              const std::string& nick,
                              const std::string& oper_nick)
{
    return reply_nickmask_prefix(m_command) + " " + channel + " " + nick + " " +
           oper_nick + "\r\n";
}

std::string
    Message::build_nick_reply(const std::string& nick)
{
    return reply_nickmask_prefix(m_command) + " " + nick + "\r\n";
}

std::string
    Message::build_join_reply(const std::string& channel)
{
    return reply_nickmask_prefix(m_command) + " " + channel + "\r\n";
}

std::string
    Message::build_topic_reply()
{
    return reply_nickmask_prefix(m_command) + " " + m_parameters[0] + " :" +
           m_parameters[1] + "\r\n";
}

/* message class reply function end */