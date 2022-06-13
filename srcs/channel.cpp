#include "../includes/channel.hpp"
#include "../includes/logger.hpp"
#include <utility>

Channel::Channel(const std::string& name, const std::string& key)
    : m_channel_name(name),
      m_channel_init_time(std::time(NULL)),
      m_key(key)
{
    m_mode.private_channel    = false;
    m_mode.secret             = false;
    m_mode.invite             = false;
    m_mode.topic              = false;
    m_mode.no_message         = false;
    m_mode.moderate           = false;
    m_mode.limit              = false;
    m_mode.key                = key.empty() ? false : true;
    m_user_limit              = 42;
    m_mode_string_need_update = true;
}

Channel::~Channel()
{
}

const std::string&
    Channel::get_channel_name() const
{
    return m_channel_name;
}

const std::string&
    Channel::get_channel_topic() const
{
    return m_channel_topic;
}

std::string
    Channel::get_channel_mode(Client* client)
{
    if (m_mode_string_need_update)
    {
        m_mode_string.clear();
        m_mode_string.push_back('+');
        if (m_mode.private_channel)
            m_mode_string.push_back('p');
        if (m_mode.secret)
            m_mode_string.push_back('s');
        if (m_mode.invite)
            m_mode_string.push_back('i');
        if (m_mode.topic)
            m_mode_string.push_back('t');
        if (m_mode.no_message)
            m_mode_string.push_back('n');
        if (m_mode.moderate)
            m_mode_string.push_back('m');
        if (m_mode.key)
            m_mode_string.push_back('k');
        if (m_mode.limit)
            m_mode_string.push_back('l');
        m_mode_string_need_update = false;
    }
    MemberShip& member = m_member_list.find(client)->second;
    std::string member_mode_string;
    if (member.mode.operater)
        member_mode_string.push_back('o');
    if (member.mode.voice)
        member_mode_string.push_back('v');
    return m_mode_string + member_mode_string;
}

const std::string&
    Channel::get_key() const
{
    return m_key;
}

const size_t&
    Channel::get_user_limit() const
{
    return m_user_limit;
}

const std::map<Client*, MemberShip>&
    Channel::get_user_list()
{
    return m_member_list;
}

void
    Channel::set_channel_name(const std::string& name)
{
    this->m_channel_name = name;
}

void
    Channel::set_channel_topic(const std::string& topic)
{
    this->m_channel_topic = topic;
}

void
    Channel::set_private_flag(bool toggle)
{
    if (m_mode.private_channel == toggle)
        return;
    m_mode.private_channel    = toggle;
    m_mode_string_need_update = true;
    if (m_mode.private_channel == true && m_mode.secret == true)
        m_mode.secret = false;
}

void
    Channel::set_secret_flag(bool toggle)
{
    if (m_mode.secret == toggle)
        return;
    m_mode.secret             = toggle;
    m_mode_string_need_update = true;
    if (m_mode.private_channel == true && m_mode.secret == true)
        m_mode.private_channel = false;
}

void
    Channel::set_invite_flag(bool toggle)
{
    if (m_mode.invite == toggle)
        return;
    m_mode.invite             = toggle;
    m_mode_string_need_update = true;
}

void
    Channel::set_topic_flag(bool toggle)
{
    if (m_mode.topic == toggle)
        return;
    m_mode.topic              = toggle;
    m_mode_string_need_update = true;
}

void
    Channel::set_no_messages_flag(bool toggle)
{
    if (m_mode.no_message == toggle)
        return;
    m_mode.no_message         = toggle;
    m_mode_string_need_update = true;
}

void
    Channel::set_moderate_flag(bool toggle)
{
    if (m_mode.moderate == toggle)
        return;
    m_mode.moderate           = toggle;
    m_mode_string_need_update = true;
}

void
    Channel::set_key_flag(bool toggle, std::string key)
{
    if (toggle == true)
        m_key = key;
    if (m_mode.key == toggle)
        return;
    m_mode.key                = toggle;
    m_mode_string_need_update = true;
}

void
    Channel::set_limit(bool toggle, size_t limit)
{
    m_user_limit = limit;
    if (m_mode.limit == toggle)
        return;
    m_mode.limit              = toggle;
    m_mode_string_need_update = true;
}

void
    Channel::set_operator_flag(bool toggle, Client* client)
{
    m_member_list.find(client)->second.mode.operater = toggle;
}

void
    Channel::set_voice_flag(bool toggle, Client* client)
{
    m_member_list.find(client)->second.mode.voice = toggle;
}

bool
    Channel::is_empty()
{
    return m_member_list.empty();
}

bool
    Channel::is_full()
{
    return m_member_list.size() >= m_user_limit;
}

bool
    Channel::is_operator(Client& client)
{
    return m_member_list.find(&client)->second.mode.operater;
}

bool
    Channel::is_voice_mode(Client& client)
{
    return m_member_list.find(&client)->second.mode.voice;
}

bool
    Channel::is_user_on_channel(Client* client)
{
    return m_member_list.count(client);
}

bool
    Channel::is_user_on_invitation_list(Client* client)
{
    return m_invitation_list.count(client);
}

bool
    Channel::is_protected_topic_mode()
{
    return m_mode.topic;
}

bool
    Channel::is_limit_mode()
{
    return m_mode.limit;
}

bool
    Channel::is_invite_only_mode()
{
    return m_mode.invite;
}

bool
    Channel::is_key_mode()
{
    return m_mode.key;
}

bool
    Channel::is_private_mode()
{
    return m_mode.private_channel;
}

bool
    Channel::is_secret_mode()
{
    return m_mode.secret;
}

void
    Channel::add_user(Client& client)
{
    m_member_list.insert(std::make_pair(&client, MemberShip(&client, this)));
}

void
    Channel::delete_user(Client& client)
{
    m_member_list.erase(&client);
}

void
    Channel::add_user_invitation_list(Client& client)
{
    m_invitation_list.insert(&client);
}

void
    Channel::delete_user_invitation_list(Client& client)
{
    m_invitation_list.erase(&client);
}
