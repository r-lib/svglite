# svglite

[![Travis-CI Build Status](https://travis-ci.org/hadley/svglite.svg?branch=master)](https://travis-ci.org/hadley/svglite) [![Coverage Status](https://img.shields.io/codecov/c/github/hadley/svglite/master.svg)](https://codecov.io/github/hadley/svglite?branch=master) [![CRAN\_Status\_Badge](http://www.r-pkg.org/badges/version/svglite)](https://cran.r-project.org/package=svglite)

svglite is a graphics device that produces clean svg output, suitable for use on the web, or hand editing. Compared to the built-in `svg()`, svglite is considerably faster, produces smaller files, and leaves text as is.

## Installation

Install the development version from github with:

```R
# install.packages("devtools")
devtools::install_github("hadley/svglite")
```

## Benchmarks

Compared to the base svg device, svglite is quite a bit faster:

```R
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
#>   0.003   0.001   0.003 

system.time({
  svg(tmp2, onefile = TRUE)
  plot(x, y)
  dev.off()
})
#>    user  system elapsed 
#>   0.015   0.001   0.017 
```

It also produces considerably smaller files:

``` r
file.size(tmp1) / 1024
#> [1] 93.54785
file.size(tmp2) / 1024
#> [1] 321.1357
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
#> [1] 9.064453
file.size(gz(tmp2)) / 1024
#> [1] 38.6123
```
