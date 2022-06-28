#ifndef IRC_HPP
#define IRC_HPP

#include "channel.hpp"
#include "client.hpp"
#include "log.hpp"
#include "resources.hpp"

class FT_IRCD;

class IRC
{
  public:
    typedef std::map<e_type, std::string>   t_map_irc;
    typedef std::map<std::string, e_type>   t_map_type;
    typedef std::map<std::string, Client*>  t_map_client;
    typedef std::map<std::string, Channel*> t_map_channel;
    typedef t_map_client::const_iterator    t_iter_cl;
    typedef t_map_channel::const_iterator   t_iter_ch;

  private:
    IRC(const IRC&);
    IRC& operator=(const IRC&);

    std::string       reply_servername_prefix(const std::string&);
    std::string       reply_nickmask_prefix(const std::string&);
    const std::string endl;

  public:
    IRC();
    ~IRC();

    std::string err_no_such_nick(const std::string&);
    std::string err_no_such_channel(const std::string&);
    std::string err_too_many_channels(const std::string&);
    std::string err_cannot_send_to_channel(const std::string&, char);
    std::string err_too_many_targets(const std::string&);
    std::string err_no_recipient();
    std::string err_no_text_to_send();
    std::string err_unknown_command();
    std::string err_file_error(const std::string&, const std::string&);
    std::string err_no_nickname_given();
    std::string err_erroneus_nickname(const std::string&);
    std::string err_nickname_in_use(const std::string&);
    std::string err_user_not_in_channel(const std::string&, const std::string&);
    std::string err_not_on_channel(const std::string&);
    std::string err_user_on_channel(const std::string&, const std::string&);
    std::string err_not_registered();
    std::string err_need_more_params();
    std::string err_already_registred();
    std::string err_passwd_mismatch();
    std::string err_channel_is_full(const std::string&);
    std::string err_unknown_mode(char);
    std::string err_invite_only_channel(const std::string&);
    std::string err_chanoprivs_needed(const std::string&);
    std::string err_u_mode_unknown_flag();
    std::string err_users_dont_match(const std::string&);

    std::string
        rpl_list(const std::string, const std::string&, const std::string);
    std::string rpl_listend();
    std::string rpl_channel_mode_is(const std::string&, const std::string&);
    std::string rpl_notopic(const std::string&);
    std::string rpl_topic(const std::string&, const std::string&);
    std::string rpl_inviting(const std::string&, const std::string&);
    std::string rpl_namereply(const std::string&);
    std::string rpl_endofnames(const std::string&);
    std::string rpl_user_mode_is();
    std::string rpl_welcome();

    std::string cmd_quit_reply(const std::string&);
    std::string cmd_part_reply(const std::string&);
    std::string cmd_message_reply(const std::string&);
    std::string cmd_invite_reply(const std::string&, const std::string&);
    std::string cmd_kick_reply(const std::string&,
                               const std::string&,
                               const std::string&);
    std::string cmd_nick_reply(const std::string&);
    std::string cmd_join_reply(const std::string&);
    std::string cmd_mode_reply(const std::string&, const std::string&);
    std::string cmd_topic_reply();
    std::string cmd_pong_reply();

  protected:
    FT_IRCD*             _ft_ircd;
    t_map_irc            _type_to_command;
    t_map_type           _command_to_type;
    Client*              _client;
    Channel*             _channel;
    Client::t_requests*  _requests;
    Client::t_request*   _request;
    Client::t_to_client* _to_client;
    std::string          _password;
    bool                 _ascii[127];
};

#endif /* TEMP_HPP */
