#include "../includes/utils.hpp"
#include "../includes/channel.hpp"
#include "../includes/client.hpp"
#include "../includes/ft_ircd.hpp"
#include "../lib/logger.hpp"

namespace utils
{

/* utils is_function begin */

static inline bool
    is_special(char c)
{
    return std::memchr(SPECIALCHAR, c, 8);
}

bool
    is_nickname_valid(std::string nick)
{
    if (nick.length() > NICK_LENGTH_MAX)
        return false;
    if (!std::isalpha(nick[0]))
        return false;
    for (size_t index = 1; index < nick.length(); ++index)
        if (!std::isalpha(nick[index]) && !std::isdigit(nick[index]) &&
            !is_special(nick[index]))
            return false;
    return true;
}

bool
    is_channel_prefix(const std::string& chan)
{
    return chan[0] == CHANNEL_PREFIX;
}

bool
    is_channel_name_valid(const std::string& chan)
{
    if (chan.length() > CHANNEL_LENGTH_MAX)
        return false;
    for (size_t index = 0; index < chan.length(); ++index)
        if (std::memchr(CHSTRING, chan[index], 5))
            return false;
    return true;
}

/* utils is_function end */

/* utils parse function begin */

void
    split_by_comma(ConstStringVector& splited_params, const std::string& params)
{
    std::istringstream iss(params);

    std::string elem;
    while (std::getline(iss, elem, ','))
        splited_params.push_back(elem);
}

static const std::string
    masked_nick(bool operater, const std::string& nickname, bool on_channel)
{
    if (on_channel && operater)
        return "@" + nickname;
    else
        return nickname;
}

/* utils parse function end */

/* utils message function begin */

void
    push_message(Client& client, std::string msg)
{
    client.push_message(msg);
    Logger().debug() << msg;
}

void
    send_name_reply(Channel* channel, Client& client, StringBuilder& msg)
{
    std::queue<const std::string> nick_queue;

    Channel::MemberMap::const_iterator user_it = channel->get_members().begin();
    Channel::MemberMap::const_iterator user_ite = channel->get_members().end();

    for (; user_it != user_ite; ++user_it)
        nick_queue.push(masked_nick(channel->is_operator(*user_it->first),
                                    user_it->first->get_names().nick,
                                    client.is_already_joined(channel)));

    if (nick_queue.size())
        client.push_message(
            msg.rpl_namreply("= " + channel->get_name(), nick_queue));
    utils::push_message(client, msg.rpl_endofnames(channel->get_name()));
}

void
    send_topic_reply(Channel* channel, Client& client, StringBuilder& msg)
{
    std::string        reply_msg;
    const std::string& channel_topic = channel->get_topic();

    if (channel_topic.empty())
        reply_msg = msg.rpl_notopic(channel->get_name());
    else
        reply_msg = msg.rpl_topic(channel->get_name(), channel_topic);

    utils::push_message(client, reply_msg);
}

/* utils message function end */

} // namespace utils
