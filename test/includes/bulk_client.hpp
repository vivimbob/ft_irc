#ifndef BULK_CLIENT_HPP
#define BULK_CLIENT_HPP
#include "client.hpp"
#include <vector>

class BulkClient
{
  private:
    std::vector<Client *> m_clients;
    std::string m_test_case;

  public:
    BulkClient();
    ~BulkClient();
    BulkClient(int num, std::string test_case);
    void write(int num, std::string message);
    void print_result();
};
#endif /* BULK_CLIENT_HPP */
