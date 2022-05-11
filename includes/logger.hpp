#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iostream>
#include <map>

class Logger
{

/* enum begin */

  public:
    enum Level
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal,
        None
    };

    enum Timestamp_mode
    {
        TimeNone,
        DateTime,
        EpochSeconds,
        EpochMilliseconds,
        EpochMicroseconds
    };
    
/* enum end */

/* Member functions begin */

  public:
    Logger();

    ~Logger();

    static void 
      enable_echo_mode(bool enable);

    static void 
      set_loglevel(int level);

    static void 
      set_level_symbol(int level, std::string symbol);

    static void 
      set_timestamp_mode(int time, std::string separator = " ");

    static void 
      set_stream(int level, std::ostream& stream);

    std::ostringstream& 
      trace();

    std::ostringstream& 
      debug();

    std::ostringstream& 
      info();

    std::ostringstream& 
      warning();

    std::ostringstream& 
      error();

    std::ostringstream& 
      fatal();

  private:
    Logger(const Logger &log);
    Logger& 
      operator=(const Logger &log);

/* Member functions end */

/* Member variable begin */

  private:
    class _Impl;
    _Impl* m_impl;

/* Member variable end */

};

class Logger::_Impl
{

/* Member types begin */

    typedef std::map<int, std::string> _Symbol_Map;

    typedef std::map<int, std::ostream *> _Stream_Map;

/* Member types end */

/* Member functions begin */

  public:

    _Impl();

    ~_Impl();

    std::ostringstream& 
      m_trace();

    std::ostringstream& 
      m_debug();

    std::ostringstream& 
      m_info();

    std::ostringstream& 
      m_warning();

    std::ostringstream& 
      m_error();

    std::ostringstream& 
      m_fatal();

    static void 
      m_enable_echo_mode(bool enable);

    static void 
      m_set_level_symbol(int level, std::string symbol);

    static void 
      m_set_loglevel(int level);

    static void 
      m_set_timestamp_mode(int time, std::string separator);

    static void 
      m_set_stream(int level, std::ostream& stream);

    void 
      m_flush();

    std::ostringstream& 
      m_get_stream(int level);

    void 
      m_prefix_timestamp();

    static _Symbol_Map m_initial_symbol_map();

    static _Stream_Map m_initial_stream_map();

/* Member functions end */

/* Member variables begin */

  private:

    static _Symbol_Map m_symbols;

    static _Stream_Map m_streams;

    static bool m_echo_mode;

    static int m_level;

    static int m_timestamp_mode;

    static std::string m_timestamp_separator;

    int m_active_level;

    std::ostringstream m_oss;

/* Member variables end */

};

#endif /* LOGGER_HPP */