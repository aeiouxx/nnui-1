#include "logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace astar {
#ifdef ASTAR_DEBUG
LogLevel Logger::level_ = LogLevel::kDebug;
#else
LogLevel Logger::level_ = LogLevel::kInfo;
#endif
std::mutex Logger::mutex_;

void Logger::Log(LogLevel level, const char* message) {
  std::lock_guard<std::mutex> lock(Logger::mutex_);
  if (level <= Logger::level_) {
    std::string timestamp = GetCurrentTimestamp();
    std::string levelTag = GetLevelTag(level);
    switch (level) {
      case LogLevel::kDebug:
        std::cout << "\033[1;34m[" << timestamp << "] " << levelTag << " "
                  << message << "\033[0m\n";
        break;
      case LogLevel::kInfo:
        std::cout << "[" << timestamp << "] " << levelTag << " " << message
                  << "\n";
        break;
      case LogLevel::kWarning:
        std::cout << "\033[1;33m[" << timestamp << "] " << levelTag << " "
                  << message << "\033[0m\n";
        break;
      case LogLevel::kError:
        std::cerr << "\033[1;31m[" << timestamp << "] " << levelTag << " "
                  << message << "\033[0m\n";
        break;
      default:
        std::cerr << "[" << timestamp << "] [" << levelTag << "] "
                  << " " << message << "\n";
        break;
    }
  }
}
void Logger::SetLogLevel(LogLevel level) {
  std::lock_guard<std::mutex> lock(Logger::mutex_);
  Logger::level_ = level;
}
std::string Logger::GetCurrentTimestamp() {
  using namespace std::chrono;
  using clock = system_clock;
  const auto now{clock::now()};
  const auto time{clock::to_time_t(now)};
  std::tm localTime;
#ifdef _WIN32
  localtime_s(&localTime, &time);
#elif __linux__
  localtime_r(&time, &localTime);
#else
#error "Unsupported platform."
#endif
  const auto timeSinceEpoch{now.time_since_epoch()};
  const auto currentMilliseconds{
      duration_cast<milliseconds>(timeSinceEpoch).count() % 1000};
  std::ostringstream timestamp;
  timestamp << std::put_time(&localTime, "%T") << "." << std::setw(3)
            << std::setfill('0') << currentMilliseconds;
  return timestamp.str();
}
std::string Logger::GetLevelTag(LogLevel level) {
  switch (level) {
    case LogLevel::kError:
      return "[ERROR]  ";
    case LogLevel::kWarning:
      return "[WARNING]";
    case LogLevel::kInfo:
      return "[INFO]   ";
    case LogLevel::kDebug:
      return "[DEBUG]  ";
    default:
      return "[UNKNOWN]";
  }
}
}  // namespace astar
