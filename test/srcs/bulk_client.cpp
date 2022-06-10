#include "../includes/bulk_client.hpp"
#include <iostream>
#include <unistd.h>

BulkClient::BulkClient()
{
}

BulkClient::~BulkClient()
{
	std::vector<Client *>::iterator it = m_clients.begin();
	std::vector<Client *>::iterator ite = m_clients.end();
	for (; it != ite; ++it) 
		delete *it;
}
// 로그가 정렬을 위해 client번호는 1부터
BulkClient::BulkClient(int num, std::string test_case)
{
	m_test_case = test_case;
	for (int i = 0; i <= num; ++i)
	{
		m_clients.push_back(new Client(i, test_case));
		sleep(1);
	}
}

void BulkClient::write(int num, std::string message)
{
	m_clients[num]->write(message);
}

void BulkClient::print_result()
{
	std::cout << m_test_case << ": ";
	std::vector<Client *>::iterator it = m_clients.begin();
	std::vector<Client *>::iterator ite = m_clients.end();
	for (int i = 0; it != ite; ++it, ++i) 
	{
		if (!(*it)->diff())
			std::cout << "\033[32m" << i << ": same\033[0m ";
		else
			std::cout << "\033[31m" << i << ": diff\033[0m ";
	}
	std::cout << std::endl;
}
