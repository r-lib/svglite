# RSvgDevice 0.6.4.9000

* Compiles on Windows

* Output no longer contains dummy `<desc>` element (#4)

* Text is now coloured.

* Stroke and fill are saved in `stroke` and `fill` attributes, rather than
  lumped into the `style` attribute. Transparent colours are now generated with
  `rgba()` rather than using `stroke-opacity` and `fill-opacity` styles. (#16)

* NA fills and colours are translated to "none"
