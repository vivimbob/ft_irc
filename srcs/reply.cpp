#include "../includes/reply.hpp"
#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include <queue>

std::string
    IRCMessage::reply_servername_prefix(std::string command)
{
    std::string msg;
    msg = msg + ":" + m_client->m_get_client_IP() + " " + command + " " + m_client->m_get_nickname();
	if (m_client->m_get_nickname().empty())
		msg += "*";
    return msg;
}

std::string
    IRCMessage::err_no_such_nick(const std::string &nickname)
{
    return reply_servername_prefix("401") + " " + nickname + " :No such nick/channel\r\n";
}

std::string
    IRCMessage::err_no_such_server(const std::string &server_name)
{
    return reply_servername_prefix("402") + " " + server_name + " :No such server\r\n";
}

std::string
	IRCMessage::err_no_such_channel(const std::string &channel_name)
{
	return reply_servername_prefix("403") + " " + channel_name + " :No such chanel\r\n";
}

std::string
	IRCMessage::err_cannot_send_to_chan(const std::string &channel_name)
{
	return reply_servername_prefix("404") + " " + channel_name + " :Cannot send to channel\r\n";
}

std::string
	IRCMessage::err_too_many_channels(const std::string &channel_name)
{
	return reply_servername_prefix("405") + " " + channel_name + " :You have joined too many channels\r\n";
}

std::string
	IRCMessage::err_was_no_such_nick(const std::string &nickname)
{
	return reply_servername_prefix("406") + " " + nickname + " :There was no such nickname\r\n";
}

std::string
	IRCMessage::err_too_many_targets(const std::string &target)
{
	return reply_servername_prefix("407") + " " + target + " :Duplicate recipients. No message delivered\r\n";
}

std::string
	IRCMessage::err_no_origin(void)
{
	return reply_servername_prefix("409") + " :No origin specified\r\n";
}

std::string
	IRCMessage::err_no_recipient(void)
{
	return reply_servername_prefix("411") + " :No recipient given (" + m_command + ")\r\n";
}

std::string
	IRCMessage::err_no_text_to_send(void)
{
	return reply_servername_prefix("412") + " :No text to send\r\n";
}

std::string
	IRCMessage::err_no_toplevel(const std::string &mask)
{
	return reply_servername_prefix("413") + " " + mask + " :No toplevel domain specified\r\n";
}

std::string
	IRCMessage::err_wild_toplevel(const std::string &mask)
{
	return reply_servername_prefix("414") + " " + mask + " :Wildcard in toplevel domain\r\n";
}

std::string
	IRCMessage::err_unknown_command(void)
{
	return reply_servername_prefix("421") + " " + m_command  + " :Unkown command\r\n";
}

std::string
	IRCMessage::err_no_motd(void)
{
	return reply_servername_prefix("422") + " :MOTD Files is missing\r\n";
}

std::string
	IRCMessage::err_no_admin_info(const std::string &server)
{
	return reply_servername_prefix("423") + " " + server + " :No administrative info available\r\n";
}

std::string
	IRCMessage::err_file_error(const std::string &file_op, const std::string &file)
{
	return reply_servername_prefix("424") + " :File erroe doing " + file_op + " on " + file + "\r\n";
}

std::string
	IRCMessage::err_no_nickname_given(void)
{
	return reply_servername_prefix("431") + " :No nickname given\r\n";
}

std::string
	IRCMessage::err_erroneus_nickname(const std::string &nick)
{
	return reply_servername_prefix("432") + " " + nick + " :Erroneus nickname\r\n";
}

std::string
	IRCMessage::err_nickname_in_use(const std::string &nick)
{
	return reply_servername_prefix("433") + " " + nick + " :Nickname is already in use\r\n";
}

std::string
	IRCMessage::err_nick_collision(const std::string &nick)
{
	return reply_servername_prefix("436") + " " + nick + " :Nickname collision KILL\r\n";
}

std::string
	IRCMessage::err_user_not_in_channel(const std::string &nick, const std::string &channel)
{
	return reply_servername_prefix("441") + " " + nick + " " + channel + " :They aren't on that channel\r\n";
}

std::string
	IRCMessage::err_not_on_channel(const std::string &channel)
{
	return reply_servername_prefix("442") + " " + channel + " :You're not on that channel\r\n";
}

std::string
	IRCMessage::err_user_on_channel(const std::string &user, const std::string &channel)
{
	return reply_servername_prefix("443") + " " + user + " " + channel + " :is already on channel\r\n";
}

std::string
	IRCMessage::err_no_login(const std::string &user)
{
	return reply_servername_prefix("444") + " " + user + " :User not logged in\r\n";
}

std::string
	IRCMessage::err_summon_disabled(void)
{
	return reply_servername_prefix("445") + " :SUMMON has been disabled\r\n";
}

std::string
	IRCMessage::err_users_disabled(void)
{
	return reply_servername_prefix("446") + " :USERS has been disabled\r\n";
}

std::string
	IRCMessage::err_not_registered(void)
{
	return reply_servername_prefix("451") + " :You have not registerd\r\n";
}

std::string
	IRCMessage::err_need_more_params(void)
{
	return reply_servername_prefix("461") + " " + m_command + " :Not enough parameters\r\n";
}

std::string
	IRCMessage::err_already_registred(void)
{
	return reply_servername_prefix("462") + " :You may not reregister\r\n";
}

std::string
	IRCMessage::err_no_perm_for_host(void)
{
	return reply_servername_prefix("463") + " :Your host isn't among the privileged\r\n";
}

std::string
	IRCMessage::err_passwd_mismatch(void)
{
	return reply_servername_prefix("464") + " :Password incorrect\r\n";
}

std::string
	IRCMessage::err_youre_banned_creep(void)
{
	return reply_servername_prefix("465") + " :You are banned from this server\r\n";
}

std::string
	IRCMessage::err_key_set(const std::string &channel)
{
	return reply_servername_prefix("467") + " " + channel + " :Channel key already set\r\n";
}

std::string
	IRCMessage::err_channel_is_full(const std::string &channel)
{
	return reply_servername_prefix("471") + " " + channel + " :Cannot join channel (+l)\r\n";
}

std::string
	IRCMessage::err_unknown_mode(const char &chr)
{
	return reply_servername_prefix("472") + " " + chr + " :is unknown mode char to me\r\n";
} 

std::string
	IRCMessage::err_invite_only_chan(const std::string &channel)
{
	return reply_servername_prefix("473") + " " + channel + " :Cannot join channel (+i)\r\n";
}

std::string
	IRCMessage::err_banned_from_chan(const std::string &channel)
{
	return reply_servername_prefix("474") + " " + channel + " :Cannot join channel (+b)\r\n";
}

std::string
	IRCMessage::err_bad_channel_key(const std::string &channel)
{
	return reply_servername_prefix("475") + " " + channel + " :Cannot join channel (+k)\r\n";
}

std::string
	IRCMessage::err_bad_chan_mask(const std::string &channel)
{
	return reply_servername_prefix("476") + " " + channel + " :Bad Channel Mask\r\n";
}

std::string
	IRCMessage::err_no_privileges(void)
{
	return reply_servername_prefix("481") + " :Permission Denied- You're not an IRC operator\r\n";
}

std::string
	IRCMessage::err_chanoprivs_needed(const std::string &channel)
{
	return reply_servername_prefix("482") + " " + channel + " :You're not channel operator\r\n";
}

std::string
	IRCMessage::err_cant_kill_server(void)
{
	return reply_servername_prefix("483") + " :You cant kill a server\r\n";
}

std::string
	IRCMessage::err_no_oper_host(void)
{
	return reply_servername_prefix("491") + " :No O-liens for your host\r\n";
}

std::string
	IRCMessage::err_u_mode_unknown_flag(void)
{
	return reply_servername_prefix("501") + " :Unknown MODE flag\r\n";
}

std::string
	IRCMessage::err_users_dont_match(void)
{
	return reply_servername_prefix("502") + " :Cant change mode for other users\r\n";
}

std::string
	IRCMessage::rpl_away(const std::string& nick, const std::string& away_message)
{
	return reply_servername_prefix("301") + " " + nick + " :" + away_message + "\r\n";
}

std::string
	IRCMessage::rpl_liststart(void)
{
	return reply_servername_prefix("321") + " Channel :Users name\r\n";
}

std::string
	IRCMessage::rpl_list(const std::string channel, const std::string& visible, const std::string topic)
{
	return reply_servername_prefix("322") + " " + channel + " " + visible + " :" + topic + "\r\n";
}

std::string
	IRCMessage::rpl_listend(void)
{
	return reply_servername_prefix("323") + " :End of /LIST\r\n";
}

std::string
	IRCMessage::rpl_channel_mode_is
(const std::string& channel, bool toggle, char mode, const std::string& mode_params)
{
	char mode_prefix = toggle == true ? '+' : '-';
	return reply_servername_prefix("324") + " " + channel + " " + mode_prefix + mode + " " + mode_params + "\r\n";
}

std::string
	IRCMessage::rpl_channel_mode_is
(const std::string& channel, const std::string& mode)
{
	return reply_servername_prefix("324") + " " + channel + " " + mode + "\r\n";
}

std::string
	IRCMessage::rpl_notopic(const std::string& channel)
{
	return reply_servername_prefix("331") + " " + channel + " :No topic is set\r\n";
}

std::string
	IRCMessage::rpl_topic(const std::string& channel, const std::string& topic)
{
	return reply_servername_prefix("332") + " " + channel + " :" + topic + "\r\n";
}

std::string
	IRCMessage::rpl_inviting(const std::string& channel, const std::string& nick)
{
	return reply_servername_prefix("341") + " " + channel + " " + nick + "\r\n";
}

std::string
	IRCMessage::rpl_namreply(const std::string& channel, std::queue<const std::string>& nick)
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
	IRCMessage::rpl_endofnames(const std::string& channel)
{
	return reply_servername_prefix("353") + " " + channel + " :End of /NAMES list\r\n";
}

std::string
	IRCMessage::rpl_banlist(const std::string& channel, const std::string& banid)
{
	return reply_servername_prefix("367") + " " + channel + " " + banid + "\r\n";
}

std::string
	IRCMessage::rpl_endofbanlist(const std::string& channel)
{
	return reply_servername_prefix("368") + " " + channel + " :End of channel ban list\r\n";
}

std::string
	IRCMessage::rpl_user_mode_is(const std::string& user_mode_string)
{
	return reply_servername_prefix("221") + " :Your user mode is [" + user_mode_string + "]\r\n";
}

std::string
	IRCMessage::rpl_welcome(void)
{
	return reply_servername_prefix("001") + " Welcome to Interet Relay Network\n"
		+ m_client->make_nickmask() + "\r\n";
}
