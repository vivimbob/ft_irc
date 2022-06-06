#include "../../includes/test.hpp"
#include "../../includes/client.hpp"
#include <ios>
#include <unistd.h>

#include <iostream>
static void case_01()
{
	std::string test_case("case_01");

	Client client1(1, test_case); 
	std::cout << "case_01 start\n";
	client1.write("PASS 1234\r\nNICK client1\r\nUSER client01 tester tester tester\r\n");
	sleep(2);
	std::cout << test_case << ": ";
	std::cout << "(1)";
	if (!client1.diff())
		std::cout << "\x1b[32msame\n\x1b[0m";
	else
		std::cout << "\x1b[31mdiff\n\x1b[0m";
}

void test_register()
{
	std::cout << "\x1b[33m[test register]\x1b[0m" << std::endl;
	case_01();
}
