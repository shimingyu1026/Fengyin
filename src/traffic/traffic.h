#pragma once

#include <print>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// 数据类型和大小
const std::unordered_map<std::string, size_t> TYPE_SIZE_MAP = {
    {"FP32", 4}, {"FLOAT", 4}, {"FP16", 2},  {"BF16", 2},
    {"INT8", 1}, {"UINT8", 1}, {"INT32", 4}, {"FP64", 8}};

// ScaleSim 仿真 Gemm 参数
const std::vector<int> M_VALUES = {16, 32, 64, 128, 256, 512, 1024};
const std::vector<int> N_VALUES = {16, 32, 64, 128, 256, 512, 1024};
const std::vector<int> K_VALUES = {16, 32, 64, 128, 256, 512, 1024};

std::pair<std::string, int> random_gemm_generate(const int numbers, std::mt19937 &rng,const std::string &type);