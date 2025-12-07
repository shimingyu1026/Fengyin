/**
 * @file mesh.h
 * @brief 网格图生成函数声明
 *
 * 提供网格图的生成和批量生成函数声明
 */

#pragma once

#include "common.h"
#include "mesh_data.h"

/**
 * @brief 手动生成网格图
 * @param N 网格大小（N x N）
 * @return 生成的网格图
 */
GraphWithMetadata generate_mesh_graph_manual(int N);

/**
 * @brief 批量生成网格图
 * @param N 生成图的数量
 * @param batch_size 每个网格图的大小（batch_size x batch_size）
 * @return 生成的网格图向量
 */
std::vector<GraphWithMetadata>
generate_mesh_graph_manual_batch(const int N, const int batch_size);