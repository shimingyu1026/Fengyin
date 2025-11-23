#include "error_inject.h"

void node_error_inject(Graph& g, const int node_idx) {
  // 检查节点索引是否有效
  if (node_idx < 0 || node_idx >= static_cast<int>(boost::num_vertices(g))) {
    return;  // 无效索引，直接返回
  }
  
  // 获取顶点描述符
  auto vertex = boost::vertex(node_idx, g);
  
  // 检查节点是否已经被删除
  if (g[vertex].is_deleted) {
    return;  // 节点已经被删除，直接返回
  }
  
  // 清除与该顶点相关的所有边
  boost::clear_vertex(vertex, g);
  
  // 标记节点为已删除（保留索引，不真正删除节点）
  g[vertex].is_deleted = true;
}

void edge_error_inject(Graph& g, const int source_idx, const int target_idx) {
  // 检查节点索引是否有效
  int num_vertices = static_cast<int>(boost::num_vertices(g));
  if (source_idx < 0 || source_idx >= num_vertices ||
      target_idx < 0 || target_idx >= num_vertices) {
    return;  // 无效索引，直接返回
  }
  
  // 检查源节点和目标节点是否已被删除
  auto source_vertex = boost::vertex(source_idx, g);
  auto target_vertex = boost::vertex(target_idx, g);
  
  if (g[source_vertex].is_deleted || g[target_vertex].is_deleted) {
    return;  // 节点已被删除，无法删除边
  }
  
  // 获取边描述符
  auto edge_pair = boost::edge(source_vertex, target_vertex, g);
  
  // 如果边存在，则删除它
  if (edge_pair.second) {
    boost::remove_edge(edge_pair.first, g);
  }
}

void  random_error_inject(std::vector<Graph>& graphs, const float node_error_rate, const float edge_error_rate,std::mt19937& rng) {

  // 获取网格大小
  int num_graphs = graphs.size(); // 图的数量

  int num_nodes = boost::num_vertices(graphs[0]);
  int num_edges = boost::num_edges(graphs[0]);

  [[maybe_unused]]int mesh_size = std::sqrt(num_nodes); // 拓扑大小 n * n 


  // 计算总的节点数和边数
  int total_nodes = num_graphs * num_nodes;
  int total_edges = num_graphs * num_edges;


  int total_error_nodes = static_cast<int>(std::round(total_nodes * node_error_rate));
  int total_error_edges = static_cast<int>(std::round(total_edges * edge_error_rate));


  // 对边进行错误注入
  std::vector<int> error_edge_idx;
  auto range = std::views::iota(0, total_edges) | std::ranges::to<std::vector>();
  std::ranges::sample(range, std::back_inserter(error_edge_idx), total_error_edges, rng);

  std::ranges::for_each(error_edge_idx, [&graphs, num_edges](int idx) {
    int sub_idx = idx % num_edges;
    int chip_idx = idx / num_edges;

    // 获取图中的所有边，找到指定的idx的边，并删除它
    auto& g = graphs[chip_idx];
    auto edges = boost::edges(g);

    auto found = std::ranges::find_if(std::ranges::subrange(edges.first, edges.second),[&g,sub_idx](auto edge){
      return g[edge].id == sub_idx;
    });

    if (found != edges.second) {
      auto edge = *found;
      edge_error_inject(g, g[edge].source_idx, g[edge].target_idx);
    }
    else
    {
      std::println("not found edge: {}", sub_idx);
    }
  });

  // 对节点进行错误注入
  std::vector<int> error_node_idx;
  range = std::views::iota(0, total_nodes) | std::ranges::to<std::vector>();
  std::ranges::sample(range, std::back_inserter(error_node_idx), total_error_nodes, rng);

  std::ranges::for_each(error_node_idx, [&graphs, num_nodes](int idx) {
    int sub_idx = idx % num_nodes;

    int chip_idx = idx / num_nodes;

    node_error_inject(graphs[chip_idx], sub_idx);
  });
}
