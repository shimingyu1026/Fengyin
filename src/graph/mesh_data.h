#pragma once

#include "common.h"

// --- 节点结构 ---
struct Node {
  int id;
  std::string name;
  bool is_deleted;  // 标记节点是否已被删除（保留索引）

  Node(int id, std::string name) : id(id), name(name), is_deleted(false) {}

  Node() : id(-1), name(""), is_deleted(false) {}
};

// --- 边结构 ---
struct Edge {
  int id;
  std::string name;
  int source_idx;
  int target_idx;

  Edge(int id, std::string name, int source_idx, int target_idx) : id(id), name(name), source_idx(source_idx), target_idx(target_idx) {}

  Edge() : id(-1), name(""), source_idx(-1), target_idx(-1) {}
};



