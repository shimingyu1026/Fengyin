#include "mesh_data.h"
#include "error_inject.h"
#include "mesh_utils.h"

// 清除所有缓存
void GraphWithMetadata::invalidate_metadata() const {
  metadata_dirty_ = true;
  metadata_.has_subgraphs.reset();
  metadata_.is_full.reset();
  metadata_.num_components.reset();
  metadata_.score.reset();
}

// 检查并清除脏标记（当所有属性都已计算时）
void GraphWithMetadata::check_and_clear_dirty() const {
  if (metadata_dirty_ && metadata_.has_subgraphs.has_value() &&
      metadata_.is_full.has_value() && metadata_.num_components.has_value() &&
      metadata_.score.has_value()) {
    metadata_dirty_ = false;
  }
}

// 惰性计算：是否有子图
void GraphWithMetadata::ensure_has_subgraphs() const {
  if (metadata_.has_subgraphs.has_value()) {
    return;
  }

  int num_vertices = static_cast<int>(boost::num_vertices(graph_));
  if (num_vertices == 0) {
    metadata_.has_subgraphs = false;
    check_and_clear_dirty();
    return;
  }

  // 创建新图，只包含未删除的节点和相关的边
  Graph filtered_g;

  // 建立原图索引到新图顶点的映射
  std::vector<int> vertex_map(num_vertices, -1);

  // 添加所有未删除的节点
  for (int i = 0; i < num_vertices; ++i) {
    auto vertex = boost::vertex(i, graph_);
    if (!graph_[vertex].is_deleted) {
      auto new_vertex = boost::add_vertex(filtered_g);
      filtered_g[new_vertex] = graph_[vertex];
      vertex_map[i] = static_cast<int>(new_vertex);
    }
  }

  // 添加所有连接未删除节点的边
  auto [edge_begin, edge_end] = boost::edges(graph_);
  for (auto it = edge_begin; it != edge_end; ++it) {
    auto edge = *it;
    int source_idx = graph_[edge].source_idx;
    int target_idx = graph_[edge].target_idx;

    // 检查两个节点是否都未删除
    if (vertex_map[source_idx] != -1 && vertex_map[target_idx] != -1) {
      auto new_source = boost::vertex(vertex_map[source_idx], filtered_g);
      auto new_target = boost::vertex(vertex_map[target_idx], filtered_g);
      auto [new_edge, inserted] =
          boost::add_edge(new_source, new_target, filtered_g);
      if (inserted) {
        filtered_g[new_edge] = graph_[edge];
      }
    }
  }

  // 计算连通分量
  int num_vertices_filtered = static_cast<int>(boost::num_vertices(filtered_g));
  if (num_vertices_filtered == 0) {
    metadata_.has_subgraphs = false;
    check_and_clear_dirty();
    return;
  }

  std::vector<int> component(num_vertices_filtered);
  int num_components =
      boost::connected_components(filtered_g, component.data());

  // 如果连通分量数量大于1，说明有子图
  metadata_.has_subgraphs = (num_components > 1);

  // 检查是否可以清除脏标记
  check_and_clear_dirty();
}

// 惰性计算：是否完整
void GraphWithMetadata::ensure_is_full() const {
  if (metadata_.is_full.has_value()) {
    return;
  }

  int num_vertices = static_cast<int>(boost::num_vertices(graph_));
  int mesh_size = std::sqrt(num_vertices);
  int num_edges = mesh_size * (mesh_size - 1) * 2;

  int full_score = num_edges * 1 + num_vertices * 3;
  int current_score = score(); // 这会触发 ensure_score

  metadata_.is_full = (current_score == full_score);

  // 检查是否可以清除脏标记
  check_and_clear_dirty();
}

// 惰性计算：连通分量数量
void GraphWithMetadata::ensure_num_components() const {
  if (metadata_.num_components.has_value()) {
    return;
  }

  int num_vertices = static_cast<int>(boost::num_vertices(graph_));
  if (num_vertices == 0) {
    metadata_.num_components = 0;
    check_and_clear_dirty();
    return;
  }

  // 创建新图，只包含未删除的节点和相关的边
  Graph filtered_g;

  // 建立原图索引到新图顶点的映射
  std::vector<int> vertex_map(num_vertices, -1);

  // 添加所有未删除的节点
  for (int i = 0; i < num_vertices; ++i) {
    auto vertex = boost::vertex(i, graph_);
    if (!graph_[vertex].is_deleted) {
      auto new_vertex = boost::add_vertex(filtered_g);
      filtered_g[new_vertex] = graph_[vertex];
      vertex_map[i] = static_cast<int>(new_vertex);
    }
  }

  // 添加所有连接未删除节点的边
  auto [edge_begin, edge_end] = boost::edges(graph_);
  for (auto it = edge_begin; it != edge_end; ++it) {
    auto edge = *it;
    int source_idx = graph_[edge].source_idx;
    int target_idx = graph_[edge].target_idx;

    // 检查两个节点是否都未删除
    if (vertex_map[source_idx] != -1 && vertex_map[target_idx] != -1) {
      auto new_source = boost::vertex(vertex_map[source_idx], filtered_g);
      auto new_target = boost::vertex(vertex_map[target_idx], filtered_g);
      auto [new_edge, inserted] =
          boost::add_edge(new_source, new_target, filtered_g);
      if (inserted) {
        filtered_g[new_edge] = graph_[edge];
      }
    }
  }

  // 计算连通分量
  int num_vertices_filtered = static_cast<int>(boost::num_vertices(filtered_g));
  if (num_vertices_filtered == 0) {
    metadata_.num_components = 0;
    check_and_clear_dirty();
    return;
  }

  std::vector<int> component(num_vertices_filtered);
  int num_components =
      boost::connected_components(filtered_g, component.data());

  metadata_.num_components = num_components;

  // 检查是否可以清除脏标记
  check_and_clear_dirty();
}

// 惰性计算：图的分数
void GraphWithMetadata::ensure_score() const {
  if (metadata_.score.has_value()) {
    return;
  }

  int num_vertices = static_cast<int>(boost::num_vertices(graph_));
  int num_edges = static_cast<int>(boost::num_edges(graph_));

  metadata_.score = num_edges * 1 + num_vertices * 3;

  // 检查是否可以清除脏标记
  check_and_clear_dirty();
}

void GraphWithMetadata::ensure_is_all_nodes_exist() const {
  if (metadata_.is_all_nodes_exist.has_value()) {
    return;
  }

  int num_vertices = static_cast<int>(boost::num_vertices(graph_));
  auto ranges = std::views::iota(0, num_vertices);
  metadata_.is_all_nodes_exist = std::ranges::all_of(ranges, [&](int i) {
    auto vertex = boost::vertex(i, graph_);
    return !graph_[vertex].is_deleted;
  });

  // 检查是否可以清除脏标记
  check_and_clear_dirty();
}

//---------------------------------------------------------------
// 公共接口：获取是否有子图
bool GraphWithMetadata::has_subgraphs() const {
  ensure_has_subgraphs();
  return metadata_.has_subgraphs.value();
}

// 公共接口：获取是否完整
bool GraphWithMetadata::is_full() const {
  ensure_is_full();
  return metadata_.is_full.value();
}

// 公共接口：获取连通分量数量
int GraphWithMetadata::num_components() const {
  ensure_num_components();
  return metadata_.num_components.value();
}

// 公共接口：获取图的分数
int GraphWithMetadata::score() const {
  ensure_score();
  return metadata_.score.value();
}

// 公共接口：获取是否所有节点都存在
bool GraphWithMetadata::is_all_nodes_exist() const {
  ensure_is_all_nodes_exist();
  return metadata_.is_all_nodes_exist.value();
}

// 公共接口：获取图的拓扑结构
std::string GraphWithMetadata::get_graph_topology() const {
  std::string topology;
  size_t node_num = graph_size_ * graph_size_;

  for (size_t i = 0; i < node_num; ++i) {
    auto vertex_i = boost::vertex(i, graph_);
    if (graph_[vertex_i].is_deleted) {
      topology += std::format("\t{}\n", i);
       continue;
    }
    for (size_t j = i + 1; j < node_num; ++j) {
      auto vertex_j = boost::vertex(j, graph_);
      if (graph_[vertex_j].is_deleted) {
        continue;
      }
      auto [edge, exists] = boost::edge(vertex_i, vertex_j, graph_);
      if (exists) {
        topology += std::format("\t{}--{}\n", i, j);
      }
    }
  }
  return topology;
}

//---------------------------------------------------------------
// 修改接口：删除节点
void GraphWithMetadata::delete_node(int node_idx) {
  // 检查节点索引是否有效
  if (node_idx < 0 ||
      node_idx >= static_cast<int>(boost::num_vertices(graph_))) {
    return; // 无效索引，直接返回
  }

  // 获取顶点描述符
  auto vertex = boost::vertex(node_idx, graph_);

  // 检查节点是否已经被删除
  if (graph_[vertex].is_deleted) {
    return; // 节点已经被删除，直接返回
  }

  // 清除与该顶点相关的所有边
  boost::clear_vertex(vertex, graph_);

  // 标记节点为已删除（保留索引，不真正删除节点）
  graph_[vertex].is_deleted = true;

  // 失效缓存
  invalidate_metadata();
}

// 修改接口：删除边
void GraphWithMetadata::delete_edge(int source_idx, int target_idx) {
  // 检查节点索引是否有效
  int num_vertices = static_cast<int>(boost::num_vertices(graph_));
  if (source_idx < 0 || source_idx >= num_vertices || target_idx < 0 ||
      target_idx >= num_vertices) {
    return; // 无效索引，直接返回
  }

  // 检查源节点和目标节点是否已被删除
  auto source_vertex = boost::vertex(source_idx, graph_);
  auto target_vertex = boost::vertex(target_idx, graph_);

  if (graph_[source_vertex].is_deleted || graph_[target_vertex].is_deleted) {
    return; // 节点已被删除，无法删除边
  }

  // 获取边描述符
  auto edge_pair = boost::edge(source_vertex, target_vertex, graph_);

  // 如果边存在，则删除它
  if (edge_pair.second) {
    boost::remove_edge(edge_pair.first, graph_);
  }

  // 失效缓存
  invalidate_metadata();
}

// 修改接口：删除孤立节点
void GraphWithMetadata::delete_isolated_nodes() {
  boost::graph_traits<Graph>::vertex_iterator vi, vend;
  boost::tie(vi, vend) = boost::vertices(graph_);

  bool modified = false;
  std::ranges::for_each(std::ranges::subrange(vi, vend), [&](auto vertex) {
    if (boost::degree(vertex, graph_) == 0 && !graph_[vertex].is_deleted) {
      graph_[vertex].is_deleted = true;
      modified = true;
    }
  });

  // 如果修改了图，失效缓存
  if (modified) {
    invalidate_metadata();
  }
}
