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

void
    split_by_comma(ConstStringVector& splited_params,
                   const std::string& params);
void
    send_name_reply(Channel* channel, Client& client, Message& msg);
void
    send_topic_reply(Channel* channel, Client& client, Message& msg);

} // namespace utils

namespace event
{

} // namespace event

#endif /* UTILS_HPP */
