#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

template <typename T>
std::vector<std::vector<T>>
matrix_transpose(const std::vector<std::vector<T>> &matrix) {
  // 1. 边界检查：如果矩阵为空，直接返回空
  if (matrix.empty()) {
    return {};
  }

  // 获取原矩阵的行数和列数
  size_t rows = matrix.size();
  size_t cols = matrix[0].size();

  // 2. 边界检查：如果第一行为空，说明是空矩阵
  if (cols == 0) {
    return {};
  }

  // 3. 预分配新矩阵空间
  // 原矩阵是 rows x cols，新矩阵就是 cols x rows
  std::vector<std::vector<T>> result(cols, std::vector<T>(rows));

  // 4. 执行转置操作
  for (size_t i = 0; i < rows; ++i) {
    // 安全检查：如果发现某一行长度不一致（锯齿矩阵），可以抛出异常或处理
    if (matrix[i].size() != cols) {
      throw std::runtime_error(
          "Matrix must be rectangular (all rows must have the same length).");
    }

    for (size_t j = 0; j < cols; ++j) {
      // 核心逻辑：行列互换
      result[j][i] = matrix[i][j];
    }
  }

  return result;
}

template <typename T>
std::vector<std::vector<T>>
matrix_transpose_non_square(const std::vector<std::vector<T>> &matrix) {
  // 专门处理非方阵转置的函数
  // 如果矩阵为空，直接返回空
  if (matrix.empty()) {
    return {};
  }

  // 获取原矩阵的行数和列数
  size_t rows = matrix.size();
  size_t cols = matrix[0].size();

  // 如果第一行为空，说明是空矩阵
  if (cols == 0) {
    return {};
  }

  // 检查是否为方阵，如果是方阵可以抛出异常或返回原矩阵
  // 但这里我们允许方阵，只是专门优化非方阵的情况
  if (rows == cols) {
    // 方阵情况，可以使用原地转置优化，但这里为了通用性还是创建新矩阵
    // 或者可以选择调用原函数
  }

  // 预分配新矩阵空间：原矩阵是 rows x cols，新矩阵就是 cols x rows
  std::vector<std::vector<T>> result(cols, std::vector<T>(rows));

  // 执行转置操作：针对非方阵优化
  for (size_t i = 0; i < rows; ++i) {
    // 安全检查：确保矩阵是矩形的
    if (matrix[i].size() != cols) {
      throw std::runtime_error(
          "Matrix must be rectangular (all rows must have the same length).");
    }

    for (size_t j = 0; j < cols; ++j) {
      // 核心逻辑：行列互换
      result[j][i] = matrix[i][j];
    }
  }

  return result;
}