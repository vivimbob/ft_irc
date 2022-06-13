#include "../../includes/logger.hpp"
#include "../../includes/server.hpp"

void
    Server::m_process_channel_mode_command(Client            &client,
                                           Message           &msg,
                                           const std::string &channel_name)
{
    if (!m_channel_map.count(channel_name))
    {
        client.push_message(msg.err_no_such_channel(channel_name),
                            Logger::Debug);
        return;
    }
    Channel *channel = m_channel_map.at(channel_name);
    if (msg.get_params().size() < 2)
    {
        client.push_message(
            msg.rpl_channel_mode_is(channel_name,
                                    channel->get_channel_mode(&client)),
            Logger::Debug);
        return;
    }
    if (!channel->is_operator(client))
    {
        client.push_message(msg.err_chanoprivs_needed(channel_name),
                            Logger::Debug);
        return;
    }

    std::vector<std::string>::const_iterator parameter =
        msg.get_params().begin() + 1;
    std::vector<std::string>::const_iterator parameter_end =
        msg.get_params().end();
    std::string::const_iterator mode = parameter->begin();
    std::string::const_iterator mode_end = parameter->end();
    int                         parameter_need_mode_count = 0;

    bool toggle = true;
    while (mode != mode_end && parameter_need_mode_count < 3)
    {
        switch (*mode)
        {
        case '+':
            toggle = true;
            break;
        case '-':
            toggle = false;
            break;
        case 'p':
            channel->set_private_flag(toggle);
            client.push_message(
                msg.rpl_channel_mode_is(channel_name, toggle, *mode),
                Logger::Debug);
            break;
        case 's':
            channel->set_secret_flag(toggle);
            client.push_message(
                msg.rpl_channel_mode_is(channel_name, toggle, *mode),
                Logger::Debug);
            break;
        case 'i':
            channel->set_invite_flag(toggle);
            client.push_message(
                msg.rpl_channel_mode_is(channel_name, toggle, *mode),
                Logger::Debug);
            break;
        case 't':
            channel->set_topic_flag(toggle);
            client.push_message(
                msg.rpl_channel_mode_is(channel_name, toggle, *mode),
                Logger::Debug);
            break;
        case 'n':
            channel->set_no_messages_flag(toggle);
            client.push_message(
                msg.rpl_channel_mode_is(channel_name, toggle, *mode),
                Logger::Debug);
            break;
        case 'm':
            channel->set_moderate_flag(toggle);
            client.push_message(
                msg.rpl_channel_mode_is(channel_name, toggle, *mode),
                Logger::Debug);
            break;
        case 'k':
            if (toggle == true)
            {
                ++parameter_need_mode_count;
                if (++parameter == parameter_end)
                {
                    client.push_message(msg.err_need_more_params(),
                                        Logger::Debug);
                    break;
                }
                channel->set_key_flag(true, *parameter);
                client.push_message(msg.rpl_channel_mode_is(channel_name,
                                                            toggle, *mode,
                                                            *parameter),
                                    Logger::Debug);
            }
            else
            {
                channel->set_key_flag(false);
                client.push_message(
                    msg.rpl_channel_mode_is(channel_name, toggle, *mode),
                    Logger::Debug);
            }
            break;
        case 'l':
            ++parameter_need_mode_count;
            if (++parameter == parameter_end)
            {
                client.push_message(msg.err_need_more_params(), Logger::Debug);
                break;
            }
            channel->set_limit(toggle, atoi(parameter.base()->data()));
            client.push_message(msg.rpl_channel_mode_is(channel_name, toggle,
                                                        *mode, *parameter),
                                Logger::Debug);
            break;
        case 'o':
            ++parameter_need_mode_count;
            if (++parameter == parameter_end)
            {
                client.push_message(msg.err_need_more_params(), Logger::Debug);
                break;
            }
            {
                ClientMap::iterator iterator_user =
                    m_client_map.find(*parameter.base());
                if (iterator_user == m_client_map.end())
                {
                    client.push_message(
                        msg.err_no_such_nick(iterator_user->first),
                        Logger::Debug);
                    break; // no such nick;
                }
                Client *user = iterator_user->second;
                if (!channel->is_user_on_channel(user))
                {
                    client.push_message(msg.err_not_on_channel(channel_name),
                                        Logger::Debug);
                    break; // no to channel
                }
                channel->set_operator_flag(toggle, user);
                client.push_message(msg.rpl_channel_mode_is(channel_name,
                                                            toggle, *mode,
                                                            *parameter),
                                    Logger::Debug);
            }
            break;
        case 'v':
            ++parameter_need_mode_count;
            if (++parameter == parameter_end)
            {
                client.push_message(msg.err_need_more_params(), Logger::Debug);
                break;
            }
            {
                ClientMap::iterator iterator_user =
                    m_client_map.find(*parameter.base());
                if (iterator_user == m_client_map.end())
                {
                    client.push_message(
                        msg.err_no_such_nick(iterator_user->first),
                        Logger::Debug);
                    break; // no such nick;
                }
                Client *user = iterator_user->second;
                if (!channel->is_user_on_channel(user))
                {
                    client.push_message(msg.err_not_on_channel(channel_name),
                                        Logger::Debug);
                    break; // no to channel
                }
                channel->set_voice_flag(toggle, user);
                client.push_message(msg.rpl_channel_mode_is(channel_name,
                                                            toggle, *mode,
                                                            *parameter),
                                    Logger::Debug);
            }
            break;
        default:
            client.push_message(msg.err_unknown_mode(*mode), Logger::Debug);
        }
        ++mode;
    }
}

void
    Server::m_process_user_mode_command(Client            &client,
                                        Message           &msg,
                                        const std::string &nickname)
{
    if (nickname != client.get_nickname())
    {
        client.push_message(msg.err_users_dont_match(), Logger::Debug);
        return;
    }

    if (msg.get_params().size() == 1)
    {
        client.push_message(msg.rpl_user_mode_is(client.get_usermode()),
                            Logger::Debug);
        return;
    }

    std::string::const_iterator it = msg.get_params()[1].begin();
    std::string::const_iterator ite = msg.get_params()[1].end();

    bool toggle = true;
    while (it != ite)
    {
        switch (*it)
        {
        case '+':
            toggle = true;
            break;
        case '-':
            toggle = false;
            break;
        case 'i':
            client.set_invisible_flag(toggle);
            break;
        case 'o':
            if (toggle == false)
                client.set_operator_flag(toggle);
            break;
        case 's':
            client.set_server_notice_flag(toggle);
            break;
        case 'w':
            client.set_wallops_flag(toggle);
            break;
        default:
            client.push_message(msg.err_u_mode_unknown_flag(), Logger::Debug);
        }
        ++it;
    }
    client.push_message(msg.rpl_user_mode_is(client.get_usermode()),
                        Logger::Debug);
}

void
    Server::m_process_mode_command(Client &client, Message &msg)
{
    if (msg.get_params().empty())
    {
        client.push_message(msg.err_need_more_params(), Logger::Debug);
        return;
    }

    const std::string &target = msg.get_params()[0];

    if (utils::is_channel_prefix(target))
        m_process_channel_mode_command(client, msg, target);
    else
        m_process_user_mode_command(client, msg, target);
}
