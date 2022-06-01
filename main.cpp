#include "./includes/logger.hpp"
#include "./includes/server.hpp"
#include <cstdlib>

int
    main(int argc, char **argv)
{
    Server(argc, argv).run();
    return 0;
}
