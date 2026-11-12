# Changelog

## svglite (development version)

## svglite 2.2.2

CRAN release: 2025-10-21

- Avoid sanitizer error when calculating precision
  ([\#198](https://github.com/r-lib/svglite/issues/198))
- Fix a test bug on newer versions of macOS

## svglite 2.2.1

CRAN release: 2025-05-12

- Fix a compilation issue on Fedora

## svglite 2.2.0

CRAN release: 2025-05-07

- Added support for luminance masking
  ([\#174](https://github.com/r-lib/svglite/issues/174))
- Fixed a bug in calculating text widths when using font features
  ([\#175](https://github.com/r-lib/svglite/issues/175))
- Added support for path stroking and filling
  ([\#174](https://github.com/r-lib/svglite/issues/174))
- Added support for groups along with affine transformation and color
  blending. No support for Porter-Duff composition as it is porly
  supported in SVG
  ([\#174](https://github.com/r-lib/svglite/issues/174))
- Added better support for specifying webfonts as embedded in the SVG
  ([\#182](https://github.com/r-lib/svglite/issues/182))
- Added support for glyphs by rendering them as paths or rasters. This
  means the text is not editable in post-production but it is better
  than nothing.

## svglite 2.1.3

CRAN release: 2023-12-08

- Fix a stack imbalance bug

## svglite 2.1.2

CRAN release: 2023-10-11

- Windows: use libpng included with Rtools on R 4.2 and up.
- Add support for
  [`dev.capabilities()`](https://rdrr.io/r/grDevices/dev.capabilities.html)

## svglite 2.1.1

CRAN release: 2023-01-10

- Fix `<cstdint>` include at request of CRAN

## svglite 2.1.0

CRAN release: 2022-02-03

- Add new graphics engine features:

  - Path clipping
  - Masking
  - Pattern and gradient fills

- Font family names are now quoted
  ([\#136](https://github.com/r-lib/svglite/issues/136))

- svglite now renders leading white-space in nodes.
  ([\#131](https://github.com/r-lib/svglite/issues/131),
  [@hrbrmstr](https://github.com/hrbrmstr))

## svglite 2.0.0

CRAN release: 2021-02-20

- svglite now uses systemfonts for text metric calculations and font
  family lookup.

- svglite can now encode OpenType font features into the CSS if the used
  font contains registered features

- svglite now directly encodes raster data into png instead of rendering
  it through cairo. If the provided raster is smaller than the final
  requested size it will be resized.

- SVG’s can now get a top-level id so that style definitions doesn’t
  spill into the surrounding HTML
  ([\#91](https://github.com/r-lib/svglite/issues/91))

- Dimensions are now encoded into the top-level `<svg>` tag
  ([\#90](https://github.com/r-lib/svglite/issues/90))

- Starting a new page creates a new file if the filename supports it
  ([\#98](https://github.com/r-lib/svglite/issues/98),
  [@vandenman](https://github.com/vandenman)).

- The *inline* devices now defaults to the same dimensions as
  [`svglite()`](https://svglite.r-lib.org/reference/svglite.md)
  ([\#89](https://github.com/r-lib/svglite/issues/89))

- Clip defs are now only written if they don’t already exist
  ([\#110](https://github.com/r-lib/svglite/issues/110))

- Clipping is now defined with outer groups instead of on each element
  ([\#109](https://github.com/r-lib/svglite/issues/109))

- svglite now uses cpp11 instead of Rcpp
  ([\#100](https://github.com/r-lib/svglite/issues/100))

- svgz output is now supported natively
  ([\#6](https://github.com/r-lib/svglite/issues/6))

- Text adjustments are now encoded in css where possible
  ([\#107](https://github.com/r-lib/svglite/issues/107))

- The use of textLength CSS property can now be turned off
  ([\#118](https://github.com/r-lib/svglite/issues/118))

- web font imports can now be given when creating an svg
  ([\#108](https://github.com/r-lib/svglite/issues/108))

- Add scaling argument to devices to control line-width and text scaling
  ([\#115](https://github.com/r-lib/svglite/issues/115))

- svg files that are being written are now only valid at all times if
  `always_valid` is set to `TRUE` in the
  [`svglite()`](https://svglite.r-lib.org/reference/svglite.md) call.

- svglite now guards against attempts at writing to the device before a
  new page has been requested
  ([\#126](https://github.com/r-lib/svglite/issues/126))

## svglite 1.2.3

CRAN release: 2020-02-07

- The radius of circles is no longer expressed in pt
  ([\#93](https://github.com/r-lib/svglite/issues/93),
  [@vandenman](https://github.com/vandenman)).

- Dimensions smaller than 1 now retain two significant figures
  ([\#94](https://github.com/r-lib/svglite/issues/94),
  [@ilia-kats](https://github.com/ilia-kats)).

- [@thomasp85](https://github.com/thomasp85) takes over as maintainer

## svglite 1.2.2

CRAN release: 2019-05-17

- Improvements to reproducibility of generated SVGs: Negative zeros are
  now treated as positive, and the clip ID are now generated from
  truncated elements.

- svglite now uses the `polygon` SVG element. This ensures that polygons
  are properly closed
  ([\#82](https://github.com/r-lib/svglite/issues/82)).

- Text metrics are now correctly computed for Unicode characters in
  plotmath expressions
  ([\#81](https://github.com/r-lib/svglite/issues/81)).

## svglite 1.2.1

CRAN release: 2017-09-11

This release makes svglite compatible with gdtools 0.1.6

## svglite 1.2.0

CRAN release: 2016-11-04

### New features

- The device functions gain `system_fonts` and `user_fonts` arguments.

- Two new vignettes: `vignette("fonts")` and
  [`vignette("scaling")`](https://svglite.r-lib.org/articles/scaling.md).
  The vignette on fonts explains in detail how to use the new fonts
  arguments and why. The vignette on scaling goes over scaling issues,
  e.g. when embedding svglite outputs in a web page.

- [`xmlSVG()`](https://svglite.r-lib.org/reference/xmlSVG.md) gains
  `height` and `width` arguments
  ([\#66](https://github.com/r-lib/svglite/issues/66)).

- New [`stringSVG()`](https://svglite.r-lib.org/reference/stringSVG.md)
  device function for quick testing.

### Improvements

- Greatly improves the performance of
  [`svgstring()`](https://svglite.r-lib.org/reference/svgstring.md)
  ([\#58](https://github.com/r-lib/svglite/issues/58)).

- Clip paths now get a unique identifier to avoid collisions when
  multiple plots are included in a document
  ([\#67](https://github.com/r-lib/svglite/issues/67)).

- Raster graphics are now correctly cropped
  ([\#64](https://github.com/r-lib/svglite/issues/64)) and handle
  transparency properly.

- The dimensions of text fields are now hardcoded in the SVGs to prevent
  alignment issues.

### Bug fixes

- [`editSVG()`](https://svglite.r-lib.org/reference/editSVG.md) works
  again ([\#56](https://github.com/r-lib/svglite/issues/56)).

- The dashes in lines with `lwd < 1` are scaled better
  ([\#68](https://github.com/r-lib/svglite/issues/68)).

- Transparent blacks are written correctly
  ([\#62](https://github.com/r-lib/svglite/issues/62),
  [\#63](https://github.com/r-lib/svglite/issues/63)).

- Text is now scaled correctly
  ([\#72](https://github.com/r-lib/svglite/issues/72),
  [\#59](https://github.com/r-lib/svglite/issues/59)). See also the new
  vignette on scaling.

## svglite 1.1.0

CRAN release: 2016-02-09

- Text metrics now converted from points to pixels
  ([\#45](https://github.com/r-lib/svglite/issues/45),
  [\#48](https://github.com/r-lib/svglite/issues/48)) - this fixes text
  alignment issues.

- Intermediate outputs are always valid SVG
  ([\#53](https://github.com/r-lib/svglite/issues/53)).

- New [`svgstring()`](https://svglite.r-lib.org/reference/svgstring.md)
  returns plot as a string
  ([\#40](https://github.com/r-lib/svglite/issues/40),
  [@yixuan](https://github.com/yixuan)).

- Use raster test compatible with older versions of R.

- Add support for [`clip()`](https://rdrr.io/r/graphics/clip.html). This
  also fixes a number of minor issues with grid graphics
  ([@yixuan](https://github.com/yixuan),
  [\#47](https://github.com/r-lib/svglite/issues/47) and
  [\#49](https://github.com/r-lib/svglite/issues/49)).

- Fix incorrect device size
  ([\#50](https://github.com/r-lib/svglite/issues/50)).

## svglite 1.0.0

CRAN release: 2015-12-09

svglite is fully featured svg graphics device that works on all
platforms, forked from RSvgDevice. It supports all graphics device
features:

- All types of line dashing are supported
  ([\#15](https://github.com/r-lib/svglite/issues/15)). All line end and
  line join styles are supported
  ([\#24](https://github.com/r-lib/svglite/issues/24)).

- Text is now coloured, and uses the same default fonts as R (Arial,
  Times New Roman, and Courier). Font metrics are computed using the
  gdtools package so that `plotmath()` and
  [`strwidth()`](https://rdrr.io/r/graphics/strwidth.html) now work.

- Transparent colours are now generated with `rgba()` rather than using
  `stroke-opacity` and `fill-opacity` styles
  ([\#16](https://github.com/r-lib/svglite/issues/16)). NA fills and
  colours are translated to “none”.

- `par(bg)` affects the background colour
  ([\#8](https://github.com/r-lib/svglite/issues/8)).

- Rasters are supported by embedding base64-encoded pngs in a data url
  ([\#2](https://github.com/r-lib/svglite/issues/2)).

- [`polypath()`](https://rdrr.io/r/graphics/polypath.html) is now
  supported, which also allows the `showtext` package to render fonts
  correctly with this device
  ([\#36](https://github.com/r-lib/svglite/issues/36)).

We also made a few other tweaks to the rendered SVG:

- Only the `viewBox` attribute of `<svg>` is set (not `width` and
  `height`): I’m reasonably certain this makes it easier to use in more
  places ([\#12](https://github.com/r-lib/svglite/issues/12)).

- Default styling properties are specified in a global `<style>`
  element: this reduces overall file size, and should make it easier to
  re-style the output for your own needs.

- You can now only produce a single page per device - previously this
  worked but produced incorrect output
  ([\#5](https://github.com/r-lib/svglite/issues/5)).

- Output no longer contains dummy `<desc>` element
  ([\#4](https://github.com/r-lib/svglite/issues/4))

And added some helper functions:

- [`xmlSVG()`](https://svglite.r-lib.org/reference/xmlSVG.md) and
  [`htmlSVG()`](https://svglite.r-lib.org/reference/htmlSVG.md) make it
  easier to view generated SVG, either as raw XML or in RStudio/the
  browser.

- [`editSVG()`](https://svglite.r-lib.org/reference/editSVG.md) opens
  the SVG in the default SVG viewer/editor.
