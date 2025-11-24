#include "Log.h"
#include <filesystem>

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init() {
  // 1. 创建控制台 sink (带颜色)
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::trace);
  // 格式参考: [时间] [日志名] [级别] 消息
  console_sink->set_pattern("%^[%T] %n: %v%$");

  // 2. 删除旧的日志文件（如果存在），确保每次运行都创建新文件
  std::filesystem::path log_path("logs/app.log");
  if (std::filesystem::exists(log_path)) {
    std::filesystem::remove(log_path);
  }
  // 确保 logs 目录存在
  if (!std::filesystem::exists(log_path.parent_path())) {
    std::filesystem::create_directories(log_path.parent_path());
  }

  // 3. 创建文件 sink (每次运行创建新文件)
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/app.log", false); // false 表示覆盖模式，不追加
  file_sink->set_level(spdlog::level::trace);
  file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

  // 4. 组合 sinks
  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

  // 5. 创建 Logger
  s_Logger = std::make_shared<spdlog::logger>("APP", begin(sinks), end(sinks));

  // 6. 注册到 spdlog 全局注册表中 (可选，但推荐)
  spdlog::register_logger(s_Logger);

  // 7. 设置全局级别
  s_Logger->set_level(spdlog::level::trace);

  // 8. 设置遇到错误立即刷新到磁盘 (防止崩了以后日志没存下来)
  s_Logger->flush_on(spdlog::level::err);
}

std::shared_ptr<spdlog::logger> &Log::GetLogger() { return s_Logger; }