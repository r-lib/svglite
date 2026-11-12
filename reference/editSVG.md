# Run plotting code and open svg in OS/system default svg viewer or editor.

This is useful primarily for testing or post-processing the SVG.

## Usage

``` r
editSVG(code, ..., width = NA, height = NA)
```

## Arguments

- code:

  Plotting code to execute.

- ...:

  Other arguments passed on to
  [`svgstring`](https://svglite.r-lib.org/reference/svgstring.md).

- height, width:

  Height and width in inches.

## Examples

``` r
if (interactive()) {
  editSVG(plot(1:10))
  editSVG(contour(volcano))
}
```
