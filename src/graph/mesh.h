#pragma once

#include "common.h"
#include "mesh_data.h"

GraphWithMetadata generate_mesh_graph_manual(int N);

std::vector<GraphWithMetadata> generate_mesh_graph_manual_batch(const int N, const int batch_size);