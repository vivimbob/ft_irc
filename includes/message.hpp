#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <queue>
#include <string>
#include <vector>

#define MESSAGE_MEX_LEN 512

class Client;

class Message
{
  private:
    std::string m_message;
    std::string m_prefix;
    std::string m_command;
    std::vector<std::string> m_parameters;
    bool m_valid_message;
    size_t m_position;
    Client *m_client;

    Message();
    Message(const Message &copy);
    Message &operator=(const Message &other);

    size_t next_position();
    std::string reply_servername_prefix(std::string command);
    std::string reply_nickmask_prefix(std::string command);

  public:
    Message(Client *client, const std::string &message);
    ~Message();

    void parse_message();
    const std::string &get_message() const;
    const std::string &get_prefix() const;
    const std::string &get_command() const;
    const std::vector<std::string> &get_params() const;
    const bool &is_valid_message() const;

    std::string err_no_such_nick(const std::string &nickname);
    std::string err_no_such_server(const std::string &server_name);
    std::string err_no_such_channel(const std::string &channel_name);
    std::string err_cannot_send_to_chan(const std::string &channel_name);
    std::string err_too_many_channels(const std::string &channel_name);
    std::string err_was_no_such_nick(const std::string &nickname);
    std::string err_too_many_targets(const std::string &target);
    std::string err_no_origin();
    std::string err_no_recipient();
    std::string err_no_text_to_send();
    std::string err_no_toplevel(const std::string &mask);
    std::string err_wild_toplevel(const std::string &mask);
    std::string err_unknown_command();
    std::string err_no_motd();
    std::string err_no_admin_info(const std::string &server);
    std::string err_file_error(const std::string &file_op,
                               const std::string &file);
    std::string err_no_nickname_given();
    std::string err_erroneus_nickname(const std::string &nick);
    std::string err_nickname_in_use(const std::string &nick);
    std::string err_nick_collision(const std::string &nick);
    std::string err_user_not_in_channel(const std::string &nick,
                                        const std::string &channel);
    std::string err_not_on_channel(const std::string &channel);
    std::string err_user_on_channel(const std::string &user,
                                    const std::string &channel);
    std::string err_no_login(const std::string &user);
    std::string err_summon_disabled();
    std::string err_users_disabled();
    std::string err_not_registered();
    std::string err_need_more_params();
    std::string err_already_registred();
    std::string err_no_perm_for_host();
    std::string err_passwd_mismatch();
    std::string err_youre_banned_creep();
    std::string err_key_set(const std::string &channel);
    std::string err_channel_is_full(const std::string &channel);
    std::string err_unknown_mode(const char &chr);
    std::string err_invite_only_chan(const std::string &channel);
    std::string err_banned_from_chan(const std::string &channel);
    std::string err_bad_channel_key(const std::string &channel);
    std::string err_bad_chan_mask(const std::string &channel);
    std::string err_no_privileges();
    std::string err_chanoprivs_needed(const std::string &channel);
    std::string err_cant_kill_server();
    std::string err_no_oper_host();
    std::string err_u_mode_unknown_flag();
    std::string err_users_dont_match();

    std::string rpl_away(const std::string &nick,
                         const std::string &away_message);
    std::string rpl_liststart();
    std::string rpl_list(const std::string channel,
                         const std::string &visible,
                         const std::string topic);
    std::string rpl_listend();
    std::string
        rpl_channel_mode_is(const std::string &channel,
                            bool toggle,
                            char mode,
                            const std::string &mode_params = std::string());
    std::string rpl_channel_mode_is(const std::string &channel,
                                    const std::string &mode);
    std::string rpl_notopic(const std::string &channel);
    std::string rpl_topic(const std::string &channel, const std::string &topic);
    std::string rpl_inviting(const std::string &nick,
                             const std::string &channel);
    std::string rpl_namreply(const std::string &channel,
                             std::queue<const std::string> &nick);
    std::string rpl_endofnames(const std::string &channel);
    std::string rpl_banlist(const std::string &channel,
                            const std::string &banid);
    std::string rpl_endofbanlist(const std::string &channel);
    std::string rpl_user_mode_is(const std::string &user_mode_string);
    std::string rpl_welcome();

    std::string build_quit_reply();
    std::string build_part_reply(const std::string &channel);
    std::string build_message_reply(const std::string &target);
    std::string build_invite_reply(const std::string &nick,
                                   const std::string &channel);
    std::string build_kick_reply(const std::string &chanel,
                                 const std::string &nick);
};

#endif /* MESSAGE_HPP */
