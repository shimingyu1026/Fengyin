/**
 * @file error_inject.h
 * @brief 错误注入功能声明
 *
 * 提供节点和边的错误注入函数声明
 */

#pragma once

#include "common.h"
#include "mesh.h"

/**
 * @brief 注入节点错误
 * @param g 要操作的图
 * @param node_idx 要删除的节点索引
 */
void node_error_inject(Graph &g, const int node_idx);

/**
 * @brief 注入边错误
 * @param g 要操作的图
 * @param source_idx 源节点索引
 * @param target_idx 目标节点索引
 */
void edge_error_inject(Graph &g, const int source_idx, const int target_idx);

/**
 * @brief 随机错误注入
 * @param graphs 图向量
 * @param node_error_rate 节点错误率（0.0-1.0）
 * @param edge_error_rate 边错误率（0.0-1.0）
 * @param rng 随机数生成器
 */
void random_error_inject(std::vector<GraphWithMetadata> &graphs,
                         const float node_error_rate,
                         const float edge_error_rate, std::mt19937 &rng);