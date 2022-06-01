#include "./includes/logger.hpp"
#include "./includes/server.hpp"
#include <cstdlib>

int
    main(int argc, char **argv)
{
    const std::string timestamp_separator = ": ";
    Logger::set_timestamp_mode(Logger::Date_time_ver1, timestamp_separator);
	Logger::set_loglevel(Logger::Trace);

    Server(argc, argv).run();
    return 0;
}
