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
    size_t m_position;
	Client *m_client;

    IRCMessage(void);
    IRCMessage(const IRCMessage& copy);
    IRCMessage	&operator=(const IRCMessage& other);

    size_t next_position(void);
    std::string m_reply_prefix(std::string command);
public:
    IRCMessage(Client *client, const std::string &message);
    ~IRCMessage(void);

    void    parse_message(void);
    const std::string   &get_message(void) const;
    const std::string   &get_prefix(void) const;
    const std::string   &get_command(void) const;
    const std::vector<std::string>  &get_params(void) const;
    const bool &is_valid_message(void) const;

    std::string err_no_such_nick(const std::string &nickname);
    std::string err_no_such_server(const std::string &server_name);
    std::string err_no_such_channel(const std::string &channel_name);
    std::string err_cannot_send_to_chan(const std::string &channel_name);
    std::string err_too_many_channels(const std::string &channel_name);
    std::string err_was_no_such_nick(const std::string &nickname);
    std::string err_too_many_targets(const std::string &target);
    std::string err_no_origin(void);
    std::string err_no_recipient(void);
    std::string err_no_text_to_send(void);
    std::string err_no_toplevel(const std::string &mask);
    std::string err_wild_toplevel(const std::string &mask);
    std::string err_unknown_command(void);
    std::string err_no_motd(void);
    std::string err_no_admin_info(const std::string &server);
    std::string err_file_error(const std::string &file_op, const std::string &file);
    std::string err_no_nickname_given(void);
    std::string err_erroneus_nickname(const std::string &nick);
    std::string err_nickname_in_use(const std::string &nick);
    std::string err_nick_collision(const std::string &nick);
    std::string err_user_not_in_channel(const std::string &nick, const std::string &channel);
    std::string err_not_on_channel(const std::string &channel);
    std::string err_user_on_channel(const std::string &user, const std::string &channel);
    std::string err_no_login(const std::string &user);
    std::string err_summon_disabled(void);
    std::string err_users_disabled(void);
    std::string err_not_registered(void);
    std::string err_need_more_params(void);
    std::string err_already_registred(void);
    std::string err_no_perm_for_host(void);
    std::string err_passwd_mismatch(void);
    std::string err_youre_banned_creep(void);
    std::string err_key_set(const std::string &channel);
    std::string err_channel_is_full(const std::string &channel);
    std::string err_unknown_mode(const char &chr);
    std::string err_invite_only_chan(const std::string &channel);
    std::string err_banned_from_chan(const std::string &channel);
    std::string err_bad_channel_key(const std::string &channel);
    std::string err_bad_chan_mask(const std::string &channel);
    std::string err_no_privileges(void);
    std::string err_chanoprivs_needed(const std::string &channel);
    std::string err_cant_kill_server(void);
    std::string err_no_oper_host(void);
    std::string err_u_mode_unknown_flag(void);
    std::string err_users_dont_match(void);

    std::string rpl_away(const std::string& nick, const std::string& away_message);
    std::string rpl_liststart(void);
    std::string rpl_list(const std::string channel, const std::string& visible, const std::string topic);
    std::string rpl_listend(void);
    std::string rpl_channel_mode_is(const std::string& channel, bool toggle, char mode, const std::string& mode_params = std::string());
    std::string rpl_channel_mode_is(const std::string& channel, const std::string& mode);
    std::string rpl_notopic(const std::string& channel);
    std::string rpl_topic(const std::string& channel, const std::string& topic);
    std::string rpl_inviting(const std::string& channel, const std::string& nick);
    std::string rpl_namreply(const std::string& channel, std::queue<const std::string>& nick);
    std::string rpl_endofnames(const std::string& channel);
    std::string rpl_banlist(const std::string& channel, const std::string& banid);
    std::string rpl_endofbanlist(const std::string& channel);
    std::string rpl_user_mode_is(const std::string& user_mode_string);
    std::string rpl_welcome(void);

    std::string build_message(void);
};

#endif /* IRCMESSAGE_HPP */
