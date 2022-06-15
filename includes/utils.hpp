#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class Channel;
class Client;
class Message;

namespace utils
{
struct ClientInfo
{
    std::string nickname;
    std::string username;
    std::string hostname;
    std::string servername;
};

bool       is_nickname_valid(std::string nick);
bool       is_channel_prefix(const std::string& chan);
bool       is_channel_name_valid(const std::string& chan);
void       split_by_comma(std::vector<const std::string>& splited_params,
                          const std::string&              params);
ClientInfo parse_client_info(std::string client_str);
void       send_name_reply(Channel* channel, Client& client, Message& msg);
void       send_topic_reply(Channel* channel, Client& client, Message& msg);
void       push_message(Client& client, std::string msg);
} // namespace utils

#endif /* UTILS_HPP */
