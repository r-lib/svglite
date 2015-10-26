# RSvgDevice 0.6.4.9000

* Text is now coloured, and defaults the same fonts the R uses (Arial, 
  Times New Roman and Courier). Font metrics are computed using the 
  gdtools package so that `plotmath()` and `strwidth()` now work.

* Stroke and fill are saved in `stroke` and `fill` attributes, rather than
  lumped into the `style` attribute. Transparent colours are now generated with
  `rgba()` rather than using `stroke-opacity` and `fill-opacity` styles. (#16)
  NA fills and colours are translated to "none".

* You can now only produce a single page per device - previously this worked
  but produced incorrect output (#5).

* `par(bg)` now affects the background colour (#8).

* Only the `viewBox` attribute of `<svg>` is set (not `width` and `height`):
  I'm reasonably certain this makes it easier to use in more places. (#12)

* Supports all line dashing (#15)

* Helper functions `xmlSVG()` and `htmlSVG()` make it easier to view generated
  SVG, either as raw XML or in RStudio/the browser. `editSVG()` opens the SVG
  in the OS/system default SVG viewer or editor.

* Rasters are saved by rendering as base64-encoded pngs which are embedded
  in a data url (#2)

* Works on Windows

* Output no longer contains dummy `<desc>` element (#4)

* The `path()` device function has been added to support the R plotting function
  `polypath()`, and it also allows the `showtext` package to render
  fonts correctly on the `devSVG()` device (#36)

* Supports all line end and line join styles (#24)
