# Add web font imports to an already created SVG file

This function allows you to add web fonts after creation. The result is
the same as using the `web_fonts` argument in
[`svglite()`](https://svglite.r-lib.org/reference/svglite.md). Only SVGs
created with svglite can get web fonts added.

## Usage

``` r
add_web_fonts(filename, web_fonts)
```

## Arguments

- filename:

  The svgfile(s) or `svg` object(s) (as created by
  [`svgstring()`](https://svglite.r-lib.org/reference/svgstring.md)) to
  edit

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

## Value

Invisibly returns `filename`. If any of elements of this were inline
SVGs then these have been modified to include the imports
