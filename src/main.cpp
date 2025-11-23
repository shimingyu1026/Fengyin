#include "error_inject.h"
#include "mesh.h"
#include "mesh_utils.h"
#include <chrono>
#include <print>
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
    std::println("[{}] 运行时间:  {}  ms", name, duration);
  }
};

int main() {

  ScopedTimer timer("generate_mesh_graph_manual");
  std::vector<Graph> graphs = generate_mesh_graph_manual_batch(50000, 4);
  random_error_inject(graphs, 0.02, 0.05, g_rng);

  std::ranges::sort(graphs, {}, count_mesh_score);

  // std::ranges::for_each(graphs | std::views::take(100), [](const Graph &g) {
  //   std::println("score: {}", count_mesh_score(g));
  //   print_mesh_graph(g);
  //   std::println("");
  // });
  int count = 0;
  std::ranges::for_each(graphs, [&count](Graph &g) {
    // if (!is_graph_full(g)) {
    //   std::println("score: {}", count_mesh_score(g));
    //   std::println("count: {}", count++);
    //   print_mesh_graph(g);
    //   std::println("");
    // }
    delete_isolated_nodes(g);
  });

  return 0;
}