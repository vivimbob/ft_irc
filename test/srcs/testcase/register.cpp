#include "../../includes/bulk_client.hpp"
#include "../../includes/client.hpp"
#include "../../includes/test.hpp"
#include <ios>
#include <iostream>
#include <unistd.h>

static void
    case_01()
{
    BulkClient client(1, "case_01");

    client.write(
        1,
        "PASS 1234\r\nNICK client1\r\nUSER client01 tester tester tester\r\n");
    client.print_result();
}

static void
    case_02()
{
    BulkClient clients(5, "case_02");

    clients.write(
        0,
        "PASS 1234\r\nNICK client0\r\nUSER client00 tester tester tester\r\n");
    clients.write(
        1,
        "PASS 1234\r\nNICK client1\r\nUSER client01 tester tester tester\r\n");
    clients.write(
        2,
        "PASS 1234\r\nNICK client2\r\nUSER client02 tester tester tester\r\n");
    clients.write(
        3,
        "PASS 1234\r\nNICK client3\r\nUSER client03 tester tester tester\r\n");
    clients.write(
        4,
        "PASS 1234\r\nNICK client4\r\nUSER client04 tester tester tester\r\n");
    clients.print_result();
}

void
    test_register()
{
    std::cout << "\x1b[32m[test register]\x1b[0m" << std::endl;
    case_01();
    case_02();
}
