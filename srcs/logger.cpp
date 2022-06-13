#include "../includes/logger.hpp"

/* Logger::_Impl static member variables begin */

int Logger::_Impl::m_level = Trace;

int Logger::_Impl::m_timestamp_mode = Date_time_ver1;

std::string Logger::_Impl::m_timestamp_separator = ": ";

Logger::_Impl::_Symbol Logger::_Impl::m_symbols[] = {
    std::make_pair(Trace, "\033[34mT:"), std::make_pair(Debug, "\033[36mD:"),
    std::make_pair(Info, "\033[37mI:"), std::make_pair(Error, "\033[35mE:")};

Logger::_Impl::_Stream Logger::_Impl::m_streams[] = {
    std::make_pair(Trace, &std::cout), std::make_pair(Debug, &std::cout),
    std::make_pair(Info, &std::cout), std::make_pair(Error, &std::cerr)};

/* Logger::_Impl static member variables end */

/* Logger::_Impl member functions begin */

void
    Logger::_Impl::m_flush()
{
    if (m_active_level < m_level)
        return;

    if (!m_oss.str().size())
        return;

    std::ostream* stream = _Impl::m_streams[m_active_level].second;
    if (stream)
    {
        *stream << m_oss.str() << "\033[0m" << std::endl;
        stream->flush();
    }
}

Logger::_Impl::_Impl() : m_active_level(Info)
{
}

Logger::_Impl::~_Impl()
{
    m_flush();
}

void
    Logger::_Impl::m_prefix_timestamp()
{
    std::string m_time_str;

    switch (_Impl::m_timestamp_mode)
    {
    case Time_none:
        break;
    case Date_time_ver1:
    {
        time_t m_raw_time;
        char   buffer[50];
        time(&m_raw_time);

        if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S",
                          std::localtime(&m_raw_time)))
            m_oss << buffer << m_timestamp_separator;
    }
    break;
    case Date_time_ver2:
    {
        time_t      m_raw_time;
        std::string m_time_str;
        time(&m_raw_time);
        m_time_str = ctime(&m_raw_time);
        m_time_str.erase(m_time_str.length() - 1);

        if (!m_time_str.empty())
            m_oss << m_time_str << m_timestamp_separator;
    }
    break;
    }
}

std::ostringstream&
    Logger::_Impl::m_get_stream(int level)
{
    m_active_level = level;
    _Impl::m_prefix_timestamp();
    m_oss << _Impl::m_symbols[level].second << " ";
    return m_oss;
}

void
    Logger::_Impl::m_set_loglevel(int level)
{
    _Impl::m_level = level;
}

void
    Logger::_Impl::m_set_timestamp_mode(int         timestamp_mode,
                                        std::string separator)
{
    _Impl::m_timestamp_mode      = timestamp_mode;
    _Impl::m_timestamp_separator = separator;
}

std::ostringstream&
    Logger::_Impl::m_trace()
{
    return m_get_stream(Trace);
}

std::ostringstream&
    Logger::_Impl::m_debug()
{
    return m_get_stream(Debug);
}

std::ostringstream&
    Logger::_Impl::m_info()
{
    return m_get_stream(Info);
}

std::ostringstream&
    Logger::_Impl::m_error()
{
    return m_get_stream(Error);
}

/* Logger::_Impl member functions end */

/* Logger member functions begin */

Logger::Logger() : m_impl(new Logger::_Impl)
{
}

Logger::~Logger()
{
    delete m_impl;
}

void
    Logger::set_loglevel(int level)
{
    _Impl::m_set_loglevel(level);
}

void
    Logger::set_timestamp_mode(int timestamp_mode, std::string separator)
{
    _Impl::m_set_timestamp_mode(timestamp_mode, separator);
}

std::ostringstream&
    Logger::trace()
{
    return m_impl->m_trace();
}

std::ostringstream&
    Logger::debug()
{
    return m_impl->m_debug();
}

std::ostringstream&
    Logger::info()
{
    return m_impl->m_info();
}

std::ostringstream&
    Logger::error()
{
    return m_impl->m_error();
}

std::ostringstream&
    Logger::log(int level)
{
    return m_impl->m_get_stream(level);
}
/* Logger member functions end */
