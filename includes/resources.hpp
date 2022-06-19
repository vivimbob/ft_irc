#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <cstdlib>
#include <sys/socket.h>
#include <type_traits>

#define HOSTNAME   "localhost"
#define SERVERNAME "ft_ircd"

#define CHANNEL_USER_LIMIT   42
#define CLIENT_CAHNNEL_LIMIT 10
#define EVENTS_MAX           1024

#define PORT_MAX     65535
#define IPV4_MTU_MAX 65535
#define IPV4_MTU_MIN 68

#define NICK_LENGTH_MAX    9
#define CHANNEL_LENGTH_MAX 50
#define SPECIALCHAR        "[]\\`-^{|}"
#define CHSTRING           " ,\r\n\7"
#define CONNECTION         4

enum EXIT
{
    SUCCESS,
    FAILURE,
};

enum RESULT
{
    FATAL = -1,
    ERROR = 0,
    FALSE = 0,
    TRUE  = 1,
    OK    = 1,
};

enum TYPE
{
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
    UNKNOWN,
    UNREGISTERED,
    CHANNEL_NAME,
    CHANNEL_PREFIX = '#'
};

enum STATUS
{
    PASSWORD   = 0b001,
    NICKNAME   = 0b010,
    USERNAME   = 0b100,
    REGISTERED = 0b111
};

enum PHASE
{
    ONE,
    TWO,
    THREE
};

#endif /* RESOURCES_HPP */
