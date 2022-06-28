#include "../includes/client.hpp"
#include "../includes/ircd.hpp"

/* message class constructor begin */

IRC::~IRC()
{
}

IRC::IRC() : endl("\r\n")
{
    _command_to_type.insert(std::make_pair("", EMPTY));
    _command_to_type.insert(std::make_pair("PASS", PASS));
    _command_to_type.insert(std::make_pair("NICK", NICK));
    _command_to_type.insert(std::make_pair("USER", USER));
    _command_to_type.insert(std::make_pair("QUIT", QUIT));
    _command_to_type.insert(std::make_pair("JOIN", JOIN));
    _command_to_type.insert(std::make_pair("PART", PART));
    _command_to_type.insert(std::make_pair("TOPIC", TOPIC));
    _command_to_type.insert(std::make_pair("NAMES", NAMES));
    _command_to_type.insert(std::make_pair("LIST", LIST));
    _command_to_type.insert(std::make_pair("INVITE", INVITE));
    _command_to_type.insert(std::make_pair("KICK", KICK));
    _command_to_type.insert(std::make_pair("MODE", MODE));
    _command_to_type.insert(std::make_pair("PRIVMSG", PRIVMSG));
    _command_to_type.insert(std::make_pair("NOTICE", NOTICE));
    _command_to_type.insert(std::make_pair("PING", PING));
    _command_to_type.insert(std::make_pair("PONG", PONG));
}

/* message class constructor end */

/* message class reply function begin */

std::string
    IRC::reply_servername_prefix(const std::string& numeric_reply)
{
    std::string nick = _client->get_names().nick;

    return (":ft_ircd " + numeric_reply + " " + (nick.empty() ? "*" : nick));
}

std::string
    IRC::reply_nickmask_prefix(const std::string& command)
{
    std::string str;
    str = str + ":" + _client->get_nickmask() + " " + command;
    return str;
}

std::string
    IRC::err_no_such_nick(const std::string& nickname)
{
    return reply_servername_prefix("401") + " " + nickname + " :No such nick"
           + IRC::endl;
}

std::string
    IRC::err_no_such_channel(const std::string& channel_name)
{
    return reply_servername_prefix("403") + " " + channel_name
           + " :No such channel" + IRC::endl;
}

std::string
    IRC::err_cannot_send_to_channel(const std::string& channel_name, char mode)
{
    return reply_servername_prefix("404") + " " + channel_name
           + " :Cannot send to channel (+" + mode + ')' + IRC::endl;
}

std::string
    IRC::err_too_many_channels(const std::string& channel_name)
{
    return reply_servername_prefix("405") + " " + channel_name
           + " :You have joined too many channels" + IRC::endl;
}

std::string
    IRC::err_too_many_targets(const std::string& target)
{
    return reply_servername_prefix("407") + " " + target
           + " :Duplicate recipients. No message delivered" + IRC::endl;
}

std::string
    IRC::err_no_recipient()
{
    return reply_servername_prefix("411") + " :No recipient given ("
           + _request->command + ")" + IRC::endl;
}

std::string
    IRC::err_no_text_to_send()
{
    return reply_servername_prefix("412") + " :No text to send" + IRC::endl;
}

std::string
    IRC::err_unknown_command()
{
    return reply_servername_prefix("421") + " " + _request->command
           + " :Unknown command" + IRC::endl;
}

std::string
    IRC::err_file_error(const std::string& file_op, const std::string& file)
{
    return reply_servername_prefix("424") + " :File error doing " + file_op
           + " on " + file + IRC::endl;
}

std::string
    IRC::err_no_nickname_given()
{
    return reply_servername_prefix("431") + " :No nickname given" + IRC::endl;
}

std::string
    IRC::err_erroneus_nickname(const std::string& nick)
{
    return reply_servername_prefix("432") + " " + nick + " :Erroneus nickname"
           + IRC::endl;
}

std::string
    IRC::err_nickname_in_use(const std::string& nick)
{
    return reply_servername_prefix("433") + " " + nick
           + " :Nickname is already in use" + IRC::endl;
}

std::string
    IRC::err_user_not_in_channel(const std::string& nick,
                                 const std::string& channel)
{
    return reply_servername_prefix("441") + " " + nick + " " + channel
           + " :They aren't on that channel" + IRC::endl;
}

std::string
    IRC::err_not_on_channel(const std::string& channel)
{
    return reply_servername_prefix("442") + " " + channel
           + " :You're not on that channel" + IRC::endl;
}

std::string
    IRC::err_user_on_channel(const std::string& user,
                             const std::string& channel)
{
    return reply_servername_prefix("443") + " " + user + " " + channel
           + " :is already on channel" + IRC::endl;
}

std::string
    IRC::err_not_registered()
{
    return reply_servername_prefix("451") + " :You have not registered"
           + IRC::endl;
}

std::string
    IRC::err_need_more_params()
{
    return reply_servername_prefix("461") + " " + _request->command
           + " :Not enough parameters" + IRC::endl;
}

std::string
    IRC::err_already_registred()
{
    return reply_servername_prefix("462") + " :You may not reregister"
           + IRC::endl;
}

std::string
    IRC::err_passwd_mismatch()
{
    return reply_servername_prefix("464") + " :Password incorrect" + IRC::endl;
}

std::string
    IRC::err_channel_is_full(const std::string& channel)
{
    return reply_servername_prefix("471") + " " + channel
           + " :Cannot join channel (+l)" + IRC::endl;
}

std::string
    IRC::err_unknown_mode(char mode)
{
    return reply_servername_prefix("472") + " " + mode + " :Unknown MODE flag"
           + IRC::endl;
}

std::string
    IRC::err_invite_only_channel(const std::string& channel)
{
    return reply_servername_prefix("473") + " " + channel
           + " :Cannot join channel (+i)" + IRC::endl;
}

std::string
    IRC::err_chanoprivs_needed(const std::string& channel)
{
    return reply_servername_prefix("482") + " " + channel
           + " :You're not channel operator" + IRC::endl;
}

std::string
    IRC::err_u_mode_unknown_flag()
{
    return reply_servername_prefix("501") + " :Unknown MODE flag" + IRC::endl;
}

std::string
    IRC::err_users_dont_match(const std::string& action)
{
    return reply_servername_prefix("502") + " :Can't " + action
           + " modes for other users" + IRC::endl;
}

std::string
    IRC::rpl_list(const std::string  channel,
                  const std::string& visible,
                  const std::string  topic)
{
    return reply_servername_prefix("322") + " " + channel + " " + visible + " :"
           + topic + IRC::endl;
}

std::string
    IRC::rpl_listend()
{
    return reply_servername_prefix("323") + " :End of LIST" + IRC::endl;
}

std::string
    IRC::rpl_channel_mode_is(const std::string& channel,
                             const std::string& mode)
{
    return reply_servername_prefix("324") + " " + channel + " +" + mode
           + IRC::endl;
}

std::string
    IRC::rpl_notopic(const std::string& channel)
{
    return reply_servername_prefix("331") + " " + channel + " :No topic is set"
           + IRC::endl;
}

std::string
    IRC::rpl_topic(const std::string& channel, const std::string& topic)
{
    return reply_servername_prefix("332") + " " + channel + " :" + topic
           + IRC::endl;
}

std::string
    IRC::rpl_inviting(const std::string& nick, const std::string& channel)
{
    return reply_servername_prefix("341") + " " + nick + " " + channel
           + IRC::endl;
}

std::string
    IRC::rpl_namereply(const std::string& str)
{
    return reply_servername_prefix("353") + " " + str + IRC::endl;
}

std::string
    IRC::rpl_endofnames(const std::string& channel)
{
    return reply_servername_prefix("366") + " " + channel
           + " :End of NAMES list" + IRC::endl;
}

std::string
    IRC::rpl_user_mode_is()
{
    return reply_servername_prefix("221") + " +" + IRC::endl;
}

std::string
    IRC::rpl_welcome()
{
    return reply_servername_prefix("001")
           + " Welcome to Internet Relay Network\n" + _client->get_nickmask()
           + IRC::endl;
}

std::string
    IRC::cmd_quit_reply(const std::string& reason)
{
    return reply_nickmask_prefix("QUIT") + " :" + reason + IRC::endl;
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
           + param + IRC::endl;
}

std::string
    IRC::cmd_message_reply(const std::string& target)
{
    return reply_nickmask_prefix(_request->command) + " " + target + " :"
           + _request->parameter[1] + IRC::endl;
}

std::string
    IRC::cmd_invite_reply(const std::string& nick, const std::string& channel)
{
    return reply_nickmask_prefix(_request->command) + " " + nick + " " + channel
           + IRC::endl;
}

std::string
    IRC::cmd_kick_reply(const std::string& channel,
                        const std::string& nick,
                        const std::string& oper_nick)
{
    return reply_nickmask_prefix(_request->command) + " " + channel + " " + nick
           + " " + oper_nick + IRC::endl;
}

std::string
    IRC::cmd_nick_reply(const std::string& nick)
{
    return reply_nickmask_prefix(_request->command) + " " + nick + IRC::endl;
}

std::string
    IRC::cmd_join_reply(const std::string& channel)
{
    return reply_nickmask_prefix(_request->command) + " " + channel + IRC::endl;
}

std::string
    IRC::cmd_mode_reply(const std::string& channel, const std::string& mode)
{
    return reply_nickmask_prefix(_request->command) + " " + channel + " " + mode
           + IRC::endl;
}

std::string
    IRC::cmd_topic_reply()
{
    return reply_nickmask_prefix(_request->command) + " "
           + _request->parameter[0] + " :" + _request->parameter[1] + IRC::endl;
}

std::string
    IRC::cmd_pong_reply()
{
    return std::string("PONG ") + NAME_SERVER + IRC::endl; 
}

/* message class reply function end */
