#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

#define NAME_HOST   "localhost"
#define NAME_SERVER "ft_ircd"
#define NAME_BOT    "bot"

#define PORT_MAX     65535
#define IPV4_MTU_MAX 65535
#define IPV4_MTU_MIN 68

#define CHANNEL_USER_MAX   42
#define CHANNEL_LENGTH_MAX 50
#define CLIENT_CAHNNEL_MAX 10
#define NICK_LENGTH_MAX    9
#define EVENTS_MAX         1024
#define SPECIALCHAR        "[]\\`-^{|}"
#define CHSTRING           " ,\r\n\7"
#define CONNECTION         4
#define DELIMITER          ','

enum e_exit
{
    SUCCESS,
    FAILURE,
};

enum e_result
{
    FATAL = -1,
    ERROR = 0,
    FALSE = 0,
    TRUE  = 1,
    OK    = 1,
};

enum e_type
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
	PING,
	PONG,
    UNKNOWN,
    UNREGISTERED,
    CHANNEL_NAME,
    NOMSG,
    CHANNEL_PREFIX = '#'
};

enum e_bot
{
    HELP,
    TIME,
    COIN,
    NONE
};

enum e_status
{
    PASSWORD   = 0b001,
    NICKNAME   = 0b010,
    USERNAME   = 0b100,
    REGISTERED = 0b111
};

enum e_phase
{
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE
};

#endif /* RESOURCES_HPP */
