#include <chrono>
#include <string>
#include <print>
#include <random>
#include "mesh.h"
#include "error_inject.h"

static std::mt19937 g_rng(42);

struct ScopedTimer {
    std::string name;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    ScopedTimer(const std::string& func_name) 
        : name(func_name), start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::println("[{}] 运行时间:  {}  ms", name, duration);
    }
};



int main() {

    ScopedTimer timer("generate_mesh_graph_manual");
    std::vector<Graph> graphs = generate_mesh_graph_manual_batch(100000,5);
    // boost::print_graph(g);

    // node_error_inject(graphs[0], 0);

    // edge_error_inject(graphs[0], 23, 24);
    // boost::print_graph(graphs[0]);
    random_error_inject(graphs, 0.0023, 0.01,g_rng);




    return 0;
}