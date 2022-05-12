#include "./includes/logger.hpp"
#include "./includes/server.hpp"

int
    main(int argc, char **argv)
{
    const std::string message = "Hello, world!";
    const std::string timestamp_separator = " ## ";
    // const std::string timestamp_separator = " -> ";
    Logger::set_timestamp_mode(Logger::Date_time_ver1, timestamp_separator);
    // Logger::set_timestamp_mode(Logger::Date_time_ver2, timestamp_separator);

    Logger::set_loglevel(Logger::None);
    Logger().fatal() << message;
    Logger::set_loglevel(Logger::Fatal);
    Logger().fatal() << message;

    Logger().error() << message;
    Logger::set_loglevel(Logger::Error);
    Logger().error() << message;

    Logger().warning() << message;
    Logger::set_loglevel(Logger::Warning);
    Logger().warning() << message;

    Logger().info() << message;
    Logger::set_loglevel(Logger::Info);
    Logger().info() << message;

    Logger().debug() << message;
    Logger::set_loglevel(Logger::Debug);
    Logger().debug() << message;

    Logger().trace() << message;
    Logger::set_loglevel(Logger::Trace);
    Logger().trace() << message;

    Server(argc, argv);
    return 0;
}