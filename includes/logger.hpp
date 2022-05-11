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
        Time_none,
        Date_time_ver1,
        Date_time_ver2
    };
    
/* enum end */

/* Member functions begin */

  public:
    Logger(void);

    ~Logger(void);

    static void 
      set_loglevel(int level);

    static void 
      set_level_symbol(int level, std::string symbol);

    static void 
      set_timestamp_mode(int time, std::string separator = " ");

    std::ostringstream& 
      trace(void);

    std::ostringstream& 
      debug(void);

    std::ostringstream& 
      info(void);

    std::ostringstream& 
      warning(void);

    std::ostringstream& 
      error(void);

    std::ostringstream& 
      fatal(void);

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

    _Impl(void);

    ~_Impl(void);

    std::ostringstream& 
      m_trace(void);

    std::ostringstream& 
      m_debug(void);

    std::ostringstream& 
      m_info(void);

    std::ostringstream& 
      m_warning(void);

    std::ostringstream& 
      m_error(void);

    std::ostringstream& 
      m_fatal(void);

    static void 
      m_set_level_symbol(int level, std::string symbol);

    static void 
      m_set_loglevel(int level);

    static void 
      m_set_timestamp_mode(int time, std::string separator);

    void 
      m_flush(void);

    std::ostringstream& 
      m_get_stream(int level);

    void 
      m_prefix_timestamp(void);

    static _Symbol_Map m_initial_symbol_map(void);

    static _Stream_Map m_initial_stream_map(void);

/* Member functions end */

/* Member variables begin */

  private:

    static _Symbol_Map m_symbols;

    static _Stream_Map m_streams;

    static int m_level;

    static int m_timestamp_mode;

    static std::string m_timestamp_separator;

    int m_active_level;

    std::ostringstream m_oss;

/* Member variables end */

};

#endif /* LOGGER_HPP */