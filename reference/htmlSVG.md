# Run plotting code and view svg in RStudio Viewer or web browser.

This is useful primarily for testing. Requires the `htmltools` package.

## Usage

``` r
htmlSVG(code, ...)
```

## Arguments

- code:

  Plotting code to execute.

- ...:

  Other arguments passed on to
  [`svgstring`](https://svglite.r-lib.org/reference/svgstring.md).

## Examples

``` r
if (interactive() && require("htmltools")) {
  htmlSVG(plot(1:10))
  htmlSVG(hist(rnorm(100)))
}
```
