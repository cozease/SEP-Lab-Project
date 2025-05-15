#pragma once
#include <cmath>

// 设置用于浮点数比较的全局精度
const double EPSILON = 1e-6;

inline bool isEqual(double a, double b) { return std::fabs(a - b) < EPSILON; }

inline bool isLessThan(double a, double b) { return (b - a) > EPSILON; }

inline bool isGreaterThan(double a, double b) { return (a - b) > EPSILON; }

inline bool isLessThanOrEqual(double a, double b) {
  return !isGreaterThan(a, b);
}

inline bool isGreaterThanOrEqual(double a, double b) {
  return !isLessThan(a, b);
}
