#include "traffic.h"
#include <format>
#include <string>

int random_selecte(const std::vector<int> &values, std::mt19937 &rng) {
  // 使用 ranges 随机选择一个元素返回
  std::uniform_int_distribution<> dist(0, std::ranges::ssize(values) - 1);
  return values[dist(rng)];
}

std::pair<std::string, int> _random_gemm_generate(std::mt19937 &rng,
                                                  const std::string &type) {
  int size = TYPE_SIZE_MAP.at(type);

  int m = random_selecte(M_VALUES, rng);
  int n = random_selecte(N_VALUES, rng);
  int k = random_selecte(K_VALUES, rng);

  std::string name = std::format("gemm_{}_{}_{}", m, n, k);

  // GEMM 数据量：输入矩阵 A (m×k) + 输入矩阵 B (k×n) + 输出矩阵 C (m×n)
  int data_size = size * (m * n);

  return std::make_pair(name, data_size);
}

std::pair<std::string, int> random_gemm_generate(const int numbers, std::mt19937 &rng,const std::string &type) {
  std::string ss;
int total_size = 0;
  for (int i = 0; i < numbers; i++) {
    auto [name, size] = _random_gemm_generate(rng, type);
    ss += std::format("{}\n", name);
    total_size += size;
  }
  return std::make_pair(ss, total_size);
}