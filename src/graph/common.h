/**
 * @file common.h
 * @brief 图数据结构通用定义
 *
 * 定义节点、边和图的基本数据结构
 */

#pragma once

#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graph_utility.hpp>
#include <cmath>
#include <execution>
#include <print>
#include <random>
#include <ranges>
#include <vector>

#include "Log.h"

/**
 * @brief 节点结构
 *
 * 表示图中的节点，包含ID、名称和删除标记
 */
struct Node {
  int id;
  std::string name;
  bool is_deleted; // 标记节点是否已被删除（保留索引）

  Node(int id, std::string name) : id(id), name(name), is_deleted(false) {}

  Node() : id(-1), name(""), is_deleted(false) {}
};

// --- 边结构 ---
struct Edge {
  int id;
  std::string name;
  int source_idx;
  int target_idx;

  Edge(int id, std::string name, int source_idx, int target_idx)
      : id(id), name(name), source_idx(source_idx), target_idx(target_idx) {}

  Edge() : id(-1), name(""), source_idx(-1), target_idx(-1) {}
};

/**
 * @brief 图类型定义
 *
 * 使用 Boost Graph Library 定义的无向图，使用向量存储顶点和边
 */
using Graph =
    boost::adjacency_list<boost::vecS, // EdgeList：使用 std::vector 存储边
                          boost::vecS, // VertexList：使用 std::vector 存储顶点
                          boost::undirectedS, // Directed：无向图 (undirected)
                          Node,               // VertexProperty：顶点属性
                          Edge                // EdgeProperty：边属性
                          >;