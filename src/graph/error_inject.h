#pragma once

#include "common.h"
#include "mesh.h"

void node_error_inject(Graph& g, const int node_idx);
void edge_error_inject(Graph& g, const int source_idx, const int target_idx); 

void random_error_inject(std::vector<Graph>& graphs, const float node_error_rate, const float edge_error_rate,std::mt19937& rng);