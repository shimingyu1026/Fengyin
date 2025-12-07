/**
 * @file Log.h
 * @brief 日志系统定义
 *
 * 提供基于 spdlog 的日志系统，包括日志宏和计时器
 */

#pragma once

#include <memory>
#include <source_location>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

/**
 * @brief 日志类
 *
 * 提供日志系统的初始化和访问接口
 */
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

/**
 * @brief 作用域计时器
 *
 * 在构造时开始计时，析构时输出运行时间
 */

#define LOG_TRACE(...) ::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Log::GetLogger()->critical(__VA_ARGS__)

// 统计代码运行时间
struct ScopedTimer {
  std::string name;
  std::chrono::time_point<std::chrono::high_resolution_clock> start;

  ScopedTimer(const std::string &func_name)
      : name(func_name), start(std::chrono::high_resolution_clock::now()) {}

  ~ScopedTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    [[maybe_unused]] auto location = std::source_location::current();
    LOG_INFO("[{}] 运行时间:  {}  ms", name, duration);
    // LOG_INFO("[{}] 文件: {}", location.file_name(), location.line());
  }
};