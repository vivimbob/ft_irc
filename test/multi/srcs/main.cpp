#include "../includes/client.hpp"
#include "../includes/event.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <unistd.h>
#include <vector>

int
    main(int argc, char** argv)
{
    std::ifstream        test_case;
    std::string          test_case_name;
    std::vector<Client*> clients;
    Event                event;
    int                  target;
    Client*              client;
    std::string          message;
    int                  result;
    bool                 skip = false;

    if (argc < 2)
    {
        std::cerr << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    test_case_name = argv[1];
    test_case.open(test_case_name);
    if (test_case.fail())
    {
        std::cerr << argv[1] << " is not open" << std::endl;
        return 1;
    }
    test_case_name.erase(0, test_case_name.find_first_of('/') + 1);
    test_case_name.erase(0, test_case_name.find_first_of('/') + 1);
    std::string buffer;
    //연결할 서버 port
    std::getline(test_case, buffer);
    int port = atoi(buffer.c_str());
    // kqueue 생성
    event.initialize();

    // 클라이언트를 서버에 연결
    std::getline(test_case, buffer);
    std::istringstream iss(buffer);
    struct kevent      tmp;
    while (std::getline(iss, buffer, ','))
    {
        if (buffer.back() == '!')
        {
            skip = true;
            buffer.pop_back();
        }
        clients.push_back(new Client(buffer, port));
        event.m_set(clients.back()->fd, EVFILT_WRITE, EV_ADD, 0, 0,
                    clients.back());
        event.m_set(clients.back()->fd, EVFILT_READ, EV_ADD, 0, 0,
                    clients.back());
        if (!skip)
        {
            buffer = "pass 1234\r\nnick " + clients.back()->nick
                     + "\r\nuser testuser testhost testserver :realname\r\n";
            while (kevent(event._kqueue, NULL, 0, &tmp, 1, &event.timer))
                if ((int)tmp.ident == clients.back()->fd)
                {
                    if (tmp.filter == EVFILT_WRITE)
                    {
                        result = write(clients.back()->fd, buffer.data(),
                                       buffer.size());
                        if (result)
                            buffer.erase(0, result);
                    }
                    if (tmp.filter == EVFILT_READ)
                    {
                        buffer.resize(tmp.data);
                        if (read(clients.back()->fd, (void*)buffer.data(),
                                 tmp.data))
                            clients.back()->buffer.append(buffer);
                        break;
                    }
                }
            std::cout << std::setiosflags(std::ios_base::left) << std::setw(9)
                      << clients.back()->nick << ": [PASS,NICK,USER]\n";
        }
        event.m_set(clients.back()->fd, EVFILT_WRITE, EV_DISABLE, 0, 0,
                    clients.back());
        skip = false;
    }
    if (std::getline(test_case, message, ' '))
    {
        target = atoi(message.data());
        std::getline(test_case, message);
        event.m_set(clients[target]->fd, EVFILT_WRITE, EV_ENABLE, 0, 0,
                    clients[target]);
        message += "\r\n";
    }
    int count;
    while ((count = event.kevent()) > 0)
    {
        for (int i = 0; i < count; ++i)
        {
            client = (Client*)event._events[i].udata;
            if (event._events[i].filter == EVFILT_WRITE)
            {
                std::cout << std::setiosflags(std::ios_base::left)
                          << std::setw(9) << client->nick << ": " << message;
                result = write(event._events[i].ident, message.data(),
                               message.size());
                if (result == -1)
                {
                    std::cerr << "write error" << std::endl;
                    return 1;
                }
                event.m_set(client->fd, EVFILT_WRITE, EV_DISABLE, 0, 0, client);
                if (std::getline(test_case, message, ' '))
                {
					sleep(1);
                    target = atoi(message.data());
                    event.m_set(clients[target]->fd, EVFILT_WRITE, EV_ENABLE, 0,
                                0, clients[target]);
                    std::getline(test_case, message);
                    message += "\r\n";
                }
            }
            else if (event._events[i].filter == EVFILT_READ)
            {
                buffer.resize(event._events[i].data);
                result = read(event._events[i].ident, (void*)buffer.data(),
                              event._events[i].data);
                if (result == -1)
                {
                    std::cerr << "read error" << std::endl;
                    return 1;
                }
                if (result)
                    client->buffer.append(buffer);
                if (result == 0)
                {
                    close(client->fd);
                    continue;
                }
            }
        }
    }
//    std::fstream std;
//    std.open("./tests/std/" + test_case_name, std::fstream::out | std::fstream::trunc);
//    if (std.is_open())
//    {
//        for (int i = 0, size = clients.size(); i < size; ++i)
//            std << "\r\n" << clients[i]->nick << "\r\n" << clients[i]->buffer;
//        std.close();
//    }

    // 로그 읽어와서 비교하기
    std::string test;
    for (int i = 0, size = clients.size(); i < size; ++i)
        test += "\r\n" + clients[i]->nick + "\r\n" + clients[i]->buffer;
    buffer.resize(test.size());
    std.open("./tests/std/" + test_case_name, std::fstream::in);
    if (std.is_open())
    {
        std.read(buffer.begin().base(), test.size());
        if (buffer == test)
            std::cout << '\n' << test_case_name << ": ok" << std::endl;
        else
        {
            std::cout << '\n' << test_case_name << ": ko" << std::endl;
			//std::fstream test_file;
            // diff 파일 생성
            // diff 파일 출력
        }
        std::cout << "===== std ======" << buffer;
    }
}
