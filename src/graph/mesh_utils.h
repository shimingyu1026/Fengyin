#pragma once

#include "common.h"
#include "mesh.h"
#include "Log.h"
#include <algorithm>
#include <cmath>
#include <filesystem> // C++17 标准库
#include <fstream>
#include <vector>

void print_mesh_graph(const Graph &g);

int count_mesh_score(const Graph &g);

bool has_subgraphs(const Graph &g);

bool is_graph_full(const Graph &g);

void delete_isolated_nodes(Graph &g);

void generate_topology(const std::filesystem::path &file_path, const std::string &name,
                       const int weight, const int pipeline_stage_delay,
                       const GraphWithMetadata &g);