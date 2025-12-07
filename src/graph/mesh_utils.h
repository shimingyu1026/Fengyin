/**
 * @file mesh_utils.h
 * @brief 网格图工具函数声明
 *
 * 提供网格图的打印、评分、拓扑生成等工具函数
 */

#pragma once

#include "Log.h"
#include "common.h"
#include "mesh.h"
#include <algorithm>
#include <cmath>
#include <filesystem> // C++17 标准库
#include <fstream>
#include <vector>

/**
 * @brief 打印网格图
 * @param g 要打印的图
 */
void print_mesh_graph(const Graph &g);

/**
 * @brief 计算网格图的分数
 * @param g 要计算的图
 * @return 图的分数
 */
int count_mesh_score(const Graph &g);

/**
 * @brief 检查图是否有子图
 * @param g 要检查的图
 * @return 如果存在子图返回 true，否则返回 false
 */
bool has_subgraphs(const Graph &g);

/**
 * @brief 检查图是否完整
 * @param g 要检查的图
 * @return 如果图完整返回 true，否则返回 false
 */
bool is_graph_full(const Graph &g);

/**
 * @brief 删除孤立节点
 * @param g 要操作的图
 */
void delete_isolated_nodes(Graph &g);

/**
 * @brief 生成图的拓扑结构文件
 * @param file_path 输出文件路径
 * @param name 图名称
 * @param weight 边权重
 * @param pipeline_stage_delay 流水线阶段延迟
 * @param g 要生成拓扑的图
 */
void generate_topology(const std::filesystem::path &file_path,
                       const std::string &name, const int weight,
                       const int pipeline_stage_delay,
                       const GraphWithMetadata &g);

/**
 * @brief 批量生成图的拓扑结构文件
 * @param graphs 图向量
 * @param base_path 输出文件的基础路径（目录）
 * @param weight 边权重（默认值为1）
 * @param pipeline_stage_delay 流水线阶段延迟（默认值为1）
 *
 * 为向量中的每个图生成一个拓扑文件，文件名为 graph_0.gv, graph_1.gv, ...
 */
void generate_topology_batch(const std::vector<GraphWithMetadata> &graphs,
                             const std::filesystem::path &base_path,
                             const int weight = 1,
                             const int pipeline_stage_delay = 1);