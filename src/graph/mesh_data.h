/**
 * @file mesh_data.h
 * @brief 带元数据的图数据结构定义
 *
 * 提供图的元数据管理、惰性计算、节点和边的操作接口
 */

#pragma once

#include "common.h"
#include <optional>

/**
 * @brief 图的元数据结构
 *
 * 存储图的各种元数据属性，使用 optional 实现惰性计算
 */
struct GraphMetadata {
  std::optional<bool> has_subgraphs;      // 是否有子图
  std::optional<bool> is_full;            // 是否完整
  std::optional<int> num_components;      // 连通分量数量
  std::optional<int> score;               // 图的分数
  std::optional<bool> is_all_nodes_exist; // 是否所有节点都存在
};

/**
 * @brief 带元数据的图包装类
 *
 * 封装 Boost Graph，提供元数据管理和惰性计算功能
 */
class GraphWithMetadata {
private:
  Graph graph_;
  size_t graph_size_;

  mutable GraphMetadata metadata_;
  mutable bool metadata_dirty_ = true; // 全局脏标记

  // 私有方法：清除所有缓存
  void invalidate_metadata() const;

  // 私有方法：检查并清除脏标记（当所有属性都已计算时）
  void check_and_clear_dirty() const;

  // 私有方法：惰性计算各个属性
  void ensure_has_subgraphs() const;
  void ensure_is_full() const;
  void ensure_num_components() const;
  void ensure_score() const;
  void ensure_is_all_nodes_exist() const;

public:
  // 构造和移动
  GraphWithMetadata() = default;
  explicit GraphWithMetadata(Graph g) : graph_(std::move(g)) {
    graph_size_ = static_cast<size_t>(std::sqrt(boost::num_vertices(g)));
  }

  // 访问原始图
  size_t graph_size() const { return graph_size_; }

  // 访问原始图（只读）
  const Graph &graph() const { return graph_; }

  // 访问原始图（可修改，修改后需要标记为脏）
  Graph &graph_mut() {
    invalidate_metadata();
    return graph_;
  }

  // 输出图的拓扑结构
  std::string get_graph_topology() const;

  // 获取元数据（惰性计算）
  bool has_subgraphs() const;
  bool is_full() const;
  int num_components() const;
  int score() const;
  bool is_all_nodes_exist() const;

  // 修改接口（自动更新脏标记）
  void delete_node(int node_idx);
  void delete_edge(int source_idx, int target_idx);
  void delete_isolated_nodes();

  void remove_random_edges(const int edge_num);
  void remove_random_nodes(const int node_num);
};
