#include "../includes/log.hpp"

namespace log
{

void
    timestamp()
{
    std::time(&time::raw);
    if (std::strftime(time::buffer, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S",
                      std::localtime(&time::raw)))
        std::cout << time::buffer << ": ";
}

std::ostream&
    print()
{
    log::timestamp();
    return std::cout;
}

std::ostream&
    endl(std::ostream& os)
{
    os.put(os.widen('\n'));
    os.flush();
    return os;
}

} // namespace log
