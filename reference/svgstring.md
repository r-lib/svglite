# Access current SVG as a string.

This is a variation on
[`svglite`](https://svglite.r-lib.org/reference/svglite.md) that makes
it easy to access the current value as a string.

## Usage

``` r
svgstring(
  width = 10,
  height = 8,
  bg = "white",
  pointsize = 12,
  standalone = TRUE,
  web_fonts = list(),
  id = NULL,
  fix_text_size = TRUE,
  scaling = 1,
  system_fonts = list(),
  user_fonts = list()
)
```

## Arguments

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

## Value

A function with no arguments: call the function to get the current value
of the string.

## Details

See [`svglite()`](https://svglite.r-lib.org/reference/svglite.md)
documentation for information about specifying fonts.

## Examples

``` r
s <- svgstring()
s()
#> 

plot.new()
s()
#> <?xml version='1.0' encoding='UTF-8' ?>
#> <svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='720.00pt' height='576.00pt' viewBox='0 0 720.00 576.00'>
#> <g class='svglite'>
#> <defs>
#>   <style type='text/css'><![CDATA[
#>     .svglite line, .svglite polyline, .svglite polygon, .svglite path, .svglite rect, .svglite circle {
#>       fill: none;
#>       stroke: #000000;
#>       stroke-linecap: round;
#>       stroke-linejoin: round;
#>       stroke-miterlimit: 10.00;
#>     }
#>     .svglite text {
#>       white-space: pre;
#>     }
#>     .svglite g.glyphgroup path {
#>       fill: inherit;
#>       stroke: none;
#>     }
#>   ]]></style>
#> </defs>
#> <rect width='100%' height='100%' style='stroke: none; fill: #FFFFFF;'/>
#> <defs>
#>   <clipPath id='cpMC4wMHw3MjAuMDB8MC4wMHw1NzYuMDA='>
#>     <rect x='0.00' y='0.00' width='720.00' height='576.00' />
#>   </clipPath>
#> </defs>
#> <g clip-path='url(#cpMC4wMHw3MjAuMDB8MC4wMHw1NzYuMDA=)'>
#> </g>
#> </g>
#> </svg>
text(0.5, 0.5, "Hi!")
s()
#> <?xml version='1.0' encoding='UTF-8' ?>
#> <svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='720.00pt' height='576.00pt' viewBox='0 0 720.00 576.00'>
#> <g class='svglite'>
#> <defs>
#>   <style type='text/css'><![CDATA[
#>     .svglite line, .svglite polyline, .svglite polygon, .svglite path, .svglite rect, .svglite circle {
#>       fill: none;
#>       stroke: #000000;
#>       stroke-linecap: round;
#>       stroke-linejoin: round;
#>       stroke-miterlimit: 10.00;
#>     }
#>     .svglite text {
#>       white-space: pre;
#>     }
#>     .svglite g.glyphgroup path {
#>       fill: inherit;
#>       stroke: none;
#>     }
#>   ]]></style>
#> </defs>
#> <rect width='100%' height='100%' style='stroke: none; fill: #FFFFFF;'/>
#> <defs>
#>   <clipPath id='cpMC4wMHw3MjAuMDB8MC4wMHw1NzYuMDA='>
#>     <rect x='0.00' y='0.00' width='720.00' height='576.00' />
#>   </clipPath>
#> </defs>
#> <g clip-path='url(#cpMC4wMHw3MjAuMDB8MC4wMHw1NzYuMDA=)'>
#> </g>
#> <defs>
#>   <clipPath id='cpNTkuMDR8Njg5Ljc2fDU5LjA0fDUwMi41Ng=='>
#>     <rect x='59.04' y='59.04' width='630.72' height='443.52' />
#>   </clipPath>
#> </defs>
#> <g clip-path='url(#cpNTkuMDR8Njg5Ljc2fDU5LjA0fDUwMi41Ng==)'>
#> <text x='374.40' y='285.15' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='14.67px' lengthAdjust='spacingAndGlyphs'>Hi!</text>
#> </g>
#> </g>
#> </svg>
dev.off()
#> agg_record_184b6d216590 
#>                       2 

s <- svgstring()
plot(rnorm(5), rnorm(5))
s()
#> <?xml version='1.0' encoding='UTF-8' ?>
#> <svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='720.00pt' height='576.00pt' viewBox='0 0 720.00 576.00'>
#> <g class='svglite'>
#> <defs>
#>   <style type='text/css'><![CDATA[
#>     .svglite line, .svglite polyline, .svglite polygon, .svglite path, .svglite rect, .svglite circle {
#>       fill: none;
#>       stroke: #000000;
#>       stroke-linecap: round;
#>       stroke-linejoin: round;
#>       stroke-miterlimit: 10.00;
#>     }
#>     .svglite text {
#>       white-space: pre;
#>     }
#>     .svglite g.glyphgroup path {
#>       fill: inherit;
#>       stroke: none;
#>     }
#>   ]]></style>
#> </defs>
#> <rect width='100%' height='100%' style='stroke: none; fill: #FFFFFF;'/>
#> <defs>
#>   <clipPath id='cpMC4wMHw3MjAuMDB8MC4wMHw1NzYuMDA='>
#>     <rect x='0.00' y='0.00' width='720.00' height='576.00' />
#>   </clipPath>
#> </defs>
#> <g clip-path='url(#cpMC4wMHw3MjAuMDB8MC4wMHw1NzYuMDA=)'>
#> </g>
#> <defs>
#>   <clipPath id='cpNTkuMDR8Njg5Ljc2fDU5LjA0fDUwMi41Ng=='>
#>     <rect x='59.04' y='59.04' width='630.72' height='443.52' />
#>   </clipPath>
#> </defs>
#> <g clip-path='url(#cpNTkuMDR8Njg5Ljc2fDU5LjA0fDUwMi41Ng==)'>
#> <circle cx='280.43' cy='75.47' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='596.48' cy='486.13' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='82.40' cy='268.44' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='546.67' cy='268.01' r='2.70' style='stroke-width: 0.75;' />
#> <circle cx='666.40' cy='273.33' r='2.70' style='stroke-width: 0.75;' />
#> </g>
#> <g clip-path='url(#cpMC4wMHw3MjAuMDB8MC4wMHw1NzYuMDA=)'>
#> <line x1='70.42' y1='502.56' x2='643.19' y2='502.56' style='stroke-width: 0.75;' />
#> <line x1='70.42' y1='502.56' x2='70.42' y2='509.76' style='stroke-width: 0.75;' />
#> <line x1='165.88' y1='502.56' x2='165.88' y2='509.76' style='stroke-width: 0.75;' />
#> <line x1='261.35' y1='502.56' x2='261.35' y2='509.76' style='stroke-width: 0.75;' />
#> <line x1='356.81' y1='502.56' x2='356.81' y2='509.76' style='stroke-width: 0.75;' />
#> <line x1='452.27' y1='502.56' x2='452.27' y2='509.76' style='stroke-width: 0.75;' />
#> <line x1='547.73' y1='502.56' x2='547.73' y2='509.76' style='stroke-width: 0.75;' />
#> <line x1='643.19' y1='502.56' x2='643.19' y2='509.76' style='stroke-width: 0.75;' />
#> <text x='70.42' y='528.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='20.67px' lengthAdjust='spacingAndGlyphs'>-2.5</text>
#> <text x='165.88' y='528.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='20.67px' lengthAdjust='spacingAndGlyphs'>-2.0</text>
#> <text x='261.35' y='528.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='20.67px' lengthAdjust='spacingAndGlyphs'>-1.5</text>
#> <text x='356.81' y='528.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='20.67px' lengthAdjust='spacingAndGlyphs'>-1.0</text>
#> <text x='452.27' y='528.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='20.67px' lengthAdjust='spacingAndGlyphs'>-0.5</text>
#> <text x='547.73' y='528.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='16.67px' lengthAdjust='spacingAndGlyphs'>0.0</text>
#> <text x='643.19' y='528.48' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='16.67px' lengthAdjust='spacingAndGlyphs'>0.5</text>
#> <line x1='59.04' y1='441.64' x2='59.04' y2='95.99' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='441.64' x2='51.84' y2='441.64' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='372.51' x2='51.84' y2='372.51' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='303.38' x2='51.84' y2='303.38' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='234.25' x2='51.84' y2='234.25' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='165.12' x2='51.84' y2='165.12' style='stroke-width: 0.75;' />
#> <line x1='59.04' y1='95.99' x2='51.84' y2='95.99' style='stroke-width: 0.75;' />
#> <text transform='translate(41.76,441.64) rotate(-90.00)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='20.67px' lengthAdjust='spacingAndGlyphs'>-1.5</text>
#> <text transform='translate(41.76,372.51) rotate(-90.00)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='20.67px' lengthAdjust='spacingAndGlyphs'>-1.0</text>
#> <text transform='translate(41.76,303.38) rotate(-90.00)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='20.67px' lengthAdjust='spacingAndGlyphs'>-0.5</text>
#> <text transform='translate(41.76,234.25) rotate(-90.00)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='16.67px' lengthAdjust='spacingAndGlyphs'>0.0</text>
#> <text transform='translate(41.76,165.12) rotate(-90.00)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='16.67px' lengthAdjust='spacingAndGlyphs'>0.5</text>
#> <text transform='translate(41.76,95.99) rotate(-90.00)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='16.67px' lengthAdjust='spacingAndGlyphs'>1.0</text>
#> <polygon points='59.04,502.56 689.76,502.56 689.76,59.04 59.04,59.04 ' style='stroke-width: 0.75;' />
#> <text x='374.40' y='557.28' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='46.02px' lengthAdjust='spacingAndGlyphs'>rnorm(5)</text>
#> <text transform='translate(12.96,280.80) rotate(-90.00)' text-anchor='middle' style='font-size: 12.00px; font-family: "Liberation Sans";' textLength='46.02px' lengthAdjust='spacingAndGlyphs'>rnorm(5)</text>
#> </g>
#> </g>
#> </svg>
dev.off()
#> agg_record_184b6d216590 
#>                       2 
```
