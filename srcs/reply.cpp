#include "../includes/reply.hpp"
#include "../includes/server.hpp"
#include "../includes/client.hpp"
#include <queue>

std::string
    IRCMessage::m_reply_prefix(Client &client, std::string command)
{
    std::string msg;
    msg = msg + ":" + client.m_get_client_IP() + " " + command + " " + client.m_get_nickname();
    return msg;
}

std::string
    IRCMessage::err_no_such_nick(Client &client, const std::string &nickname)
{
    return m_reply_prefix(client, "401") + " " + nickname + " :No such nick/channel\r\n";
}

std::string
    IRCMessage::err_no_such_server(Client &client, const std::string &server_name)
{
    return m_reply_prefix(client, "402") + " " + server_name + " :No such server\r\n";
}

std::string
	IRCMessage::err_no_such_channel(Client &client, const std::string &channel_name)
{
	return m_reply_prefix(client, "403") + " " + channel_name + " :No such chanel\r\n";
}

std::string
	IRCMessage::err_cannot_send_to_chan(Client &client, const std::string &channel_name)
{
	return m_reply_prefix(client, "404") + " " + channel_name + " :Cannot send to channel\r\n";
}

std::string
	IRCMessage::err_too_many_channels(Client &client, const std::string &channel_name)
{
	return m_reply_prefix(client, "405") + " " + channel_name + " :You have joined too many channels\r\n";
}

std::string
	IRCMessage::err_was_no_such_nick(Client &client, const std::string &nickname)
{
	return m_reply_prefix(client, "406") + " " + nickname + " :There was no such nickname\r\n";
}

std::string
	IRCMessage::err_too_many_targets(Client &client, const std::string &target)
{
	return m_reply_prefix(client, "407") + " " + target + " :Duplicate recipients. No message delivered\r\n";
}

std::string
	IRCMessage::err_no_origin(Client &client)
{
	return m_reply_prefix(client, "409") + " :No origin specified\r\n";
}

std::string
	IRCMessage::err_no_recipient(Client &client, const std::string &command)
{
	return m_reply_prefix(client, "411") + " :No recipient given (" + command + ")\r\n";
}

std::string
	IRCMessage::err_no_text_to_send(Client &client)
{
	return m_reply_prefix(client, "412") + " :No text to send\r\n";
}

std::string
	IRCMessage::err_no_toplevel(Client &client, const std::string &mask)
{
	return m_reply_prefix(client, "413") + " " + mask + " :No toplevel domain specified\r\n";
}

std::string
	IRCMessage::err_wild_toplevel(Client &client, const std::string &mask)
{
	return m_reply_prefix(client, "414") + " " + mask + " :Wildcard in toplevel domain\r\n";
}

std::string
	IRCMessage::err_unknown_command(Client &client, const std::string &command)
{
	return m_reply_prefix(client, "421") + " " + command  + " :Unkown command\r\n";
}

std::string
	IRCMessage::err_no_motd(Client &client)
{
	return m_reply_prefix(client, "422") + " :MOTD Files is missing\r\n";
}

std::string
	IRCMessage::err_no_admin_info(Client &client, const std::string &server)
{
	return m_reply_prefix(client, "423") + " " + server + " :No administrative info available\r\n";
}

std::string
	IRCMessage::err_file_error(Client &client, const std::string &file_op, const std::string &file)
{
	return m_reply_prefix(client, "424") + " :File erroe doing " + file_op + " on " + file + "\r\n";
}

std::string
	IRCMessage::err_no_nickname_given(Client &client)
{
	return m_reply_prefix(client, "431") + " :No nickname given\r\n";
}

std::string
	IRCMessage::err_erroneus_nickname(Client &client, const std::string &nick)
{
	return m_reply_prefix(client, "432") + " " + nick + " :Erroneus nickname\r\n";
}

std::string
	IRCMessage::err_nickname_in_use(Client &client, const std::string &nick)
{
	return m_reply_prefix(client, "433") + " " + nick + " :Nickname is already in use\r\n";
}

std::string
	IRCMessage::err_nick_collision(Client &client, const std::string &nick)
{
	return m_reply_prefix(client, "436") + " " + nick + " :Nickname collision KILL\r\n";
}

std::string
	IRCMessage::err_user_not_in_channel(Client &client, const std::string &nick, const std::string &channel)
{
	return m_reply_prefix(client, "441") + " " + nick + " " + channel + " :They aren't on that channel\r\n";
}

std::string
	IRCMessage::err_not_on_channel(Client &client, const std::string &channel)
{
	return m_reply_prefix(client, "442") + " " + channel + " :You're not on that channel\r\n";
}

std::string
	IRCMessage::err_user_on_channel(Client &client, const std::string &user, const std::string &channel)
{
	return m_reply_prefix(client, "443") + " " + user + " " + channel + " :is already on channel\r\n";
}

std::string
	IRCMessage::err_no_login(Client &client, const std::string &user)
{
	return m_reply_prefix(client, "444") + " " + user + " :User not logged in\r\n";
}

std::string
	IRCMessage::err_summon_disabled(Client &client)
{
	return m_reply_prefix(client, "445") + " :SUMMON has been disabled\r\n";
}

std::string
	IRCMessage::err_users_disabled(Client &client)
{
	return m_reply_prefix(client, "446") + " :USERS has been disabled\r\n";
}

std::string
	IRCMessage::err_not_registered(Client &client)
{
	return m_reply_prefix(client, "451") + " :You have not registerd\r\n";
}

std::string
	IRCMessage::err_need_more_params(Client &client, const std::string& command)
{
	return m_reply_prefix(client, "461") + " " + command + " :Not enough parameters\r\n";
}

std::string
	IRCMessage::err_already_registred(Client &client)
{
	return m_reply_prefix(client, "462") + " :You may not reregister\r\n";
}

std::string
	IRCMessage::err_no_perm_for_host(Client &client)
{
	return m_reply_prefix(client, "463") + " :Your host isn't among the privileged\r\n";
}

std::string
	IRCMessage::err_passwd_mismatch(Client &client)
{
	return m_reply_prefix(client, "464") + " :Password incorrect\r\n";
}

std::string
	IRCMessage::err_youre_banned_creep(Client &client)
{
	return m_reply_prefix(client, "465") + " :You are banned from this server\r\n";
}

std::string
	IRCMessage::err_key_set(Client &client, const std::string &channel)
{
	return m_reply_prefix(client, "467") + " " + channel + " :Channel key already set\r\n";
}

std::string
	IRCMessage::err_channel_is_full(Client &client, const std::string &channel)
{
	return m_reply_prefix(client, "471") + " " + channel + " :Cannot join channel (+l)\r\n";
}

std::string
	IRCMessage::err_unknown_mode(Client &client, const std::string &chr)
{
	return m_reply_prefix(client, "472") + " " + chr + " :is unknown mode char to me\r\n";
} 

std::string
	IRCMessage::err_invite_only_chan(Client &client, const std::string &channel)
{
	return m_reply_prefix(client, "473") + " " + channel + " :Cannot join channel (+i)\r\n";
}

std::string
	IRCMessage::err_banned_from_chan(Client &client, const std::string &channel)
{
	return m_reply_prefix(client, "474") + " " + channel + " :Cannot join channel (+b)\r\n";
}

std::string
	IRCMessage::err_bad_channel_key(Client &client, const std::string &channel)
{
	return m_reply_prefix(client, "475") + " " + channel + " :Cannot join channel (+k)\r\n";
}

std::string
	IRCMessage::err_bad_chan_mask(Client &client, const std::string &channel)
{
	return m_reply_prefix(client, "476") + " " + channel + " :Bad Channel Mask\r\n";
}

std::string
	IRCMessage::err_no_privileges(Client &client)
{
	return m_reply_prefix(client, "481") + " :Permission Denied- You're not an IRC operator\r\n";
}

std::string
	IRCMessage::err_chanoprivs_needed(Client &client, const std::string &channel)
{
	return m_reply_prefix(client, "482") + " " + channel + " :You're not channel operator\r\n";
}

std::string
	IRCMessage::err_cant_kill_server(Client &client)
{
	return m_reply_prefix(client, "483") + " :You cant kill a server\r\n";
}

std::string
	IRCMessage::err_no_oper_host(Client &client)
{
	return m_reply_prefix(client, "491") + " :No O-liens for your host\r\n";
}

std::string
	IRCMessage::err_u_mode_unknown_flag(Client &client)
{
	return m_reply_prefix(client, "501") + " :Unknown MODE flag\r\n";
}

std::string
	IRCMessage::err_users_dont_match(Client &client)
{
	return m_reply_prefix(client, "502") + " :Cant change mode for other users\r\n";
}

std::string
	IRCMessage::rpl_away(Client &client, const std::string& nick, const std::string& away_message)
{
	return m_reply_prefix(client, "301") + " " + nick + " :" + away_message + "\r\n";
}

std::string
	IRCMessage::rpl_liststart(Client &client)
{
	return m_reply_prefix(client, "321") + " Channel :Users name\r\n";
}

std::string
	IRCMessage::rpl_list(Client &client, const std::string channel, const std::string& visible, const std::string topic)
{
	return m_reply_prefix(client, "322") + " " + channel + " " + visible + " :" + topic + "\r\n";
}

std::string
	IRCMessage::rpl_listend(Client &client)
{
	return m_reply_prefix(client, "323") + " :End of /LIST\r\n";
}

std::string
	IRCMessage::rpl_channel_mode_is
(Client &client, const std::string& channel, const std::string& mode, const std::string& mode_params)
{
	return m_reply_prefix(client, "324") + " " + channel + " " + mode + " " + mode_params + "\r\n";
}

std::string
	IRCMessage::rpl_notopic(Client &client, const std::string& channel)
{
	return m_reply_prefix(client, "331") + " " + channel + " :No topic is set\r\n";
}

std::string
	IRCMessage::rpl_topic(Client &client, const std::string& channel, const std::string& topic)
{
	return m_reply_prefix(client, "332") + " " + channel + " :" + topic + "\r\n";
}

std::string
	IRCMessage::rpl_inviting(Client &client, const std::string& channel, const std::string& nick)
{
	return m_reply_prefix(client, "341") + " " + channel + " " + nick + "\r\n";
}

std::string
	IRCMessage::rpl_namreply(Client &client, const std::string& channel,std::queue<const std::string>& nick)
{
	std::string message = m_reply_prefix(client, "353") + " " + channel + " :";

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
	IRCMessage::rpl_endofnames(Client &client, const std::string& channel)
{
	return m_reply_prefix(client, "353") + " " + channel + " :End of /NAMES list\r\n";
}

std::string
	IRCMessage::rpl_banlist(Client &client, const std::string& channel, const std::string& banid)
{
	return m_reply_prefix(client, "367") + " " + channel + " " + banid + "\r\n";
}

std::string
	IRCMessage::rpl_endofbanlist(Client &client, const std::string& channel)
{
	return m_reply_prefix(client, "368") + " " + channel + " :End of channel ban list\r\n";
}

std::string
	IRCMessage::rpl_user_mode_is(Client &client, const std::string& user_mode_string)
{
	return m_reply_prefix(client, "221") + " " + user_mode_string + "\r\n";
}
