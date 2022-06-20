
#include <map>
#include <netinet/in.h>
#include <queue>
#include <string>
#include <vector>

class Client;
class Channel;
class MemberShip;

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
    UNREGISTERED
};

typedef std::map<std::string, Client*>  ClientMap;
typedef std::map<std::string, Channel*> ChannelMap;
typedef std::map<Client*, MemberShip>   MemberMap;

typedef struct s_map
{
    ClientMap  client;
    ChannelMap channel;
} t_map;

typedef struct s_names
{
    std::string nick;
    std::string user;
    std::string host;
    std::string server;
    std::string real;
} t_names;

typedef struct s_request
{
    std::string              command;
    std::vector<std::string> parameter;
    TYPE                     type;
    s_request(std::string line, TYPE type) : command(line), type(type){};
} t_request;

typedef struct s_requests
{
    Client*               from;
    std::queue<t_request> queue;
} t_requests;

typedef struct s_to_client
{
    int                     offset;
    std::queue<std::string> queue;
} t_to_client;

typedef struct s_buffers
{
    int         offset;
    std::string buffer;
    t_requests  requests;
    t_to_client to_client;
} t_buffers;

typedef struct s_target
{
    Client*    client;
    t_request* request;
} t_target;

typedef struct s_socket
{
    int         fd;
    sockaddr_in addr;
    socklen_t   len;
} t_socket;
