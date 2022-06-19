#ifndef IRC_HPP
#define IRC_HPP

#include "../includes/client.hpp"
#include "channel.hpp"
#include "resources.hpp"
#include <queue>
#include <string>
#include <vector>

class FT_IRCD;

class IRC
{
  public:
    typedef std::map<TYPE, std::string>     IRCMap;
    typedef std::map<std::string, TYPE>     TypeMap;
    typedef std::map<std::string, Client*>  ClientMap;
    typedef std::map<std::string, Channel*> ChannelMap;
    typedef ClientMap::const_iterator       CL_CITER;
    typedef ChannelMap::const_iterator      CH_CITER;

  private:
    IRC(const IRC&);
    IRC& operator=(const IRC& other);

    std::string       reply_servername_prefix(std::string numeric_reply);
    std::string       reply_nickmask_prefix(std::string command);
    const std::string endl;

  public:
    IRC();
    ~IRC();

    std::string err_no_such_nick(const std::string& nickname);
    std::string err_no_such_channel(const std::string& channel_name);
    std::string err_too_many_channels(const std::string& channel_name);
    std::string err_too_many_targets(const std::string& target);
    std::string err_no_recipient();
    std::string err_no_text_to_send();
    std::string err_unknown_command();
    std::string err_file_error(const std::string& file_op,
                               const std::string& file);
    std::string err_no_nickname_given();
    std::string err_erroneus_nickname(const std::string& nick);
    std::string err_nickname_in_use(const std::string& nick);
    std::string err_user_not_in_channel(const std::string& nick,
                                        const std::string& channel);
    std::string err_not_on_channel(const std::string& channel);
    std::string err_user_on_channel(const std::string& user,
                                    const std::string& channel);
    std::string err_not_registered();
    std::string err_need_more_params();
    std::string err_already_registred();
    std::string err_passwd_mismatch();
    std::string err_channel_is_full(const std::string& channel);
    std::string err_unknown_mode(const std::string& flag);
    std::string err_chanoprivs_needed(const std::string& channel);
    std::string err_u_mode_unknown_flag();
    std::string err_users_dont_match(const std::string& action);

    std::string rpl_list(const std::string  channel,
                         const std::string& visible,
                         const std::string  topic);
    std::string rpl_listend();
    std::string rpl_channel_mode_is(const std::string& channel);
    std::string rpl_notopic(const std::string& channel);
    std::string rpl_topic(const std::string& channel, const std::string& topic);
    std::string rpl_inviting(const std::string& nick,
                             const std::string& channel);
    std::string rpl_namereply(const std::string& str);
    std::string rpl_endofnames(const std::string& channel);
    std::string rpl_user_mode_is();
    std::string rpl_welcome();

    std::string cmd_quit_reply(const std::string& reason);
    std::string cmd_part_reply(const std::string& channel);
    std::string cmd_message_reply(const std::string& target);
    std::string cmd_invite_reply(const std::string& nick,
                                 const std::string& channel);
    std::string cmd_kick_reply(const std::string& channel,
                               const std::string& nick,
                               const std::string& oper_nick);
    std::string cmd_nick_reply(const std::string& nick);
    std::string cmd_join_reply(const std::string& channel);
    std::string cmd_topic_reply();

  protected:
    FT_IRCD* _ft_ircd;
    IRCMap   _type_to_command;
    TypeMap  _command_to_type;

    Client*              _client;
    Channel*             _channel;
    Client::t_requests*  _requests;
    Client::t_request*   _request;
    Client::t_to_client* _to_client;

    std::string _password;
};

#endif /* TEMP_HPP */
