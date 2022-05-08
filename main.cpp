#include "./includes/logger.hpp"
#include <iostream>

void    test_logger(void)
{
    logger::V("hello %s", "world");
    logger::D("hello %s", "world");
    logger::I("hello %s", "world");
    logger::W("hello %s", "world");
    logger::E("hello %s", "world");
    logger::F("hello %s", "world");
    logger::Vp("snpark", "hello %s", "world");
    logger::Dp("snpark", "hello %s", "world");
    logger::Ip("snpark", "hello %s", "world");
    logger::Wp("snpark", "hello %s", "world");
    logger::Ep("snpark", "hello %s", "world");
    logger::Fp("snpark", "hello %s", "world");
}

int
    main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " [port] [password]" << std::endl;
        return 1;
    }
}