/**
 * @file traffic.cpp
 * @brief 流量生成和转换实现
 *
 * 实现流量节点的生成、排序和代码转换功能
 */

#include "traffic.h"
#include <algorithm>
#include <format>
#include <string>
#include <unordered_map>

/**
 * @brief 从向量中随机选择一个元素
 * @param values 候选值向量
 * @param rng 随机数生成器
 * @return 随机选中的元素值
 */
int random_selecte(const std::vector<int> &values, std::mt19937 &rng) {
  std::uniform_int_distribution<> dist(0, std::ranges::ssize(values) - 1);
  return values[dist(rng)];
}

/**
 * @brief 生成单个随机GEMM操作
 * @param rng 随机数生成器
 * @param type 数据类型
 * @return <GEMM名称, 数据量（字节）>
 * @note 数据量计算基于输出矩阵大小：m × n
 */
std::pair<std::string, int> _random_gemm_generate(std::mt19937 &rng,
                                                  const std::string &type) {
  int size = TYPE_SIZE_MAP.at(type);

  int m = random_selecte(M_VALUES, rng);
  int n = random_selecte(N_VALUES, rng);
  int k = random_selecte(K_VALUES, rng);

  std::string name = std::format("gemm_{}_{}_{}", m, n, k);
  int data_size = size * (m * n);

  return std::make_pair(name, data_size);
}

/**
 * @brief 生成多个随机GEMM操作
 * @param numbers GEMM操作数量
 * @param rng 随机数生成器
 * @param type 数据类型
 * @return <逗号分隔的GEMM名称字符串, 总数据量（字节）>
 */
std::pair<std::string, int> random_gemm_generate(const int numbers,
                                                 std::mt19937 &rng,
                                                 const std::string &type) {
  std::string ss;
  int total_size = 0;
  for (int i = 0; i < numbers; i++) {
    auto [name, size] = _random_gemm_generate(rng, type);
    if (i == 0) {
      ss += std::format("\"{}\"", name);
    } else {
      ss += std::format(",\"{}\"", name);
    }
    total_size += size;
  }
  return std::make_pair(ss, total_size);
}

/**
 * @brief 生成单轮随机流量节点
 * @param rng 随机数生成器
 * @param graph 图结构
 * @param layer_num 每个节点的GEMM层数
 * @param data_type 数据类型
 * @return 流量节点向量
 *
 * 生成流程：
 * 1. 获取所有有效节点ID
 * 2. 为每个节点随机生成发送流量和目标节点
 * 3. 建立接收关系
 * 4. 对接收流量排序并确定发送顺序
 */
std::vector<TrafficNode>
_random_traffic_generate(std::mt19937 &rng, GraphWithMetadata &graph,
                         int layer_num, const std::string &data_type) {
  std::vector<TrafficNode> traffic_nodes;

  // 获取所有有效节点ID
  std::vector<int> node_ids;
  auto vertices = boost::vertices(graph.graph());
  for (const auto &v : std::ranges::subrange(vertices.first, vertices.second)) {
    if (!graph.graph()[v].is_deleted) {
      node_ids.push_back(graph.graph()[v].id);
    }
  }

  if (node_ids.size() < 2) {
    return traffic_nodes; // 节点数少于2，无法建立流量关系
  }

  // 为每个节点生成发送流量（随机选择目标节点）
  for (auto node_id : node_ids) {
    auto [ss, total_size] = random_gemm_generate(layer_num, rng, data_type);

    // 在 node_ids 里除当前节点以外随机选一个作为目标
    std::vector<int> candidates;
    for (const auto &id : node_ids) {
      if (id != node_id) {
        candidates.push_back(id);
      }
    }
    std::uniform_int_distribution<> dist(
        0, static_cast<int>(candidates.size()) - 1);
    int target_node = candidates[dist(rng)];

    traffic_nodes.emplace_back(node_id, ss,
                               std::make_pair(target_node, total_size));
  }

  // 建立接收关系：为每个节点创建ID到TrafficNode的映射
  std::unordered_map<int, std::vector<TrafficNode>::iterator> node_map;
  for (auto it = traffic_nodes.begin(); it != traffic_nodes.end(); ++it) {
    node_map[it->id] = it;
  }

  // 将发送信息添加到目标节点的接收列表中
  for (auto &traffic_node : traffic_nodes) {
    int source_node = traffic_node.id;
    int target_node = traffic_node.send_traffic.first;
    int data_size = traffic_node.send_traffic.second;

    auto target_it = node_map.find(target_node);
    if (target_it != node_map.end()) {
      target_it->second->recv_traffic.emplace_back(source_node, data_size);
    }
  }

  // 对接收流量排序并确定发送顺序
  for (auto &traffic_node : traffic_nodes) {
    if (!traffic_node.recv_traffic.empty()) {
      // 按发送方ID从大到小排序（大编号节点先发送）
      std::ranges::sort(
          traffic_node.recv_traffic,
          [](const auto &a, const auto &b) { return a.first > b.first; });

      // 如果最大发送方ID >= 当前节点ID，需要先接收后发送
      int max_sender_id = traffic_node.recv_traffic.front().first;
      traffic_node.is_send_first = (max_sender_id < traffic_node.id);
    }
  }

  return traffic_nodes;
}

/**
 * @brief 生成多轮随机流量
 * @param rng 随机数生成器
 * @param graph 图结构
 * @param layer_num 每个节点的GEMM层数
 * @param data_type 数据类型
 * @param round_num 轮数
 * @return 多轮流量节点
 */
std::vector<std::vector<TrafficNode>>
random_traffic_generate(std::mt19937 &rng, GraphWithMetadata &graph,
                        int layer_num, const std::string &data_type,
                        const int round_num) {
  std::vector<std::vector<TrafficNode>> traffic_nodes;
  for (int i = 0; i < round_num; i++) {
    traffic_nodes.push_back(
        _random_traffic_generate(rng, graph, layer_num, data_type));
  }
  return traffic_nodes;
}

/**
 * @brief 将单轮流量节点转换为C++代码字符串
 * @param traffic_nodes 流量节点向量
 * @return C++代码字符串，格式为逗号分隔的字段列表
 *
 * 输出格式：
 * - 先发送模式：layers, "send", target_id, size, "recv", sender_id, size, ...
 * - 先接收模式：layers, "recv", sender_id, size, ..., "send", target_id, size,
 * "recv", sender_id, size, ...
 */
std::string _traffic_gen_to_cpp_code(std::vector<TrafficNode> traffic_nodes) {
  std::string ss;
  for (auto [index, traffic_node] :
       std::ranges::enumerate_view(traffic_nodes)) {
    if (index == 0) {
      ss += std::format("{}", traffic_node.layers);
    } else {
      ss += std::format(",{}", traffic_node.layers);
    }

    if (traffic_node.is_send_first) {
      // 先发送后接收模式
      ss += std::format(",\"send\",\"{}\",\"{}\"",
                        traffic_node.send_traffic.first,
                        traffic_node.send_traffic.second);

      for (const auto &recv_traffic : traffic_node.recv_traffic) {
        ss += std::format(",\"recv\",\"{}\",\"{}\"", recv_traffic.first,
                          recv_traffic.second);
      }
    } else {
      // 先接收后发送模式：send混在recv中间
      // 先接收发送方ID >= 当前节点ID的流量
      for (const auto &recv_traffic : traffic_node.recv_traffic) {
        if (recv_traffic.first >= traffic_node.id) {
          ss += std::format(",\"recv\",\"{}\",\"{}\"", recv_traffic.first,
                            recv_traffic.second);
        }
      }

      // 发送自己的流量
      ss += std::format(",\"send\",\"{}\",\"{}\"",
                        traffic_node.send_traffic.first,
                        traffic_node.send_traffic.second);

      // 最后接收发送方ID < 当前节点ID的流量
      for (const auto &recv_traffic : traffic_node.recv_traffic) {
        if (recv_traffic.first < traffic_node.id) {
          ss += std::format(",\"recv\",\"{}\",\"{}\"", recv_traffic.first,
                            recv_traffic.second);
        }
      }
    }
  }
  return ss;
}

/**
 * @brief 将多轮流量节点转换为C++代码字符串
 * @param traffic_nodes 多轮流量节点
 * @return C++代码字符串，格式为嵌套的初始化列表
 *
 * 输出格式：
 * {
 *  { layers, "send", ... },
 *  { layers, "recv", ... },
 *  ...
 * };
 */
std::string
traffic_gen_to_cpp_code(std::vector<std::vector<TrafficNode>> traffic_nodes) {
  std::string ss;
  ss += "{\n";
  for (auto [index, traffic_node] :
       std::ranges::enumerate_view(traffic_nodes)) {
    if (index == 0) {
      ss += std::format(" {{ {} }}", _traffic_gen_to_cpp_code(traffic_node));
    } else {
      ss += std::format(",\n{{ {} }}", _traffic_gen_to_cpp_code(traffic_node));
    }
  }
  ss += "\n};";
  return ss;
}

// 使用示例
//  auto out = random_traffic_generate(g_rng, graphs[0], 2, "BF16", 4);

// out = matrix_transpose_non_square(out);
// std::string ss = traffic_gen_to_cpp_code(out);
// std::println("{}", ss);