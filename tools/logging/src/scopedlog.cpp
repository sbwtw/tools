/*********************************************************
          File Name:scopedlog.cpp
          Author: Abby Cin
          Mail: abbytsing@gmail.com
          Created Time: Sun 14 Aug 2016 10:54:59 AM CST
**********************************************************/

#include "scopedlog.h"
#include "timefmt.h"
#include <stdlib.h>
#include <cstdio>
#include <cerrno>

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
  static void default_appender(const char* line, int len)
  {
    ::fwrite_unlocked(line, 1, len, stdout);
  }
  static void default_flush()
  {
    ::fflush(stdout);
  }
  static ScopedLog::out global_appender = nullptr;
  static ScopedLog::flush_out global_flush = nullptr;
  thread_local static char __buf[25] = {0};

  // format: yyyymmdd_HHMMSS.nnnnnnu.tid [level] file line func: message
  ScopedLog::wrapper::wrapper(const char* file, int line, LogLevel level)
    : tm_len_(0), tm_time_(TimeFmt::now()), stream_()
  {
    tm_len_ = tm_time_.format(__buf, 25);
    stream_.append(__buf, tm_len_);
    stream_ << gettid();
    stream_.append(MSG[level], 11);
    stream_ << file;
    stream_.append(" ", 1);
    stream_ << line;
    stream_.append(" => ", 4);
  }
  ScopedLog::wrapper::wrapper(const char* file, int line, LogLevel level,
      const char* func)
    : stream_()
  {
    tm_len_ = tm_time_.format(__buf, 25);
    stream_.append(__buf, tm_len_);
    stream_ << gettid();
    stream_.append(MSG[level], 11);
    stream_ << file;
    stream_.append(" ", 1);
    stream_ << line;
    stream_.append(" `", 2);
    stream_<< func;
    stream_.append("` => ", 5);
  }
  BufferStream& ScopedLog::wrapper::stream()
  {
    return stream_;
  }
  const char* ScopedLog::wrapper::data() const
  {
    return stream_.buffer();
  }
  size_t ScopedLog::wrapper::size() const
  {
    return stream_.size();
  }
  ScopedLog::wrapper::~wrapper()
  {
  }

  ScopedLog::ScopedLog(const char* file, int line, LogLevel level)
    : level_(level), logger_(file, line, level)
  {
    if(global_appender == nullptr)
      set_appender(default_appender);
    if(global_flush == nullptr)
      set_flush(default_flush);
  }
  ScopedLog::ScopedLog(const char* file, int line, LogLevel level,
      const char* func)
    : level_(level), logger_(file, line, level, func)
  {
    if(global_appender == nullptr)
      set_appender(default_appender);
    if(global_flush == nullptr)
      set_flush(default_flush);
  }
  ScopedLog::~ScopedLog()
  {
    global_appender(logger_.data(), logger_.size());
    global_flush();
    if(level_ == LogLevel::FATAL)
      abort();
  }
  BufferStream& ScopedLog::stream()
  {
    return logger_.stream();
  }
  void ScopedLog::set_appender(out f)
  {
    global_appender = f;
  }
  void ScopedLog::set_flush(flush_out f)
  {
    global_flush = f;
  }
}