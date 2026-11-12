# Create a font-face specification

Webfonts in SVG and HTML can either be specified manually using the
`@font-face` at-rule, or imported from e.g. Google Fonts using the
`@import` at-rule. `font_face()` helps you create a valid `@font-face`
block for the `web_fonts` argument in
[`svglite()`](https://svglite.r-lib.org/reference/svglite.md) and
[`svgstring()`](https://svglite.r-lib.org/reference/svgstring.md)
functions.

## Usage

``` r
font_face(
  family,
  woff2 = NULL,
  woff = NULL,
  ttf = NULL,
  otf = NULL,
  eot = deprecated(),
  svg = deprecated(),
  local = NULL,
  weight = NULL,
  style = NULL,
  range = NULL,
  variant = NULL,
  stretch = NULL,
  feature_setting = NULL,
  variation_setting = NULL,
  embed = FALSE
)
```

## Arguments

- family:

  The font family name this font should respond to.

- woff2, woff, ttf, otf:

  URLs to the font in different formats. At least one must be given.
  Best browser support is provided by the woff format.

- eot, svg:

  **\[deprecated\]**

- local:

  One or more font names that local installations of the font may have.
  If a local font is found with either of the given names it will be
  used and no download will happen.

- weight:

  An optional value for the `font-weight` descriptor

- style:

  An optional value for the `font-style` descriptor

- range:

  An optional value for the `unicode-range` descriptor Will give the
  range of unicode values that this font will support

- variant:

  An optional value for the `font-variant` descriptor

- stretch:

  An optional value for the `font-stretch` descriptor

- feature_setting:

  An optional value for the `font-feature-settings` descriptor It is
  recommended to avoid using this if possible

- variation_setting:

  An optional value for the `font-variation-settings` descriptor.

- embed:

  Should the font data be embedded directly in the SVG

## Value

A character string with the `@font-face` block.

## Examples

``` r
font_face(
  family = "MyHelvetica",
  ttf = "MgOpenModernaBold.ttf",
  local = c("Helvetica Neue Bold", "HelveticaNeue-Bold"),
  weight = "bold"
)
#>     @font-face {
#>       font-family: "MyHelvetica";
#>       src: local("Helvetica Neue Bold"),
#>            local("HelveticaNeue-Bold"),
#>            url("") format("woff2"),
#>            url("") format("woff"),
#>            url("") format("opentype"),
#>            url("MgOpenModernaBold.ttf") format("truetype");
#>       font-weight: bold;
#>     }
```
