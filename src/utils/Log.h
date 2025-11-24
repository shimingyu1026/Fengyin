#pragma once

#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <vector>

class Log {
public:
  // 初始化函数，只需在 main 开始时调用一次
  static void Init();

  // 获取 Logger 实例
  static std::shared_ptr<spdlog::logger> &GetLogger();

private:
  static std::shared_ptr<spdlog::logger> s_Logger;
};

// --- 定义宏，方便使用 ---
// 在代码里直接用 LOG_INFO("msg"); 而不用 Log::GetLogger()->info("msg");

#define LOG_TRACE(...) ::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Log::GetLogger()->critical(__VA_ARGS__)