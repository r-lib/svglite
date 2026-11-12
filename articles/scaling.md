# Scaling Issues

## Scaling SVG outputs

The SVG files produced by svglite include `width` and `height`
attributes on the `<svg>` tag, given in points (`pt`), alongside a
`viewBox` attribute. The `width` and `height` record the natural
dimensions of the figure so that it displays at its intended size by
default. The `viewBox` defines the aspect ratio of the plot (as well as
a *user coordinate system*, see next section) and is the key to making
the figure scale fluidly, as described below.

### Fluid scaling

The viewBox is determined by the `width` and `height` arguments of
svglite’s device functions (with 10’’ x 8’’ the default). Although those
dimensions are supplied in inches, the viewBox’s user coordinate system
is completely unit agnostic. The main effect is thus to determine an
aspect ratio.

Because svglite also records absolute `width` and `height` attributes,
the figure will by default display at its natural size rather than
rescale to its container. To get fluid scaling, you need to override
these attributes. One option is to remove them from the generated SVG,
e.g. with a post-processing step. Another option is to override them
with CSS, which takes precedence over the attributes:

``` css
svg {
  width: 100%;
  height: auto;
}
```

(Internet Explorer additionally requires some CSS tricks to get this
behaviour, see
<https://tympanus.net/codrops/2014/08/19/making-svgs-responsive-with-css/>.)

Aspect ratio is preserved by default when the figure is scaled up or
down. The details of how the aspect ratio is preserved can be adjusted
in multiple ways via the `preserveAspectRatio` attribute. See
<https://www.sarasoueidan.com/blog/svg-coordinate-systems/> for more
information about this property.

Other useful resource: <https://css-tricks.com/scale-svg/>

### Natural scaling

Another strategy is needed in order to scale the figure to make the text
within the SVG consistent with the text in the surrounding web page.
That could be useful, for instance, to create a consistent appearance in
an HTML presentation. Since the user coordinate system defined by the
viewBox is unitless, we need to map the figure to its natural
dimensions. This will ensure a correspondence between the scale of the
figure and that of the web page.

As mentioned above, the natural scale of svglite’s figures is in points
and is determined by the `width` and `height` arguments that you supply
to the device functions (10’’ x 8’’ being the default). Although those
dimensions are specified in inches, the coordinate system is scaled in
points. Counting 72 points per inch, the default SVG surface is thus 720
x 576 pt, which is what the `width` and `height` attributes on the
`<svg>` tag record. Note that the CSS standard defines 12pt to be equal
to 16px, the default size of text in most browsers. Since 12pt is the
default text size in svglite as well, a SVG scaled to its natural
dimensions will appear seamless with web text of 16px. If the text in
your web page has another size, you will have to compute a scale factor
and adjust the dimensions of the SVG accordingly.

Since the natural dimensions are already recorded in the SVG, displaying
a plot at its natural dimensions requires no extra work when the file is
used as-is. If you have overridden the dimensions for fluid scaling, you
can restore natural scaling by enclosing the figure in a `<div>` tag
with appropriate dimensions:

``` html
<div style="width: 720pt; height: 576pt">
  <img src="figure.svg">
</div>
```

Alternatively, you can directly specify the dimensions in the `<img>` or
`<object>` tag that is embedding the figure. Note that the dimension
attributes of those tags do not accept arbitrary units, so you will have
to supply the dimensions in pixels. Just multiply the width and height
measured in points with a factor of 16/12:

``` html
<img src="figure.svg" width="960" height="768">
```

## Internal notes

### Device scaling

As other graphics devices, svglite is scaled in big points (1/72 inch)
rather than pica points (1/72.27 inch). Note that in LaTeX and in the
`grid` graphics system on which ggplot2 is based, points refer to pica
points. Big points are denoted in LaTeX by `bp` and in CSS by `pt`. We
use the latter notation. See
<https://tex.stackexchange.com/a/200968/19755> for some historical
background about these units.

The conversion between device units and physical dimensions is
determined by the DevDesc parameter `ipr`. IPR stands for inches per
raster (native device coordinates are sometimes called rasters in R
terminology) and is set to 1/72 in svglite. The device’s physical
dimensions are set by the following DevDesc parameters (with `width` and
`height` the plot dimensions set by the user in inches):

| Parameter | Value         |
|-----------|---------------|
| `left`    | `0`           |
| `top`     | `0`           |
| `right`   | `width * 72`  |
| `bottom`  | `height * 72` |

A default svglite plot surface is thus 720 x 576 pt.

### Scaling of graphical elements

It is conventional for the fundamental line width (`lwd = 1`) to
correspond to a line width of 1/96 inch and svglite obeys this
convention. Also, like other R graphics devices, svglite interprets all
point sizes directly as big points (e.g. the `ps` graphical parameter
and the `fontsize` argument of device functions). The default font size
is 12pt.

Text metrics are computed by systemfonts, which uses freetype to extract
metrics for each glyph and calculate string dimensions from that. Text
metrics are calculated at 1000dpi based on `cex * ps` font size.

The Base graphics system also makes use of the obscure `cra` parameter
and its relatives (`cin`, `cxy`, and `csi`). `cra` serves as a crude
measure for a default character height and width for the default
fontsize provided when the device is called (12pt in svgilte). The main
effect of this parameter (more specifically, the height component) is to
change the relationship between the margin parameters `mar`/`mai` and
`oma`/`omi`. The margins `mar` and `oma` are specified in line units and
character height is used as a measure of line spacing to convert margins
measured in lines to physical margins. As in other devices, `cra[0]` is
set to `0.9 * pointsize` and `cra[1]` to `1.2 * pointsize`. These
parameters are completely unused in the Grid graphics system.

### SVG output

The SVG output sets up a viewBox (a user coordinate system) with values
scaled in big points.

**viewBox**: The width and height are set to `dd->right` and
`dd->bottom` respectively (these values are determined by the
user-supplied figure `width` and `height`).

**Line width**: `1 lwd` should equal 1/96 inch. svglite gets values
scaled in device coordinates (big points), so the line width is
multiplied by 72/96.

**Text**: gdtools returns metrics scaled in big points so no
transformation is needed. We do need to add `px` units to work around a
rendering bug in Firefox. Note that when a viewBox is set up, a pixel
equals one unit in the user coordinate system and thus actually
represents a big point.
