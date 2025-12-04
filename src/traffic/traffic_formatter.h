#pragma once

#include "traffic.h"
#include <format>

// TrafficNode 的格式化器特化
// 用于支持 std::format 和 std::println
template <> struct std::formatter<TrafficNode> {
  // 解析格式字符串
  // 对于空格式说明符 {}，返回指向 '}' 的迭代器
  constexpr auto parse(std::format_parse_context &ctx) {
    auto it = ctx.begin();
    const auto end = ctx.end();
    // 如果当前位置是 '}'，说明格式说明符为空，直接返回
    // 如果当前位置不是 '}'，说明有自定义格式说明符，需要解析到 '}' 为止
    while (it != end && *it != '}') {
      // 可以在这里处理自定义格式说明符
      // 例如：{:verbose} 或 {:compact}
      // 当前实现只支持默认格式 {}，所以直接跳过所有字符直到 '}'
      ++it;
    }
    // 返回指向 '}' 的迭代器（或 end，如果没有找到 '}'）
    return it;
  }

  // 格式化 TrafficNode
  auto format(const TrafficNode &node, std::format_context &ctx) const {
    auto out = ctx.out();

    // 格式化 ID 和 LAYERS 标题
    out = std::format_to(out, "[{0:03}] LAYERS: \n", node.id);

    // 输出 layers（每个 gemm 一行，layers 中已经包含换行符）
    out = std::format_to(out, "{}\n", node.layers);

    // 空行
    out = std::format_to(out, "\n");

    // FirstSend 字段
    out = std::format_to(out, "FirstSend: {}\n",
                         node.is_send_first ? "YES" : "NO");

    // 格式化 send_traffic
    out = std::format_to(out, "SEND_TO/DataSize: ({0}, {1})\n",
                         node.send_traffic.first, node.send_traffic.second);

    // 格式化 recv_traffic（可能有多个发送方）
    if (node.recv_traffic.empty()) {
      out = std::format_to(out, "RECV_FROM: N/A\n");
    } else {
      for (const auto &[source_id, data_size] : node.recv_traffic) {
        out = std::format_to(out, "RECV_FROM/DataSize: ({0}, {1})\n", source_id,
                             data_size);
      }
    }

    return out;
  }
};
