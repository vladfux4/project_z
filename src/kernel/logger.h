/*=============================================================================
Project Z - Operating system for ARM processors
Copyright (C) 2018 Vladyslav Samodelok <vladfux4@gmail.com>
All rights reserved.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

=============================================================================*/
#ifndef KERNEL_LOGGER_H_
#define KERNEL_LOGGER_H_

#include <cstdint>

#define __FILENAME__ (&__FILE__[SOURCE_PATH_SIZE])
#define LOG(LEVEL) kernel::log::Logger<kernel::log::Level::LEVEL>(__FILENAME__)

namespace kernel {
namespace log {

void InitPrint();
void Print(const char* s);
void Print(const char* s, const uint64_t d);
void PrintHex(const uint64_t d);

enum class Level { VERBOSE, DEBUG, INFO, WARNING, ERROR };

template <Level kLevel>
struct LogConfig {};

template <>
struct LogConfig<Level::VERBOSE> {
  static constexpr bool enabled = true;
};

template <>
struct LogConfig<Level::DEBUG> {
  static constexpr bool enabled = true;
};

template <>
struct LogConfig<Level::INFO> {
  static constexpr bool enabled = true;
};

template <>
struct LogConfig<Level::WARNING> {
  static constexpr bool enabled = true;
};

template <>
struct LogConfig<Level::ERROR> {
  static constexpr bool enabled = true;
};

template <bool>
class LoggerBase {
 protected:
  void Begin(const char* info) { (void)info; }
  void Flush() {}
  void Log(const char* value) { (void)value; }
  void Log(const uint64_t& value) { (void)value; }
  void Log(const void* value) { (void)value; }
};

template <>
class LoggerBase<true> {
 protected:
  void Begin(const char* info) {
    Log(info);
    Log(": ");
  }

  void Flush() { Print("\n"); }

  void Log(const char* value) { Print(value); }

  void Log(const uint64_t& value) { PrintHex(value); }

  void Log(const void* value) { PrintHex(reinterpret_cast<uint64_t>(value)); }
};

template <Level kLogLevel>
class Logger : public LoggerBase<LogConfig<kLogLevel>::enabled> {
 public:
  using Base = LoggerBase<LogConfig<kLogLevel>::enabled>;

  Logger(const char* info) { Base::Begin(info); }
  ~Logger() { Base::Flush(); }

  Logger& operator<<(const char* value) {
    Base::Log(value);
    return *this;
  }

  Logger& operator<<(const uint64_t& value) {
    Base::Log(value);
    return *this;
  }

  Logger& operator<<(const void* value) {
    Base::Log(value);
    return *this;
  }
};

}  // namespace log
}  // namespace kernel

#endif  // KERNEL_LOGGER_H_
