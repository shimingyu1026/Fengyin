#pragma once

#include "common.h"
#include "mesh_data.h"

using Graph = boost::adjacency_list<
    boost::vecS,      // EdgeList：使用 std::vector 存储边
    boost::vecS,      // VertexList：使用 std::vector 存储顶点
    boost::undirectedS,// Directed：无向图 (undirected)
    Node,// VertexProperty：顶点属性
    Edge// EdgeProperty：边属性
>;

Graph generate_mesh_graph_manual(int N);

std::vector<Graph> generate_mesh_graph_manual_batch(const int N, const int batch_size);