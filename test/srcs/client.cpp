#include "../includes/client.hpp"
#include "../includes/test.hpp"
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <csignal>

#include <iostream>
std::string Client::m_command("nc localhost ");

Client::Client()
{
}

Client::Client(int fifo, std::string test_case)
{
	m_ircserv_fifo_file = "./fifo/" + std::to_string(fifo) + ".ircserv";
	m_log_file = "./log/" + test_case + "." + std::to_string(fifo);
#if TEST_IRCSERV == 1
	close(mkfifo(m_ircserv_fifo_file.data(), 0666));
	m_ircserv_fd = execute(m_command + std::to_string(IRCSERV)
			+ " > " + m_log_file + ".ircserv"
			+ " < " + m_ircserv_fifo_file);
	this->m_ircserv_fifo.open(m_ircserv_fifo_file);
#endif

#if TEST_ERGO == 1
	m_ergo_fifo_file = "./fifo/" + std::to_string(fifo) + ".ergo";
	close(mkfifo(m_ergo_fifo_file.data(), 0666));
	m_ergo_fd = execute(m_command + std::to_string(ERGO)
			+ " > " + m_log_file + ".ergo"
			+ " < " + m_ergo_fifo_file);
	this->m_ergo_fifo.open(m_ergo_fifo_file);
#endif
}

Client::~Client()
{
#if TEST_IRCSERV == 1
	this->m_ircserv_fifo.close();
	kill(m_ircserv_fd, 9);
#endif

#if TEST_ERGO == 1
	this->m_ergo_fifo.close();
	kill(m_ergo_fd, 9);
#endif
}

void Client::write(std::string msg)
{
#if TEST_IRCSERV == 1
	m_ircserv_fifo << msg;
	m_ircserv_fifo.flush();
#endif
#if TEST_ERGO == 1
	m_ergo_fifo << msg;
	m_ergo_fifo.flush();
#endif
}

bool Client::diff()
{
#if TEST_ERGO == 1 && TEST_IRCSERV == 1
	int fd = execute("diff " + m_log_file + ".ircserv " + m_log_file + ".ergo > " + m_log_file + ".diff && rm " + m_log_file + ".diff 2> /dev/null");
	waitpid(fd, &m_diff_exitstatus, 0); 
	return m_diff_exitstatus;
#endif
}
