
#include "common.h"
#include "mesh.h"
#include <ranges>
#include <algorithm>
#include <numeric>
#include <string>
#include <format>
#include <vector>


GraphWithMetadata generate_mesh_graph_manual(const int N) {
  Graph g(N * N);
  
  // 生成节点索引，并设置节点名称
  auto indices_nodes = std::views::iota(0, N * N);
  
  std::ranges::for_each(indices_nodes, [&g, N](int idx) {
    int i = idx / N;  // 行坐标
    int j = idx % N;  // 列坐标
    std::string name = std::format("node_({},{})", i, j);
    g[idx] = Node(idx, name);
  });

  //生成横向边，并设置边名称
  auto indices_edges_horizontal = std::views::iota(0, N * (N - 1));
  std::ranges::for_each(indices_edges_horizontal, [&g, N](int idx) {
    int row = idx / (N - 1);
    int col = idx % (N - 1);
    int left_node_idx = row * N + col;
    int right_node_idx = row * N + col + 1;
    std::string name = std::format("connect_horizontal_({},{})", row, col);

    auto egde_pair = boost::add_edge(left_node_idx, right_node_idx, g);

    if (egde_pair.second) {
      g[egde_pair.first] = Edge(idx, name, left_node_idx, right_node_idx);
    }
  });

  //生成纵向边，并设置边名称
  auto indices_edges_vertical = std::views::iota(0, (N - 1) * N);
  std::ranges::for_each(indices_edges_vertical, [&g, N](int idx) {
    int row = idx / N;
    int col = idx % N;
    int top_node_idx = row * N + col;
    int bottom_node_idx = (row + 1) * N + col;
    std::string name = std::format("connect_vertical_({},{})", row, col);

    auto egde_pair = boost::add_edge(top_node_idx, bottom_node_idx, g);

    if (egde_pair.second) {
      g[egde_pair.first] = Edge(idx + N * (N - 1), name, top_node_idx, bottom_node_idx);//顺延边的序号
    }
  });
  return GraphWithMetadata(g);
}


std::vector<GraphWithMetadata> generate_mesh_graph_manual_batch(const int N, const int batch_size) {
  auto graph_indices = std::views::iota(0, N);
  
  return graph_indices 
    | std::views::transform([N, batch_size](int) { return generate_mesh_graph_manual(batch_size); })
    | std::ranges::to<std::vector>();

    // std::vector<Graph> graphs(N);
    // std::generate(graphs.begin(), graphs.end(), 
    //     [batch_size]() {
    //         return generate_mesh_graph_manual(batch_size);
    //     }
    // );

    // return graphs;
}
