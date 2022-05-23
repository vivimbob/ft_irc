#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>
#include <queue>

#define MESSAGE_MEX_LEN 512

class Client;

class IRCMessage
{
private:
    std::string m_message;
    std::string m_prefix;
    std::string m_command;
    std::vector<std::string> m_parameters;
    bool m_valid_message; 
    int m_position;

    IRCMessage(void);
    IRCMessage(const IRCMessage& copy);
    IRCMessage	&operator=(const IRCMessage& other);

    int next_position(void);
    std::string m_reply_prefix(Client &client, std::string command);
public:
    IRCMessage(unsigned int clientfd, const std::string &message);
    ~IRCMessage(void);

    void    parse_message(void);
    const std::string   &get_prefix(void) const;
    const std::string   &get_command(void) const;
    const std::vector<std::string>  &get_params(void) const;
    const bool &is_valid_message(void) const;

    std::string err_no_such_nick(Client &client, const std::string &nickname);
    std::string err_no_such_server(Client &client, const std::string &server_name);
    std::string err_no_such_channel(Client &client, const std::string &channel_name);
    std::string err_cannot_send_to_chan(Client &client, const std::string &channel_name);
    std::string err_too_many_channels(Client &client, const std::string &channel_name);
    std::string err_was_no_such_nick(Client &client, const std::string &nickname);
    std::string err_too_many_targets(Client &client, const std::string &target);
    std::string err_no_origin(Client &client);
    std::string err_no_recipient(Client &client, const std::string &command);
    std::string err_no_text_to_send(Client &client);
    std::string err_no_toplevel(Client &client, const std::string &mask);
    std::string err_wild_toplevel(Client &client, const std::string &mask);
    std::string err_unknown_command(Client &client, const std::string &command);
    std::string err_no_motd(Client &client);
    std::string err_no_admin_info(Client &client, const std::string &server);
    std::string err_file_error(Client &client, const std::string &file_op, const std::string &file);
    std::string err_no_nickname_given(Client &client);
    std::string err_erroneus_nickname(Client &client, const std::string &nick);
    std::string err_nickname_in_use(Client &client, const std::string &nick);
    std::string err_nick_collision(Client &client, const std::string &nick);
    std::string err_user_not_in_channel(Client &client, const std::string &nick, const std::string &channel);
    std::string err_not_on_channel(Client &client, const std::string &channel);
    std::string err_user_on_channel(Client &client, const std::string &user, const std::string &channel);
    std::string err_no_login(Client &client, const std::string &user);
    std::string err_summon_disabled(Client &client);
    std::string err_users_disabled(Client &client);
    std::string err_not_registered(Client &client);
    std::string err_need_more_params(Client &client, const std::string& command);
    std::string err_already_registred(Client &client);
    std::string err_no_perm_for_host(Client &client);
    std::string err_passwd_mismatch(Client &client);
    std::string err_youre_banned_creep(Client &client);
    std::string err_key_set(Client &client, const std::string &channel);
    std::string err_channel_is_full(Client &client, const std::string &channel);
    std::string err_unknown_mode(Client &client, const std::string &chr);
    std::string err_invite_only_chan(Client &client, const std::string &channel);
    std::string err_banned_from_chan(Client &client, const std::string &channel);
    std::string err_bad_channel_key(Client &client, const std::string &channel);
    std::string err_no_privileges(Client &client);
    std::string err_chanoprivs_needed(Client &client, const std::string &channel);
    std::string err_cant_kill_server(Client &client);
    std::string err_no_oper_host(Client &client);
    std::string err_u_mode_unknown_flag(Client &client);
    std::string err_users_dont_match(Client &client);

    std::string rpl_away(Client &client, const std::string& nick, const std::string& away_message);
    std::string rpl_liststart(Client &client);
    std::string rpl_list(Client &client, const std::string channel, const std::string& visible, const std::string topic);
    std::string rpl_listend(Client &client);
    std::string rpl_channel_mode_is(Client &client, const std::string& channel, const std::string& mode, const std::string& mode_params);
    std::string rpl_notopic(Client &client, const std::string& channel);
    std::string rpl_topic(Client &client, const std::string& channel, const std::string& topic);
    std::string rpl_inviting(Client &clientt, const std::string& channel, const std::string& nick);
    std::string rpl_namreply(Client &client, const std::string& channel, std::queue<const std::string>& nick);
    std::string rpl_endofnames(Client &client, const std::string& channel);
    std::string rpl_banlist(Client &client, const std::string& channel, const std::string& banid);
    std::string rpl_endofbanlist(Client &client, const std::string& channel);
    std::string rpl_user_mode_is(Client &client, const std::string& user_mode_string);
};

#endif /* IRCMESSAGE_HPP */
