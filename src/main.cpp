#include "Log.h"
#include "error_inject.h"
#include "mesh.h"
#include "mesh_data.h"
#include "mesh_utils.h"
#include <chrono>
#include <random>
#include <set>
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
    LOG_INFO("[{}] 运行时间:  {}  ms", name, duration);
  }
};

int main() {

  Log::Init();
  LOG_INFO("程序开始运行");

  ScopedTimer timer("generate_mesh_graph_manual");
  // for (int i = 2; i <= 2; i++) {
  //   std::vector<GraphWithMetadata> graphs =
  //       generate_mesh_graph_manual_batch(50000, i);
  //   random_error_inject(graphs, 0.01, 0.03, g_rng);

  //   std::ranges::for_each(
  //       graphs, [](GraphWithMetadata &g) { g.delete_isolated_nodes(); });

  //   auto sub_graphs = graphs | std::views::filter([](GraphWithMetadata &g) {
  //                       return g.has_subgraphs();
  //                     });
  //   auto incomplete_graphs =
  //       graphs |
  //       std::views::filter([](GraphWithMetadata &g) { return !g.is_full();
  //       });
  //   LOG_INFO("i: {}", i);
  //   LOG_INFO("sub_graphs rate: {:.4f}%",
  //            static_cast<float>(std::ranges::distance(sub_graphs)) / 50000 *
  //                100);
  //   LOG_INFO("incomplete_graphs rate: {:.4f}%",
  //            static_cast<float>(std::ranges::distance(incomplete_graphs)) /
  //                50000 * 100);
  //   LOG_INFO("--------------------------------");
  // }

  std::vector<GraphWithMetadata> graphs =
      generate_mesh_graph_manual_batch(5000, 5);
  random_error_inject(graphs, 0.05, 0.09, g_rng);

  std::ranges::sort(graphs, {}, [](GraphWithMetadata &g) { return g.score(); });

  auto num_sub_graphs = graphs |
                        std::views::transform([](GraphWithMetadata &g) {
                          return g.num_components();
                        }) |
                        std::ranges::to<std::set>();

  for (auto num : num_sub_graphs) {
    LOG_INFO("num_sub_graphs: {}", num);
  }

  // int count = 0;
  // std::ranges::for_each(
  //     graphs, [&count](GraphWithMetadata &g) { g.delete_isolated_nodes(); });

  // auto sub_graphs = graphs | std::views::filter([](GraphWithMetadata &g) {
  //                     return g.has_subgraphs();
  //                   });
  // auto incomplete_graphs =
  //     graphs |
  //     std::views::filter([](GraphWithMetadata &g) { return !g.is_full(); });

  // std::ranges::for_each(sub_graphs, [](GraphWithMetadata &g) {
  //   print_mesh_graph(g.graph());
  //   std::println("--------------------------------");
  // });

  return 0;
}