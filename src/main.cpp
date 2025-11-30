#include "Log.h"
#include "error_inject.h"
#include "mesh.h"
#include "mesh_data.h"
#include "mesh_utils.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <omp.h>
#include <random>
#include <set>
#include <source_location>
#include <string>

static std::mt19937 g_rng(42);

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
    auto location = std::source_location::current();
    LOG_INFO("[{}] 运行时间:  {}  ms", name, duration);
    // LOG_INFO("[{}] 文件: {}", location.file_name(), location.line());
  }
};

int main() {

  Log::Init();
  LOG_INFO("程序开始运行");

  ScopedTimer timer("start the program");

  // 创建输出文件夹
  std::filesystem::create_directories("out");

  for (int k = 2; k <= 8; k++) {
    // 为每个 k 创建对应的 CSV 文件
    std::string filename = "out/heatmap_data_k" + std::to_string(k) + ".csv";
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
      LOG_INFO("无法打开文件: {}", filename);
      continue;
    }
    LOG_INFO("output file: {}", filename);

    // 写入 CSV 表头
    outfile << "node_error_rate,edge_error_rate,sub_graphs_rate,"
               "incomplete_graphs_rate\n";
#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int j_idx = 0; j_idx <= 19; ++j_idx) {
      for (int i_idx = 0; i_idx <= 18; ++i_idx) {

        float j = 0.005f + j_idx * 0.005f;
        float i = 0.01f + i_idx * 0.005f;
        std::mt19937 local_rng(std::random_device{}());

        std::vector<GraphWithMetadata> graphs =
            generate_mesh_graph_manual_batch(10000, k);

        random_error_inject(graphs, j, i, local_rng);

        // std::ranges::for_each(
        //     graphs, [](GraphWithMetadata &g) { g.delete_isolated_nodes(); });
        auto sub_graphs = graphs | std::views::filter([](GraphWithMetadata &g) {
                            return g.has_subgraphs();
                          });
        auto incomplete_graphs =
            graphs | std::views::filter(
                         [](GraphWithMetadata &g) { return !g.is_full(); });

        float sub_graphs_rate =
            static_cast<float>(std::ranges::distance(sub_graphs)) / 10000 * 100;
        float incomplete_graphs_rate =
            static_cast<float>(std::ranges::distance(incomplete_graphs)) /
            10000 * 100;

#pragma omp critical
        {
          LOG_INFO("start with topology size: {} and node error rate: {:.2f}% "
                   "and edge error rate: {:.2f}%",
                   k, j * 100, i * 100);
          LOG_INFO("sub_graphs rate: {:.4f}%", sub_graphs_rate);
          LOG_INFO("incomplete_graphs rate: {:.4f}%", incomplete_graphs_rate);
          LOG_INFO("--------------------------------");

          outfile << j * 100 << "," << i * 100 << "," << sub_graphs_rate << ","
                  << incomplete_graphs_rate << "\n";
          outfile.flush();
        }
      }
    }

    outfile.close();
    LOG_INFO("数据已保存到文件: {}", filename);
  }

  // std::vector<GraphWithMetadata> graphs =
  //     generate_mesh_graph_manual_batch(5000, 4);
  // random_error_inject(graphs, 0.05, 0.09, g_rng);

  // std::ranges::sort(graphs, {}, [](GraphWithMetadata &g) { return
  // g.score(); });

  // auto num_sub_graphs = graphs |
  //                       std::views::transform([](GraphWithMetadata &g) {
  //                         return g.num_components();
  //                       }) |
  //                       std::ranges::to<std::set>();

  // for (auto num : num_sub_graphs) {
  //   LOG_INFO("num_sub_graphs: {}", num);
  // }

  // int count = 0;
  // std::ranges::for_each(
  //     graphs, [&count](GraphWithMetadata &g) { g.delete_isolated_nodes();
  //     });

  // auto sub_graphs = graphs | std::views::filter([](GraphWithMetadata &g) {
  //                     return g.has_subgraphs();
  //                   });
  // auto incomplete_graphs =
  //     graphs |
  //     std::views::filter([](GraphWithMetadata &g) { return !g.is_full();
  //     });

  // std::ranges::for_each(sub_graphs, [](GraphWithMetadata &g) {
  //   print_mesh_graph(g.graph());
  //   std::println("--------------------------------");
  // });

  return 0;
}