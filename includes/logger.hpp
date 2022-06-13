#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <ctime>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>

class Logger
{

    /* enum begin */

  public:
    enum Level
    {
        Trace,
        Debug,
        Info,
        Error,
        None
    };

    enum Timestamp_mode
    {
        Time_none,
        Date_time_ver1,
        Date_time_ver2
    };

    /* enum end */

    /* Member functions begin */

  public:
    Logger();

    ~Logger();

    static void set_loglevel(int level);

    static void set_timestamp_mode(int time, std::string separator = " ");

    std::ostringstream &trace();

    std::ostringstream &debug();

    std::ostringstream &info();

    std::ostringstream &error();

    std::ostringstream &log(int level);

  private:
    Logger(const Logger &log);
    Logger &operator=(const Logger &log);

    /* Member functions end */

    /* Member variable begin */

  private:
    class _Impl;
    _Impl *m_impl;

    /* Member variable end */
};

class Logger::_Impl
{

    /* Member types begin */

    typedef std::pair<int, std::string> _Symbol;

    typedef std::pair<int, std::ostream *> _Stream;

    /* Member types end */

    /* Member functions begin */

  public:
    _Impl();

    ~_Impl();

    std::ostringstream &m_trace();

    std::ostringstream &m_debug();

    std::ostringstream &m_info();

    std::ostringstream &m_error();

    static void m_set_loglevel(int level);

    static void m_set_timestamp_mode(int time, std::string separator);

    void m_flush();

    std::ostringstream &m_get_stream(int level);

    void m_prefix_timestamp();

    //    static _Symbol_Map m_initial_symbol_map();
    //
    //    static _Stream_Map m_initial_stream_map();

    /* Member functions end */

    /* Member variables begin */

  private:
    static _Symbol m_symbols[4];
    static _Stream m_streams[4];

    static int m_level;

    static int m_timestamp_mode;

    static std::string m_timestamp_separator;

    int m_active_level;

    std::ostringstream m_oss;

    /* Member variables end */
};

#endif /* LOGGER_HPP */
