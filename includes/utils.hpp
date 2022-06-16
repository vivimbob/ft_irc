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

typedef std::vector<const std::string> ConstStringVector;

namespace utils
{

bool is_nickname_valid(std::string nick);
bool is_channel_prefix(const std::string& chan);
bool is_channel_name_valid(const std::string& chan);
void split_by_comma(ConstStringVector& splited_params,
                    const std::string& params);
void send_name_reply(Channel* channel, Client& client, Message& msg);
void send_topic_reply(Channel* channel, Client& client, Message& msg);
void push_message(Client& client, std::string msg);
} // namespace utils

#endif /* UTILS_HPP */
