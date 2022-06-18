#include "../includes/command.hpp"
#include "../includes/ft_ircd.hpp"

bool
    Command::m_checker(Client::t_requests& requests)
{
    // type: PRIVMGS
    /////// if (check_error(parameter.empty(), client, msg.err_no_recipient()))
    ///////     return;
    /////// if (check_error(parameter.size() == 1, client,
    /////// msg.err_no_text_to_send()))
    ///////     return;
    // type CHANNEL
    // if (check_error(!_ft_ircd->_map.channel.count(*target_it), client,
    //            msg.err_no_such_channel(*target_it)))
    // continue;
}

void
    Command::m_to_members(Channel*           channel,
                          const std::string& msg,
                          Client*            exclusion)
{
    const Channel::MemberMap&          user_list = channel->get_members();
    Channel::MemberMap::const_iterator user      = user_list.begin();

    Logger().trace() << "send message to channel :" << channel->get_name();
    for (; user != user_list.end(); ++user)
        if (user->first != exclusion)
            m_to_client(*user->first, msg);
}

void
    Command::m_to_members(Client&            client,
                          const std::string& msg,
                          Client*            exclusion)
{
    std::set<Channel*>::iterator it = client.get_joined_list().begin();
    for (; it != client.get_joined_list().end(); ++it)
        m_to_members(*it, msg, exclusion);
}

static void
    split_by_comma(ConstStringVector& splited_params, const std::string& params)
{
    std::istringstream iss(params);

    std::string elem;
    while (std::getline(iss, elem, ','))
        splited_params.push_back(elem);
}

void
    Command::m_privmsg(Client::t_requests& requests)
{
    Client&            client  = *requests.from;
    Client::s_request& request = requests.queue.front();

    // m_checker(PRIVMSG관련);

    ConstStringVector targets;
    split_by_comma(targets, request.parameter.front());

    ConstStringVector::iterator target_it  = targets.begin();
    ConstStringVector::iterator target_ite = targets.end();
    for (; target_it != target_ite; ++target_it)
    {
        if (utils::is_channel_prefix(*target_it))
        {
            // m_checker(채널있냐?)
            m_to_members(_ft_ircd->_map.channel[*target_it],
                         msg.build_message_reply(*target_it), &client);
        }
        else if (_ft_ircd->_map.client.count(*target_it))
            _ft_ircd->m_prepare_to_send(*_ft_ircd->_map.client[*target_it],
                                        msg.build_message_reply(*target_it));
        else if (msg.get_command() != "NOTICE")
            utils::push_message(client, msg.err_no_such_nick(*target_it));
    }
}

Command::Command()
{
    _handler.push_back(&Command::m_empty);
    _handler.push_back(&Command::m_pass);
    _handler.push_back(&Command::m_nick);
    _handler.push_back(&Command::m_user);
    _handler.push_back(&Command::m_quit);
    _handler.push_back(&Command::m_join);
    _handler.push_back(&Command::m_part);
    _handler.push_back(&Command::m_topic);
    _handler.push_back(&Command::m_names);
    _handler.push_back(&Command::m_list);
    _handler.push_back(&Command::m_invite);
    _handler.push_back(&Command::m_kick);
    _handler.push_back(&Command::m_mode);
    _handler.push_back(&Command::m_privmsg);
    _handler.push_back(&Command::m_notice);
    _handler.push_back(&Command::m_unknown);
}
