#include "mesh_utils.h"

void print_mesh_graph(const Graph &g) {
  int num_vertices = static_cast<int>(boost::num_vertices(g));
  if (num_vertices == 0) {
    std::println("Empty graph");
    return;
  }

  // 计算网格大小 N
  int N = static_cast<int>(std::sqrt(num_vertices));
  if (N * N != num_vertices) {
    std::println("Error: Graph is not a perfect square mesh ({} vertices)",
                 num_vertices);
    return;
  }

  // 创建打印矩阵：每个节点占一行，节点之间用边行分隔
  // 节点行：O - O - O
  // 边行：  |   |   |
  // 总行数 = N * 2 - 1 (最后一行不需要边行)
  // 总列数 = N * 2 - 1 (最后一列不需要边)
  int rows = N * 2 - 1;
  int cols = N * 2 - 1;
  std::vector<std::vector<char>> grid(rows, std::vector<char>(cols, ' '));

  // 标记所有节点位置
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      int node_idx = i * N + j;
      auto vertex = boost::vertex(node_idx, g);

      int grid_row = i * 2;
      int grid_col = j * 2;

      // 如果节点未删除，标记为 'O'
      if (!g[vertex].is_deleted) {
        grid[grid_row][grid_col] = 'O';
      }
    }
  }

  // 标记所有边
  auto [edge_begin, edge_end] = boost::edges(g);
  for (auto it = edge_begin; it != edge_end; ++it) {
    auto edge = *it;

    // 使用 Boost 的标准方法获取边的端点
    auto source_vertex = boost::source(edge, g);
    auto target_vertex = boost::target(edge, g);

    // 如果边的任一端点被删除，不显示该边
    if (g[source_vertex].is_deleted || g[target_vertex].is_deleted) {
      continue;
    }

    // 对于 vecS 顶点列表，顶点描述符就是索引
    int source_idx = static_cast<int>(source_vertex);
    int target_idx = static_cast<int>(target_vertex);

    int source_i = source_idx / N;
    int source_j = source_idx % N;
    int target_i = target_idx / N;
    int target_j = target_idx % N;

    // 判断是横向边还是纵向边
    if (source_i == target_i) {
      // 横向边：连接同一行的两个节点
      int left_j = std::min(source_j, target_j);
      int right_j = std::max(source_j, target_j);

      // 确保是相邻节点
      if (right_j - left_j == 1) {
        int grid_row = source_i * 2;
        int grid_col = left_j * 2 + 1;
        grid[grid_row][grid_col] = '-';
      }
    } else if (source_j == target_j) {
      // 纵向边：连接同一列的两个节点
      int top_i = std::min(source_i, target_i);
      int bottom_i = std::max(source_i, target_i);

      // 确保是相邻节点
      if (bottom_i - top_i == 1) {
        int grid_row = top_i * 2 + 1;
        int grid_col = source_j * 2;
        grid[grid_row][grid_col] = '|';
      }
    }
  }

  // 打印网格
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      std::print("{}", grid[i][j]);
    }
    std::println("");
  }
}

int count_mesh_score(const Graph &g) {
  int num_vertices = static_cast<int>(boost::num_vertices(g));
  int num_edges = static_cast<int>(boost::num_edges(g));

  int score = num_edges * 1 + num_vertices * 3;
  return score;
}

bool has_subgraphs(const Graph &g) {
  int num_vertices = static_cast<int>(boost::num_vertices(g));
  if (num_vertices == 0) {
    return false; // 空图没有子图
  }

  // 创建新图，只包含未删除的节点和相关的边
  Graph filtered_g;

  // 建立原图索引到新图顶点的映射
  std::vector<int> vertex_map(num_vertices, -1);

  // 添加所有未删除的节点
  for (int i = 0; i < num_vertices; ++i) {
    auto vertex = boost::vertex(i, g);
    if (!g[vertex].is_deleted) {
      auto new_vertex = boost::add_vertex(filtered_g);
      filtered_g[new_vertex] = g[vertex];
      vertex_map[i] = static_cast<int>(new_vertex);
    }
  }

  // 添加所有连接未删除节点的边
  auto [edge_begin, edge_end] = boost::edges(g);
  for (auto it = edge_begin; it != edge_end; ++it) {
    auto edge = *it;
    int source_idx = g[edge].source_idx;
    int target_idx = g[edge].target_idx;

    // 检查两个节点是否都未删除
    if (vertex_map[source_idx] != -1 && vertex_map[target_idx] != -1) {
      auto new_source = boost::vertex(vertex_map[source_idx], filtered_g);
      auto new_target = boost::vertex(vertex_map[target_idx], filtered_g);
      auto [new_edge, inserted] =
          boost::add_edge(new_source, new_target, filtered_g);
      if (inserted) {
        filtered_g[new_edge] = g[edge];
      }
    }
  }

  // 计算连通分量
  int num_vertices_filtered = static_cast<int>(boost::num_vertices(filtered_g));
  if (num_vertices_filtered == 0) {
    return false; // 没有有效节点
  }

  std::vector<int> component(num_vertices_filtered);
  int num_components =
      boost::connected_components(filtered_g, component.data());

  // 如果连通分量数量大于1，说明有子图
  return num_components > 1;
}

bool is_graph_full(const Graph &g) {
  int num_vertices = static_cast<int>(boost::num_vertices(g));

  int mesh_size = std::sqrt(num_vertices);
  int num_edges = mesh_size * (mesh_size - 1) * 2;

  int full_score = num_edges * 1 + num_vertices * 3;

  return count_mesh_score(g) == full_score;
}

void delete_isolated_nodes(Graph &g) {
  boost::graph_traits<Graph>::vertex_iterator vi, vend;
  boost::tie(vi, vend) = boost::vertices(g);

  std::ranges::for_each(std::ranges::subrange(vi, vend), [&g](auto vertex) {
    if (boost::degree(vertex, g) == 0 && !g[vertex].is_deleted) {
      g[vertex].is_deleted = true;
    }
  });
}

void generate_topology(const std::filesystem::path &file_path,
                       const std::string &name, const int weight,
                       const int pipeline_stage_delay,
                       const GraphWithMetadata &g) {

  std::filesystem::path parentDir = file_path.parent_path();
  if (!std::filesystem::exists(parentDir)) {
    std::filesystem::create_directories(parentDir); // 递归创建目录
    LOG_INFO("目录已创建: {}", parentDir.string());
  }

  // 如果文件存在，先删除（清空）
  if (std::filesystem::exists(file_path)) {
    std::filesystem::remove(file_path);
    LOG_INFO("已删除旧文件: {}", file_path.string());
  }

  std::ofstream outfile(file_path);

  std::string topology = g.get_graph_topology();
  std::string ss;
  ss += std::format("graph {} {{\n", name);
  ss += std::format("\tedge[weight={}]\n", weight);
  ss += std::format("\tnode[pipeline_stage_delay={}]\n", pipeline_stage_delay);
  ss += topology;
  ss += std::format("}}\n");
  std::print(outfile, "{}", ss);
}