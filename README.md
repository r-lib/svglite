
<!-- README.md is generated from README.Rmd. Please edit that file -->

# svglite <a href='https://svglite.r-lib.org'><img src="man/figures/logo.png" align="right" height="131.5"/></a>

<!-- badges: start -->

[![R-CMD-check](https://github.com/r-lib/svglite/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/r-lib/svglite/actions/workflows/R-CMD-check.yaml)
[![CRAN Status
Badge](http://www.r-pkg.org/badges/version/svglite)](https://cran.r-project.org/package=svglite)
[![Codecov test
coverage](https://codecov.io/gh/r-lib/svglite/graph/badge.svg)](https://app.codecov.io/gh/r-lib/svglite)
<!-- badges: end -->

svglite is a graphics device that produces clean svg output, suitable
for use on the web, or hand editing. Compared to the built-in `svg()`,
svglite produces smaller files, and leaves text as is, making it easier
to edit the result after creation. It also supports multiple nice
features such as embedding of web fonts.

## Installation

svglite is available on CRAN using `install.packages("svglite")`. You
can install the development version from github with:

``` r
# install.packages("pak")
pak::pak("r-lib/svglite")
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

bench::mark(svglite_test(), svg_test(), min_iterations = 250, check = FALSE)
#> # A tibble: 2 × 6
#>   expression          min   median `itr/sec` mem_alloc `gc/sec`
#>   <bch:expr>     <bch:tm> <bch:tm>     <dbl> <bch:byt>    <dbl>
#> 1 svglite_test()   2.64ms   2.73ms      362.     627KB    2.92 
#> 2 svg_test()       6.11ms   6.26ms      159.     224KB    0.638
```

### File size

Another point with high relevance when serving SVGs over the web is the
size. `svglite()` produces much smaller files

``` r
# svglite
fs::file_size(tmp1)
#> 75K

# svg
fs::file_size(tmp2)
#> 327K
```

In both cases, compressing to make `.svgz` (gzipped svg) is worthwhile.
svglite supports compressed output directly which will be triggered if
the provided path has a `".svgz"` (or `".svg.gz"`) extension.

``` r
tmp3 <- tempfile(fileext = ".svgz")
svglite(tmp3)
plot(x, y)
invisible(dev.off())

# svglite - svgz
fs::file_size(tmp3)
#> 9.46K
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
`register_variant()` will also be available, as will fonts added with
`add_fonts()` and `require_font()`. If any of these contains
non-standard weights or OpenType features (e.g. ligatures or tabular
numerics) this will be correctly encoded in the style block. systemfonts
also allows you to embed webfont `@imports` in your file to ensure that
the file looks as expected even on systems without the used font
installed. Using `systemfonts::fonts_as_import()` you can generate these
imports automatically, optionally embedding the font data directly in
the file.

## Building svglite

*This section is only relevant for building svglite from scratch, as
opposed to installing from a pre-built package on CRAN.*

Building svglite requires the system dependency libpng. As svglite
doesn’t have any build-time configuration, your R configuration must
point to libpng’s `include` and `lib` folders.

For instance on macOS, install libpng with:

``` sh
brew install libpng
```

And make sure your `~/.R/Makevars` knows about Homebrew’s `include` and
`lib` folders where libpng should now be installed. On arm64 hardware,
this would be:

``` mk
CPPFLAGS += -I/opt/homebrew/include
LDFLAGS += -L/opt/homebrew/lib
```

## Code of Conduct

Please note that the svglite project is released with a [Contributor
Code of Conduct](https://svglite.r-lib.org/CODE_OF_CONDUCT.html). By
contributing to this project, you agree to abide by its terms.
