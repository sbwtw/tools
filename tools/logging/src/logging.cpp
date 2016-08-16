/*********************************************************
          File Name:logging.cpp
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Sun 14 Aug 2016 03:51:01 PM CST
**********************************************************/

#include "logging.h"
#include <stdlib.h>

static const char* MSG[] = 
{
  " [INFO]    ",
  " [DEBUG]   ",
  " [WARNING] ",
  " [ERROR]   ",
  " [FATAL]   "
};
namespace nm
{
  thread_local static char __buf[25] = {0};
  std::unique_ptr<Logging::Logger> Logging::logger_ = nullptr;

  Logging::Logger::Logger(const std::string& path,
      size_t size_limit,
      time_t duration,
      bool thread_safe,
      int flush_interval,
      int check_interval)
    : tm_len_(0), tm_time_(),
    stream_(new FileCtl(path.c_str(), size_limit, duration,
        thread_safe, flush_interval, check_interval))
  {
  }
  Logging::Logger& Logging::Logger::operator() (const char* file, int line,
      LogLevel level)
  {
    tm_len_ = tm_time_.update().format(__buf, 25);
    stream_.append(__buf, tm_len_);
    stream_ << gettid();
    stream_.append(MSG[level], 11);
    stream_ << file;
    stream_.append(" ", 1);
    stream_ << line;
    stream_.append(" => ", 4);
    return *this;
  }
  Logging::Logger& Logging::Logger::operator() (const char* file, int line,
      LogLevel level, bool is_abort)
  {
    (*this)(file, line, level);
    if(is_abort)
      abort();
    return *this;
  }
  Logging::Logger& Logging::Logger::operator() (const char* file, int line,
      LogLevel level, const char* func)
  {
    tm_len_ = tm_time_.update().format(__buf, 25);
    stream_.append(__buf, tm_len_);
    stream_ << gettid();
    stream_.append(MSG[level], 11);
    stream_ << file;
    stream_.append(" ", 1);
    stream_ << line;
    stream_.append(" `", 2);
    stream_ << func;
    stream_.append("` => ", 5);
    return *this;
  }
}