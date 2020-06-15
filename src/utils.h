#ifndef __SVGLITE_UTILS__
#define __SVGLITE_UTILS__

#include <limits>
#include <cmath>

inline static double dbl_format(double x) {
  if (std::abs(x) < std::numeric_limits<double>::epsilon())
    return 0.00;
  else
    return x;
}

#endif
