#include "../includes/client.hpp"
#include "../includes/ircd.hpp"
#include "../lib/logger.hpp"

/* message class constructor begin */

IRC::~IRC()
{
}

IRC::IRC()
{
}

/* message class constructor end */

/* message class reply function begin */

std::string
    IRC::reply_servername_prefix(std::string numeric_reply)
{
    std::string nick = _client->get_names().nick;

    return (":ft_ircd " + numeric_reply + " " + (nick.empty() ? "*" : nick));
}

std::string
    IRC::reply_nickmask_prefix(std::string command)
{
    std::string msg;
    msg = msg + ":" + _client->make_nickmask() + " " + command;
    return msg;
}

std::string
    IRC::err_no_such_nick(const std::string& nickname)
{
    return reply_servername_prefix("401") + " " + nickname
           + " :No such nick/channel\r\n";
}

std::string
    IRC::err_no_such_channel(const std::string& channel_name)
{
    return reply_servername_prefix("403") + " " + channel_name
           + " :No such channel\r\n";
}

std::string
    IRC::err_too_many_channels(const std::string& channel_name)
{
    return reply_servername_prefix("405") + " " + channel_name
           + " :You have joined too many channels\r\n";
}

std::string
    IRC::err_too_many_targets(const std::string& target)
{
    return reply_servername_prefix("407") + " " + target
           + " :Duplicate recipients. No message delivered\r\n";
}

std::string
    IRC::err_no_recipient()
{
    return reply_servername_prefix("411") + " :No recipient given ("
           + _request->command + ")\r\n";
}

std::string
    IRC::err_no_text_to_send()
{
    return reply_servername_prefix("412") + " :No text to send\r\n";
}

std::string
    IRC::err_unknown_command()
{
    return reply_servername_prefix("421") + " " + _request->command
           + " :Unknown command\r\n";
}

std::string
    IRC::err_file_error(const std::string& file_op, const std::string& file)
{
    return reply_servername_prefix("424") + " :File error doing " + file_op
           + " on " + file + "\r\n";
}

std::string
    IRC::err_no_nickname_given()
{
    return reply_servername_prefix("431") + " :No nickname given\r\n";
}

std::string
    IRC::err_erroneus_nickname(const std::string& nick)
{
    return reply_servername_prefix("432") + " " + nick
           + " :Erroneus nickname\r\n";
}

std::string
    IRC::err_nickname_in_use(const std::string& nick)
{
    return reply_servername_prefix("433") + " " + nick
           + " :Nickname is already in use\r\n";
}

std::string
    IRC::err_user_not_in_channel(const std::string& nick,
                                 const std::string& channel)
{
    return reply_servername_prefix("441") + " " + nick + " " + channel
           + " :They aren't on that channel\r\n";
}

std::string
    IRC::err_not_on_channel(const std::string& channel)
{
    return reply_servername_prefix("442") + " " + channel
           + " :You're not on that channel\r\n";
}

std::string
    IRC::err_user_on_channel(const std::string& user,
                             const std::string& channel)
{
    return reply_servername_prefix("443") + " " + user + " " + channel
           + " :is already on channel\r\n";
}

std::string
    IRC::err_not_registered()
{
    return reply_servername_prefix("451") + " :You have not registered\r\n";
}

std::string
    IRC::err_need_more_params()
{
    return reply_servername_prefix("461") + " " + _request->command
           + " :Not enough parameters\r\n";
}

std::string
    IRC::err_already_registred()
{
    return reply_servername_prefix("462") + " :You may not reregister\r\n";
}

std::string
    IRC::err_passwd_mismatch()
{
    return reply_servername_prefix("464") + " :Password incorrect\r\n";
}

std::string
    IRC::err_channel_is_full(const std::string& channel)
{
    return reply_servername_prefix("471") + " " + channel
           + " :Cannot join channel (+l)\r\n";
}

std::string
    IRC::err_unknown_mode(const std::string& flag)
{
    return reply_servername_prefix("472") + " " + flag
           + " :Unknown MODE flag\r\n";
}

std::string
    IRC::err_chanoprivs_needed(const std::string& channel)
{
    return reply_servername_prefix("482") + " " + channel
           + " :You're not channel operator\r\n";
}

std::string
    IRC::err_u_mode_unknown_flag()
{
    return reply_servername_prefix("501") + " :Unknown MODE flag\r\n";
}

std::string
    IRC::err_users_dont_match(const std::string& action)
{
    return reply_servername_prefix("502") + " :Can't " + action
           + " modes for other users\r\n";
}

std::string
    IRC::rpl_list(const std::string  channel,
                  const std::string& visible,
                  const std::string  topic)
{
    return reply_servername_prefix("322") + " " + channel + " " + visible + " :"
           + topic + "\r\n";
}

std::string
    IRC::rpl_listend()
{
    return reply_servername_prefix("323") + " :End of LIST\r\n";
}

std::string
    IRC::rpl_channel_mode_is(const std::string& channel)
{
    return reply_servername_prefix("324") + " " + channel + " +\r\n";
}

std::string
    IRC::rpl_notopic(const std::string& channel)
{
    return reply_servername_prefix("331") + " " + channel
           + " :No topic is set\r\n";
}

std::string
    IRC::rpl_topic(const std::string& channel, const std::string& topic)
{
    return reply_servername_prefix("332") + " " + channel + " :" + topic
           + "\r\n";
}

std::string
    IRC::rpl_inviting(const std::string& nick, const std::string& channel)
{
    return reply_servername_prefix("341") + " " + nick + " " + channel + "\r\n";
}

std::string
    IRC::rpl_namereply(const std::string&             channel,
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
    IRC::rpl_endofnames(const std::string& channel)
{
    return reply_servername_prefix("366") + " " + channel
           + " :End of NAMES list\r\n";
}

std::string
    IRC::rpl_user_mode_is()
{
    return reply_servername_prefix("221") + " +\r\n";
}

std::string
    IRC::rpl_welcome()
{
    return reply_servername_prefix("001")
           + " Welcome to Internet Relay Network\n" + _client->make_nickmask()
           + "\r\n";
}

std::string
    IRC::cmd_quit_reply(const std::string& reason)
{
    return reply_nickmask_prefix("QUIT") + " :" + reason + "\r\n";
}

std::string
    IRC::cmd_part_reply(const std::string& channel)
{
    std::string param;

    if (_request->parameter.size() == 2)
        param = _request->parameter[1];
    else
        param = _client->get_names().nick;
    return reply_nickmask_prefix(_request->command) + " " + channel + " :"
           + param + "\r\n";
}

std::string
    IRC::cmd_message_reply(const std::string& target)
{
    return reply_nickmask_prefix(_request->command) + " " + target + " :"
           + _request->parameter[1] + "\r\n";
}

std::string
    IRC::cmd_invite_reply(const std::string& nick, const std::string& channel)
{
    return reply_nickmask_prefix(_request->command) + " " + nick + " " + channel
           + "\r\n";
}

std::string
    IRC::cmd_kick_reply(const std::string& channel,
                        const std::string& nick,
                        const std::string& oper_nick)
{
    return reply_nickmask_prefix(_request->command) + " " + channel + " " + nick
           + " " + oper_nick + "\r\n";
}

std::string
    IRC::cmd_nick_reply(const std::string& nick)
{
    return reply_nickmask_prefix(_request->command) + " " + nick + "\r\n";
}

std::string
    IRC::cmd_join_reply(const std::string& channel)
{
    return reply_nickmask_prefix(_request->command) + " " + channel + "\r\n";
}

std::string
    IRC::cmd_topic_reply()
{
    return reply_nickmask_prefix(_request->command) + " "
           + _request->parameter[0] + " :" + _request->parameter[1] + "\r\n";
}

/* message class reply function end */
