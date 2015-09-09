# RSvgDevice 0.6.4.9000

* Works on Windows

* Output no longer contains dummy `<desc>` element (#4)

* Text is now coloured, and defaults to a sans-serif font.

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
  SVG, either as raw XML or in RStudio/the browser.
