#include "../includes/command.hpp"

void
    Command::m_privmsg(Client::t_requests& requests)
{
    Client&            client  = *requests.from;
    Client::s_request& request = requests.queue.front();

    const std::vector<std::string>& parameter = msg.get_params();

    if (check_error(parameter.empty(), client, msg.err_no_recipient()))
        return;
    if (check_error(parameter.size() == 1, client, msg.err_no_text_to_send()))
        return;

    ConstStringVector target_list;
    utils::split_by_comma(target_list, parameter[0]);

    ConstStringVector::iterator target_it  = target_list.begin();
    ConstStringVector::iterator target_ite = target_list.end();
    for (; target_it != target_ite; ++target_it)
    {
        if (utils::is_channel_prefix(*target_it))
        {
            if (check_error(!_ft_ircd->_map.channel.count(*target_it), client,
                            msg.err_no_such_channel(*target_it)))
                continue;
            _ft_ircd->m_send_to_channel(_ft_ircd->_map.channel[*target_it],
                                        msg.build_message_reply(*target_it),
                                        &client);
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
}
