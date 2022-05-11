#include "includes/logger.hpp"

int main(int, char **)
{
    Logger::enable_echo_mode(true);

    const std::string message = "Hello, world!";
    const std::string timestampSeparator = " ## ";
    Logger::set_timestamp_mode(Logger::DateTime, timestampSeparator);

    Logger::set_loglevel(Logger::None);
    Logger().fatal() << message;
    Logger::set_loglevel(Logger::Fatal);
    Logger().fatal() << message;
\
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

    return 0;
}