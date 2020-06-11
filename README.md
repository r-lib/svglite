
<!-- README.md is generated from README.Rmd. Please edit that file -->

# svglite

<!-- badges: start -->

[![R build
status](https://github.com/r-lib/svglite/workflows/R-CMD-check/badge.svg)](https://github.com/r-lib/svglite/actions)
[![Codecov test
coverage](https://codecov.io/gh/r-lib/svglite/branch/master/graph/badge.svg)](https://codecov.io/gh/r-lib/svglite?branch=master)
[![CRAN Status
Badge](http://www.r-pkg.org/badges/version/svglite)](https://cran.r-project.org/package=svglite)
<!-- badges: end -->

svglite is a graphics device that produces clean svg output, suitable
for use on the web, or hand editing. Compared to the built-in `svg()`,
svglite is considerably faster, produces smaller files, and leaves text
as is.

## Installation

svglite is available on CRAN using `install.packages("svglite")`. You
can install the development version from github with:

``` r
# install.packages("devtools")
devtools::install_github("r-lib/svglite")
```

## Benchmarks

Compared to the base svg device, svglite is quite a bit faster:

``` r
library(svglite)
x <- runif(1e3)
y <- runif(1e3)

tmp1 <- tempfile()
tmp2 <- tempfile()
system.time({
  svglite(tmp1)
  plot(x, y)
  dev.off()
})
#>    user  system elapsed 
#>   0.227   0.073   0.443

system.time({
  svg(tmp2, onefile = TRUE)
  plot(x, y)
  dev.off()
})
#>    user  system elapsed 
#>   0.027   0.012   0.044
```

It also produces considerably smaller files:

``` r
file.size(tmp1) / 1024
#> [1] 131.6221
file.size(tmp2) / 1024
#> [1] 321.4102
```

In both cases, compressing to make `.svgz` is worthwhile:

``` r
gz <- function(in_path, out_path = tempfile()) {
  out <- gzfile(out_path, "w")
  writeLines(readLines(in_path), out)
  close(out)
  
  invisible(out_path)
}
file.size(gz(tmp1)) / 1024
#> [1] 9.908203
file.size(gz(tmp2)) / 1024
#> [1] 38.58789
```
