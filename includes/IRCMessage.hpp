#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>

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

    std::string rpl_none(Client &client);
    std::string rpl_userhost(Client &client);
    std::string rpl_ison(Client &client);
    std::string rpl_away(Client &client);
    std::string rpl_unaway(Client &client);
    std::string rpl_nowaway(Client &client);
    std::string rpl_whoisuser(Client &client);
    std::string rpl_whoisserver(Client &client);
    std::string rpl_whoisoperator(Client &client);
    std::string rpl_whoisidle(Client &client);
    std::string rpl_endofwhois(Client &client);
    std::string rpl_whoischannels(Client &client);
    std::string rpl_whowasuser(Client &client);
    std::string rpl_endofwhowas(Client &client);
    std::string rpl_liststart(Client &client);
    std::string rpl_list(Client &client);
    std::string rpl_listend(Client &client);
    std::string rpl_channelmodeis(Client &client);
    std::string rpl_notopic(Client &client);
    std::string rpl_topic(Client &client);
    std::string rpl_inviting(Client &client);
    std::string rpl_summoning(Client &client);
    std::string rpl_version(Client &client);
    std::string rpl_whoreply(Client &client);
    std::string rpl_endofwho(Client &client);
    std::string rpl_namreply(Client &client);
    std::string rpl_endofnames(Client &client);
    std::string rpl_links(Client &client);
    std::string rpl_endoflinks(Client &client);
    std::string rpl_banlist(Client &client);
    std::string rpl_endofbanlist(Client &client);
    std::string rpl_info(Client &client);
    std::string rpl_endofinfo(Client &client);
    std::string rpl_motdstart(Client &client);
    std::string rpl_motd(Client &client);
    std::string rpl_endofmotd(Client &client);
    std::string rpl_youreoper(Client &client);
    std::string rpl_rehashing(Client &client);
    std::string rpl_time(Client &client);
    std::string rpl_usersstart(Client &client);
    std::string rpl_users(Client &client);
    std::string rpl_endofusers(Client &client);
    std::string rpl_nousers(Client &client);
    std::string rpl_tracelink(Client &client);
    std::string rpl_traceconnecting(Client &client);
    std::string rpl_tracehandshake(Client &client);
    std::string rpl_traceunknown(Client &client);
    std::string rpl_traceoperator(Client &client);
    std::string rpl_traceuser(Client &client);
    std::string rpl_traceserver(Client &client);
    std::string rpl_tracenewtype(Client &client);
    std::string rpl_tracelog(Client &client);
    std::string rpl_statslinkinfo(Client &client);
    std::string rpl_statscommands(Client &client);
    std::string rpl_statscline(Client &client);
    std::string rpl_statsnline(Client &client);
    std::string rpl_statsiline(Client &client);
    std::string rpl_statskline(Client &client);
    std::string rpl_statsyline(Client &client);
    std::string rpl_endofstats(Client &client);
    std::string rpl_umodeis(Client &client);
    std::string rpl_statslline(Client &client);
    std::string rpl_statsuptime(Client &client);
    std::string rpl_statsoline(Client &client);
    std::string rpl_statshline(Client &client);
    std::string rpl_luserclient(Client &client);
    std::string rpl_luserop(Client &client);
    std::string rpl_luserunknown(Client &client);
    std::string rpl_luserchannels(Client &client);
    std::string rpl_luserme(Client &client);
    std::string rpl_adminme(Client &client);
    std::string rpl_adminloc1(Client &client);
    std::string rpl_adminloc2(Client &client);
    std::string rpl_adminemail(Client &client);
};

#endif /* IRCMESSAGE_HPP */
