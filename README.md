
<!-- README.md is generated from README.Rmd. Please edit that file -->

# svglite <a href='https://svglite.r-lib.org'><img src="man/figures/logo.png" align="right" height="131.5"/></a>

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
svglite produces smaller files, and leaves text as is, making it easier
to edit the result after creation. It also support multiple nice
features such as embedding of web fonts.

## Installation

svglite is available on CRAN using `install.packages("svglite")`. You
can install the development version from github with:

``` r
# install.packages("devtools")
devtools::install_github("r-lib/svglite")
```

## Motivation

The grDevices package bundled with R already comes with an SVG device
(using the eponymous `svg()` call). The development of svglite is
motivated by the following considerations:

### Speed

`svglite()` is considerably faster than `svg()`. If you are rendering
SVGs dynamically to serve over the web this can be quite important:

``` r
library(svglite)

x <- runif(1e3)
y <- runif(1e3)
tmp1 <- tempfile()
tmp2 <- tempfile()

svglite_test <- function() {
  svglite(tmp1)
  plot(x, y)
  dev.off()
}
svg_test <- function() {
  svg(tmp2, onefile = TRUE)
  plot(x, y)
  dev.off()
}

bench::mark(svglite_test(), svg_test(), min_iterations = 250)
#> # A tibble: 2 x 6
#>   expression          min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>     <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 svglite_test()   3.31ms   3.87ms     247.      561KB     5.05
#> 2 svg_test()      10.57ms  12.19ms      76.3     177KB     1.24
```

### File size

Another point with high relevance when serving SVGs over the web is the
size. `svglite()` produces much smaller files

``` r
# svglite
fs::file_size(tmp1)
#> 74.9K

# svg
fs::file_size(tmp2)
#> 321K
```

In both cases, compressing to make `.svgz` (gzipped svg) is worthwhile.
svglite supports compressed output directly which will be triggered if
the provided path has a `".svgz"` extension.

``` r
tmp3 <- tempfile(fileext = ".svgz")
svglite(tmp3)
plot(x, y)
invisible(dev.off())

# svglite - svgz
fs::file_size(tmp3)
#> 9.38K
```

### Editability

One of the main reasons for the size difference between the size of the
output of `svglite()` and `svg()` is the fact that `svglite()` encodes
text as styled `<text>` elements, whereas `svg()` converts the glyphs to
polygons and renders these. The latter approach means that the output of
`svg()` does not require the font to be present on the system that
displays the SVG but makes it more or less impossible to edit the text
after the fact. svglite focuses on providing maximal editability of the
output, so that you can open up the result in a vector drawing program
such as Inkscape or Illustrator and polish the output if you so choose.

### Font support

svglite uses systemfonts for font discovery which means that all
installed fonts on your system is available to use. The systemfonts
foundation means that fonts registered with `register_font()` or
`register_variant()` will also be available. If any of these contains
non-standard weights or OpenType features (e.g. ligatures or tabular
numerics) this will be correctly encoded in the style block. systemfonts
also allows you to embed webfont `@imports` in your file to ensure that
the file looks as expected even on systems without the used font
installed.

## Code of Conduct

Please note that the svglite project is released with a [Contributor
Code of Conduct](https://svglite.r-lib.org/CODE_OF_CONDUCT.html). By
contributing to this project, you agree to abide by its terms.
