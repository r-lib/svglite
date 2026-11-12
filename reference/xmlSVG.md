# Run plotting code and return svg

This is useful primarily for testing. Requires the `xml2` package.

## Usage

``` r
xmlSVG(code, ..., standalone = FALSE, height = 7, width = 7)
```

## Arguments

- code:

  Plotting code to execute.

- ...:

  Other arguments passed on to
  [`svgstring`](https://svglite.r-lib.org/reference/svgstring.md).

- standalone:

  Produce a standalone svg file? If `FALSE`, omits xml header and
  default namespace.

- height, width:

  Height and width in inches.

## Value

A `xml2::xml_document` object.

## Examples

``` r
if (require("xml2")) {
  x <- xmlSVG(plot(1, axes = FALSE))
  x
  xml_find_all(x, ".//text")
}
#> Loading required package: xml2
#> {xml_nodeset (2)}
#> [1] <text x="266.40" y="485.28" text-anchor="middle" style="font-siz ...
#> [2] <text transform="translate(12.96,244.80) rotate(-90)" text-ancho ...
```
