#include "../includes/log.hpp"

void
    log::timestamp()
{
    std::time(&log::time::raw);
    if (std::strftime(time::buffer, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S",
                      std::localtime(&time::raw)))
        std::cout << time::buffer << ": ";
}

std::ostream&
    log::print()
{
    log::timestamp();
    return std::cout;
}

std::ostream&
    log::endl(std::ostream& os)
{
    os.put(os.widen('\n'));
    os.flush();
    return os;
}

