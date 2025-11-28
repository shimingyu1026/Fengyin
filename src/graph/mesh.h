#pragma once

#include "common.h"



Graph generate_mesh_graph_manual(int N);

std::vector<Graph> generate_mesh_graph_manual_batch(const int N, const int batch_size);