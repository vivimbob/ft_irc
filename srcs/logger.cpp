#ifndef LOGGER_CPP
#define LOGGER_CPP

#include "../includes/logger.hpp"

/* Logger::_Impl static member variables begin */

  int Logger::_Impl::m_level = Info;

  int Logger::_Impl::m_timestamp_mode = Date_time_ver1;

  std::string Logger::_Impl::m_timestamp_separator = ": ";

  Logger::_Impl::_Symbol_Map Logger::_Impl::m_symbols = Logger::_Impl::m_initial_symbol_map();

  Logger::_Impl::_Stream_Map Logger::_Impl::m_streams = Logger::_Impl::m_initial_stream_map();

/* Logger::_Impl static member variables end */

/* Logger::_Impl member functions begin */

  Logger::_Impl::_Impl() : m_active_level(Info)
  {
  }

  Logger::_Impl::~_Impl()
  {
      m_flush();
  }

  std::ostringstream& 
    Logger::_Impl::m_get_stream(int level)
  {
      m_active_level = level;
      _Impl::m_prefix_timestamp();
      m_oss << _Impl::m_symbols[level] << " ";
      return m_oss;
  }

  void 
    Logger::_Impl::m_set_level_symbol(int level, std::string symbol)
  {
      _Impl::m_symbols[level] = symbol;
  }

  void 
    Logger::_Impl::m_set_loglevel(int level)
  {
      _Impl::m_level = level;
  }

  void 
    Logger::_Impl::m_set_timestamp_mode(int timestamp_mode, std::string separator)
  {
      _Impl::m_timestamp_mode = timestamp_mode;
      _Impl::m_timestamp_separator = separator;
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
          char buffer[50];
          time(&m_raw_time);

          if(std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&m_raw_time)))
          m_oss << buffer << m_timestamp_separator;
      }
          break;
      case Date_time_ver2:
      {
          time_t m_raw_time;
          std::string m_time_str;
          time(&m_raw_time);
          m_time_str = ctime(&m_raw_time);
          m_time_str.erase(m_time_str.length() - 1);

          if (!m_time_str.empty())
          {
              m_oss << m_time_str << m_timestamp_separator;
          }
      }
          break;
      }
  }

  void 
    Logger::_Impl::m_flush()
  {
      if (m_active_level < m_level)
      {
          return;
      }

      if (!m_oss.str().size())
      {
          return;
      }

      std::ostream* stream = _Impl::m_streams[m_active_level];
      if (stream) {
          *stream << m_oss.str() << "\033[0m" << std::endl;
          stream->flush();
      }
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
    Logger::_Impl::m_warning()
  {
      return m_get_stream(Warning);
  }

  std::ostringstream& 
    Logger::_Impl::m_error()
  {
      return m_get_stream(Error);
  }

  std::ostringstream& 
    Logger::_Impl::m_fatal()
  {
      return m_get_stream(Fatal);
  }

  void 
    Logger::_Impl::m_set_stream(int level, std::ostream& stream)
  {
      Logger::_Impl::m_streams[level] = &stream;
  }

  Logger::_Impl::_Symbol_Map 
    Logger::_Impl::m_initial_symbol_map()
  {
    Logger::_Impl::_Symbol_Map temp_map;

    temp_map.insert(std::make_pair(Trace, "\033[34mT:"));
    temp_map.insert(std::make_pair(Debug, "\033[36mD:"));
    temp_map.insert(std::make_pair(Info, "\033[37mI:"));
    temp_map.insert(std::make_pair(Warning, "\033[33mW:"));
    temp_map.insert(std::make_pair(Error, "\033[35mE:"));
    temp_map.insert(std::make_pair(Fatal, "\033[31mF:"));

    return (temp_map);
  }

  Logger::_Impl::_Stream_Map 
    Logger::_Impl::m_initial_stream_map()
  {
    Logger::_Impl::_Stream_Map temp_map;

    temp_map.insert(std::make_pair(Trace, &std::cout));
    temp_map.insert(std::make_pair(Debug, &std::cout));
    temp_map.insert(std::make_pair(Info, &std::cout));
    temp_map.insert(std::make_pair(Warning, &std::cerr));
    temp_map.insert(std::make_pair(Error, &std::cerr));
    temp_map.insert(std::make_pair(Fatal, &std::cerr));

    return (temp_map);
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
    Logger::set_level_symbol(int level, std::string symbol)
  {
      _Impl::m_set_level_symbol(level, symbol);
  }

  void 
    Logger::set_timestamp_mode(int timestamp_mode, std::string separator)
  {
      _Impl::m_set_timestamp_mode(timestamp_mode, separator);
  }

  void 
    Logger::set_stream(int level, std::ostream& stream)
  {
      _Impl::m_set_stream(level, stream);
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
    Logger::warning()
  {
      return m_impl->m_warning();
  }

  std::ostringstream& 
    Logger::error()
  {
      return m_impl->m_error();
  }

  std::ostringstream& 
    Logger::fatal()
  {
      return m_impl->m_fatal();
  }

/* Logger member functions end */

#endif /* LOGGER_CPP */