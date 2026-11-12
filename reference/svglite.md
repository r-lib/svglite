# An SVG Graphics Driver

This function produces graphics compliant to the current w3 svg XML
standard. The driver output is currently NOT specifying a DOCTYPE DTD.

## Usage

``` r
svglite(
  filename = "Rplot%03d.svg",
  width = 10,
  height = 8,
  bg = "white",
  pointsize = 12,
  standalone = TRUE,
  web_fonts = list(),
  id = NULL,
  fix_text_size = TRUE,
  scaling = 1,
  always_valid = FALSE,
  file,
  system_fonts = list(),
  user_fonts = list()
)
```

## Arguments

- filename:

  The file where output will appear.

- height, width:

  Height and width in inches.

- bg:

  Default background color for the plot (defaults to "white").

- pointsize:

  Default point size.

- standalone:

  Produce a standalone svg file? If `FALSE`, omits xml header and
  default namespace.

- web_fonts:

  A list containing web fonts to use in the SVG. The fonts will still
  need to be available locally on the computer running the code, but
  viewers of the final SVG will not need the font if specified as a web
  font. Web fonts can either be specified using
  [`font_face()`](https://svglite.r-lib.org/reference/font_face.md) or
  given as a single string in which case they are taken to be URL's for
  an `@import` directive to e.g. Google Fonts. For the latter, you can
  use
  [`fonts_as_import()`](https://systemfonts.r-lib.org/reference/fonts_as_import.html)
  to automatically generate the string, optionally embedding the font
  data in it. If the passed in string is not in the form of a URL or
  `@import` statement then it is considered a font family name and
  [`fonts_as_import()`](https://systemfonts.r-lib.org/reference/fonts_as_import.html)
  will be called to convert it to an import automatically, using the
  default arguments.

- id:

  A character vector of ids to assign to the generated SVG's. If
  creating more SVG files than supplied ids the exceeding SVG's will not
  have an id tag and a warning will be thrown.

- fix_text_size:

  Should the width of strings be fixed so that it doesn't change between
  svg renderers depending on their font rendering? Defaults to `TRUE`.
  If `TRUE` each string will have the `textLength` CSS property set to
  the width calculated by systemfonts and
  `lengthAdjust='spacingAndGlyphs'`. Setting this to `FALSE` can be
  beneficial for heavy post-processing that may change content or style
  of strings, but may lead to inconsistencies between strings and
  graphic elements that depend on the dimensions of the string (e.g.
  label borders and background).

- scaling:

  A scaling factor to apply to the rendered line width and text size.
  Useful for getting the right sizing at the dimension that you need.

- always_valid:

  Should the svgfile be a valid svg file while it is being written to?
  Setting this to `TRUE` will incur a considerable performance hit
  (\>50% additional rendering time) so this should only be set to `TRUE`
  if the file is being parsed while it is still being written to.

- file:

  Identical to `filename`. Provided for backward compatibility.

- system_fonts:

  **\[superseded\]** *Consider using
  [`systemfonts::register_font()`](https://systemfonts.r-lib.org/reference/register_font.html)
  instead*. Named list of font names to be aliased with fonts installed
  on your system. If unspecified, the R default families `sans`,
  `serif`, `mono` and `symbol` are aliased to the family returned by
  [`font_info()`](https://systemfonts.r-lib.org/reference/font_info.html).

- user_fonts:

  **\[superseded\]** *Consider using
  [`systemfonts::register_font()`](https://systemfonts.r-lib.org/reference/register_font.html)
  instead*. Named list of fonts to be aliased with font files provided
  by the user rather than fonts properly installed on the system. The
  aliases can be fonts from the fontquiver package, strings containing a
  path to a font file, or a list containing `name` and `file` elements
  with `name` indicating the font alias in the SVG output and `file` the
  path to a font file.

## Details

svglite provides two ways of controlling fonts: system fonts aliases and
user fonts aliases. Supplying a font alias has two effects. First it
determines the `font-family` property of all text anchors in the SVG
output. Secondly, the font is used to determine the dimensions of
graphical elements and has thus an influence on the overall aspect of
the plots. This means that for optimal display, the font must be
available on both the computer used to create the svg, and the computer
used to render the svg. See the `fonts` vignette for more information.

## References

*W3C Scalable Vector Graphics (SVG)*:
<https://www.w3.org/groups/wg/svg/>

## See also

[`pictex`](https://rdrr.io/r/grDevices/pictex.html),
[`postscript`](https://rdrr.io/r/grDevices/postscript.html),
[`Devices`](https://rdrr.io/r/grDevices/Devices.html)

## Author

This driver was written by T Jake Luciani <jakeluciani@yahoo.com> 2012:
updated by Matthieu Decorde <matthieu.decorde@ens-lyon.fr>

## Examples

``` r
# Save to file
svglite(tempfile("Rplots.svg"))
plot(1:11, (-5:5)^2, type = "b", main = "Simple Example")
dev.off()
#> agg_record_1bd865ed6f7b 
#>                       2 
```
