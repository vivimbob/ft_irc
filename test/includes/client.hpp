#ifndef TEST_CLIENT_HPP
#define TEST_CLIENT_HPP
#include <fstream>

#define ERGO 6667
#define IRCSERV 6668

class Client
{
  private:
    static std::string m_command;
    std::string m_ergo_fifo_file;
    std::string m_ircserv_fifo_file;
    int m_ergo_fd;
    int m_ircserv_fd;
    std::fstream m_ergo_fifo;
    std::fstream m_ircserv_fifo;
    int m_diff_exitstatus;

  public:
    std::string m_log_file;
    Client();
    Client(int fifo, std::string test_case);
    ~Client();
    void write(std::string msg);
    bool diff();
};

#endif /* TEST_CLIENT_HPP */
