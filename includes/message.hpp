#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <queue>
#include <string>
#include <vector>

class Client;

class Message
{
  private:
    std::string              m_message;
    std::string              m_prefix;
    std::string              m_command;
    std::vector<std::string> m_parameters;
    bool                     m_valid_message;
    size_t                   m_position;
    Client*                  _from;

    Message();
    Message(const Message& copy);
    Message& operator=(const Message& other);

    size_t      next_position();
    std::string reply_servername_prefix(std::string numeric_reply);
    std::string reply_nickmask_prefix(std::string command);

  public:
    Message(Client* client, const std::string& message);
    ~Message();

    void                            parse_message();
    const std::string&              get_message() const;
    const std::string&              get_prefix() const;
    const std::string&              get_command() const;
    const std::vector<std::string>& get_params() const;
    const bool&                     is_valid_message() const;
    Client&                         get_from();

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
    std::string rpl_namreply(const std::string&             channel,
                             std::queue<const std::string>& nick);
    std::string rpl_endofnames(const std::string& channel);
    std::string rpl_user_mode_is();
    std::string rpl_welcome();

    std::string build_quit_reply(const std::string& reason);
    std::string build_part_reply(const std::string& channel);
    std::string build_message_reply(const std::string& target);
    std::string build_invite_reply(const std::string& nick,
                                   const std::string& channel);
    std::string build_kick_reply(const std::string& channel,
                                 const std::string& nick,
                                 const std::string& oper_nick);
    std::string build_nick_reply(const std::string& nick);
    std::string build_join_reply(const std::string& channel);
    std::string build_topic_reply();
};

#endif /* MESSAGE_HPP */
