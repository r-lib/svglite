RSvgDevice
==========

[![Travis-CI Build Status](https://travis-ci.org/mdecorde/RSvgDevice.svg?branch=master)](https://travis-ci.org/mdecorde/RSvgDevice) [![Coverage Status](https://img.shields.io/codecov/c/github/mdecorde/RSvgDevice/master.svg)](https://codecov.io/github/mdecorde/RSvgDevice?branch=master) [![CRAN\_Status\_Badge](http://www.r-pkg.org/badges/version/RSvgDevice)](http://cran.r-project.org/package=RSvgDevice)

A graphics device that produces clean svg output, suitable for use on the web, and to be edited by hand. Compared to the built-in svg device, this leaves text as is, rather than producing vectorised glyphs.

Installation
------------

Install the released version from CRAN with:

``` r
install.packages("RSvgDevice")
```

Install the development version from github with:

``` r
# install.packages("devtools")
devtools::install_github("mdecorde/RSvgDevice")
```

Benchmarks
----------

Compared to the base svg device, RSvgDevice is quite a bit faster:

``` r
library(RSvgDevice)
x <- runif(1e3)
y <- runif(1e3)

system.time({
  devSVG()
  plot(x, y)
  dev.off()
})
#>    user  system elapsed 
#>   0.003   0.000   0.004

system.time({
  svg(onefile = TRUE)
  plot(x, y)
  dev.off()
})
#>    user  system elapsed 
#>   0.025   0.004   0.029
```
