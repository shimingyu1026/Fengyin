#pragma once

#include "common.h"
#include "mesh.h"

#include <algorithm>
#include <cmath>
#include <vector>

void print_mesh_graph(const Graph &g);

int count_mesh_score(const Graph &g);

bool has_subgraphs(const Graph &g);

bool is_graph_full(const Graph &g);

void delete_isolated_nodes(Graph &g);