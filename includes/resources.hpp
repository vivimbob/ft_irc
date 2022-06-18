#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <sys/socket.h>
#define CHANNEL_USER_LIMIT 42

#define CLIENT_CAHNNEL_LIMIT 10

#define HOSTNAME   "localhost"
#define SERVERNAME "ft_ircd"

#define EVENTS_MAX 1024

#define IPV4_MTU_MAX 65535
#define IPV4_MTU_MIN 68

#define NICK_LENGTH_MAX    9
#define CHANNEL_LENGTH_MAX 50
#define CHANNEL_PREFIX     '#'
#define SPECIALCHAR        "[]\\`-^{|}"
#define CHSTRING           " ,\r\n\7"

enum TYPE
{
    ERROR = -1,
    EMPTY,
    PASS,
    NICK,
    USER,
    QUIT,
    JOIN,
    PART,
    TOPIC,
    NAMES,
    LIST,
    INVITE,
    KICK,
    MODE,
    PRIVMSG,
    NOTICE,
    UNKNOWN
};

#define PORT_MAX 65535

#endif /* RESOURCES_HPP */
