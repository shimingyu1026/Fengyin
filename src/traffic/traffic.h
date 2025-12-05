/**
 * @file traffic.h
 * @brief 流量生成和转换相关定义
 *
 * 提供流量节点结构定义、GEMM参数配置以及流量生成和代码转换函数声明
 */

#pragma once

#include <print>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "mesh_data.h"

/// 数据类型到字节大小的映射表
const std::unordered_map<std::string, size_t> TYPE_SIZE_MAP = {
    {"FP32", 4}, {"FLOAT", 4}, {"FP16", 2},  {"BF16", 2},
    {"INT8", 1}, {"UINT8", 1}, {"INT32", 4}, {"FP64", 8}};

/// ScaleSim 仿真中 GEMM 操作的 M 维度可选值
const std::vector<int> M_VALUES = {16};
/// ScaleSim 仿真中 GEMM 操作的 N 维度可选值
const std::vector<int> N_VALUES = {16};
/// ScaleSim 仿真中 GEMM 操作的 K 维度可选值
const std::vector<int> K_VALUES = {16};

/**
 * @brief 流量节点结构体
 *
 * 表示网络中的一个节点及其流量信息，包括发送和接收的流量数据
 */
struct TrafficNode {
  int id;                           ///< 节点ID
  std::string layers;               ///< GEMM层的字符串表示
  std::pair<int, int> send_traffic; ///< 发送流量：<目标节点ID, 数据量>
  std::vector<std::pair<int, int>>
      recv_traffic; ///< 接收流量列表：<发送方节点ID,
                    ///< 数据量>，按发送方ID从大到小排序
  bool is_send_first; ///< 是否先发送后接收（true: 先发送后接收, false:
                      ///< 先接收后发送）

  /**
   * @brief 构造函数
   * @param node_id 节点ID
   * @param layers_str GEMM层的字符串表示
   * @param send 发送流量信息 <目标节点ID, 数据量>
   */
  TrafficNode(int node_id, std::string layers_str, std::pair<int, int> send)
      : id(node_id), layers(std::move(layers_str)), send_traffic(send),
        recv_traffic(), is_send_first(true) {}

  /**
   * @brief 默认构造函数
   * 创建一个无效的节点（id=-1）
   */
  TrafficNode()
      : id(-1), layers(""), send_traffic(0, 0), recv_traffic(),
        is_send_first(true) {}
};

/**
 * @brief 随机生成指定数量的GEMM操作
 * @param numbers GEMM操作的数量
 * @param rng 随机数生成器
 * @param type 数据类型（如 "FP32", "BF16" 等）
 * @return <GEMM名称的字符串表示, 总数据量（字节）>
 */
std::pair<std::string, int> random_gemm_generate(const int numbers,
                                                 std::mt19937 &rng,
                                                 const std::string &type);

/**
 * @brief 为图生成多轮随机流量
 * @param rng 随机数生成器
 * @param graph 图结构
 * @param layer_num 每个节点的GEMM层数
 * @param data_type 数据类型
 * @param round_num 轮数
 * @return 多轮流量节点，每轮是一个 TrafficNode 向量
 */
std::vector<std::vector<TrafficNode>>
random_traffic_generate(std::mt19937 &rng, GraphWithMetadata &graph,
                        int layer_num, const std::string &data_type,
                        const int round_num);

/**
 * @brief 将多轮流量节点转换为C++代码字符串
 * @param traffic_nodes 多轮流量节点
 * @return C++代码字符串，格式为嵌套的初始化列表
 */
std::string
traffic_gen_to_cpp_code(std::vector<std::vector<TrafficNode>> traffic_nodes);