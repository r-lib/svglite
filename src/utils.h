#pragma once

#include <limits>
#include <cmath>
#include <string>

inline static double dbl_format(double x) {
  if (std::abs(x) < std::numeric_limits<double>::epsilon())
    return 0.00;
  else
    return x;
}

inline bool iequals(const std::string& a, const std::string& b) {
  unsigned int sz = a.size();
  if (b.size() != sz) {
    return false;
  }
  for (unsigned int i = 0; i < sz; ++i) {
    if (tolower(a[i]) != tolower(b[i])) {
      return false;
    }
  }
  return true;
}
