#include "SvgStream.h"

template <>
SvgStream& operator<<(SvgStream& object, const double& data) {
  // Make sure negative zeros are converted to positive zero for
  // reproducibility of SVGs
  object.write(dbl_format(data));
  return object;
}
