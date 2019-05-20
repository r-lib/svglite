#ifndef __UTILS__
#define __UTILS__

#include <limits>
#include <cmath>

double dbl_format(double x) {
  if (std::abs(x) < std::numeric_limits<double>::epsilon())
    return 0.00;
  else
    return x;
}

#endif
