#ifndef ASTAR_COMMON_LOGGER_H
#define ASTAR_COMMON_LOGGER_H
#include <mutex>
namespace astar {
enum class LogLevel {
  kError,
  kWarning,
  kInfo,
  kDebug,
};
// just a quick trash logger (logging to files is for nerds)
class Logger {
 public:
  static void Log(LogLevel level, const char* message);
  static void SetLogLevel(LogLevel level);

 private:
  static std::string GetCurrentTimestamp();
  static std::string GetLevelTag(LogLevel level);
  static LogLevel level_;
  static std::mutex mutex_;
};

#define LOG_DEBUG(message) astar::Logger::Log(astar::LogLevel::kDebug, message)
#define LOG_INFO(message) astar::Logger::Log(astar::LogLevel::kInfo, message)
#define LOG_WARNING(message) \
  astar::Logger::Log(astar::LogLevel::kWarning, message)
#define LOG_ERROR(message) astar::Logger::Log(astar::LogLevel::kError, message)
}  // namespace astar
#endif
