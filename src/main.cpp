#include "Log.h"
#include "error_inject.h"
#include "mesh.h"
#include "mesh_utils.h"
#include <chrono>
#include <random>
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
  for (int i = 2; i <= 8; i ++) {
    std::vector<Graph> graphs = generate_mesh_graph_manual_batch(50000, i);
    random_error_inject(graphs, 0.01, 0.03, g_rng);

    std::ranges::for_each(graphs, [](Graph &g) { delete_isolated_nodes(g); });

    auto sub_graphs = graphs | std::views::filter(has_subgraphs);
    auto incomplete_graphs =
        graphs | std::views::filter([](Graph &g) { return !is_graph_full(g); });
    LOG_INFO("i: {}", i);
    LOG_INFO("sub_graphs rate: {:.4f}%", static_cast<float>(std::ranges::distance(sub_graphs)) / 50000 * 100);
    LOG_INFO("incomplete_graphs rate: {:.4f}%", static_cast<float>(std::ranges::distance(incomplete_graphs)) / 50000 * 100);
    LOG_INFO("--------------------------------");
  }

  // std::vector<Graph> graphs = generate_mesh_graph_manual_batch(50000, 4);
  // random_error_inject(graphs, 0.005, 0.02, g_rng);

  // std::ranges::sort(graphs, {}, count_mesh_score);

  // int count = 0;
  // std::ranges::for_each(graphs,
  //                       [&count](Graph &g) { delete_isolated_nodes(g); });

  // auto sub_graphs = graphs | std::views::filter(has_subgraphs);
  // auto incomplete_graphs =
  //     graphs | std::views::filter([](Graph &g) { return !is_graph_full(g);
  //     });

  // std::ranges::for_each(sub_graphs, [](Graph &g) {
  //   print_mesh_graph(g);
  //   std::println("--------------------------------");
  // });

  // std::println("sub_graphs count: {}", std::ranges::distance(sub_graphs));
  // std::println("incomplete_graphs count: {}",
  //              std::ranges::distance(incomplete_graphs));

  return 0;
}