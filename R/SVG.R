#' An SVG Graphics Driver
#'
#' This function produces graphics compliant to the current w3 svg XML
#' standard. The driver output is currently NOT specifying a DOCTYPE DTD.
#'
#' svglite provides two ways of controlling fonts: system fonts
#' aliases and user fonts aliases. Supplying a font alias has two
#' effects. First it determines the \code{font-family} property of all
#' text anchors in the SVG output. Secondly, the font is used to
#' determine the dimensions of graphical elements and has thus an
#' influence on the overall aspect of the plots.
#'
#' By default, fontconfig is used to find fonts corresponding to the R
#' families of fonts (\code{sans}, \code{serif}, \code{mono},
#' \code{symbol}). You can supply the name of an installed font with
#' the \code{system_fonts} argument. Fontconfig will be used to
#' identify where the font location. Use
#' \code{\link[gdtools]{match_family}()} and
#' \code{\link[gdtools]{match_font}()} to determine the font family
#' and the font file matched by fontconfig.
#'
#' You can also provide specific files with the \code{user_fonts}
#' argument. It accepts fonts returned by the fontquiver package as
#' well as lists containing \code{name} and \code{ttf} elements. The
#' name field specifies an alias for the \code{font-family} text
#' property.
#'
#' @param file The file where output will appear.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param pointsize Default point size.
#' @param standalone Produce a standalone svg file? If \code{FALSE}, omits
#'   xml header and default namespace.
#' @param system_fonts Named list of font names to be aliased with
#'   fonts installed on your system. If unspecified, the R default
#'   families \code{sans}, \code{serif}, \code{mono} and \code{symbol}
#'   are aliased to the family returned by
#'   \code{\link[gdtools]{match_family}()}.
#' @param user_fonts Named list of fonts to be aliased with specific
#'   font files. These font files can be fonts from the fontquiver
#'   package or they
#' @references \emph{W3C Scalable Vector Graphics (SVG)}:
#'   \url{http://www.w3.org/Graphics/SVG/Overview.htm8}
#' @author This driver was written by T Jake Luciani
#'   \email{jakeluciani@@yahoo.com} 2012: updated by Matthieu Decorde
#'   \email{matthieu.decorde@@ens-lyon.fr}
#' @seealso \code{\link{pictex}}, \code{\link{postscript}}, \code{\link{Devices}}
#' @examples
#' # Save to file
#' svglite("Rplots.svg")
#' plot(1:11, (-5:5)^2, type = 'b', main = "Simple Example")
#' dev.off()
#'
#' # Supply system font aliases. First check the font can be located:
#' gdtools::match_family("Verdana")
#'
#' # Then supply a list of aliases:
#' fonts <- list(sans = "Verdana", mono = "Times New Roman")
#' svglite("Rplots.svg", system_fonts = fonts)
#' plot.new()
#' text(1, 1, "Some text", family = "mono")
#' dev.off()
#'
#' # To supply user font aliases, use either fontquiver:
#' fonts <- fontquiver::font_families("Liberation")
#'
#' # Or create yourself a list of fonts. First create a font file
#' # object, which is a list with name and ttf elements:
#' myfont <- list(name = "Some Font Serif", ttf = "myfont.ttf")
#' myotherfont <- list(name = "Other Font Mono", ttf = "myotherfont.ttf")
#'
#' # Then create a named tree with R font families (sans, serif, mono,
#' # symbol) in the first level and R faces in the second (plain,
#' # itali bold, bolditalic, symbol).
#' fonts <- list(
#'   sans = list(plain = myfont),
#'   mono = list(italic = myotherfont),
#'   `Foo Bar` = list(plain = myfont, italic = myotherfont)
#' )
#'
#' # This will cause the sans/plain, mono/italic and Foo
#' # Bar/plain/italic faces to be aliased with the font names supplied
#' # in the font objects. Also the supplied fonts will be used to
#' # compute the metrics of the graphical elements.
#' ss <- svgstring(user_fonts = fonts)
#' plot(1:2, main = "Simple Example")
#' text(1, 1, family = "Foo Bar", font = 3)
#' dev.off()
#' ss()
#' @keywords device
#' @useDynLib svglite
#' @importFrom Rcpp sourceCpp
#' @importFrom gdtools raster_view
#' @export
svglite <- function(file = "Rplots.svg", width = 10, height = 8,
                    bg = "white", pointsize = 12, standalone = TRUE,
                    system_fonts = list(), user_fonts = list()) {
  aliases <- validate_aliases(system_fonts, user_fonts)
  invisible(svglite_(file, bg, width, height, pointsize, standalone, aliases))
}

#' Access current SVG as a string.
#'
#' This is a variation on \code{\link{svglite}} that makes it easy to access
#' the current value as a string.
#'
#' See \code{\link{svglite}()} documentation for information about
#' specifying fonts.
#'
#' @param ... Arguments passed on to \code{\link{svglite}}.
#' @return A function with no arguments: call the function to get the
#'   current value of the string.
#' @examples
#' s <- svgstring(); s()
#'
#' plot.new(); s();
#' text(0.5, 0.5, "Hi!"); s()
#' dev.off()
#'
#' s <- svgstring()
#' plot(rnorm(5), rnorm(5))
#' s()
#' dev.off()
#' @inheritParams svglite
#' @export
svgstring <- function(width = 10, height = 8, bg = "white",
                      pointsize = 12, standalone = TRUE,
                      system_fonts = list(), user_fonts = list()) {
  aliases <- validate_aliases(system_fonts, user_fonts)

  env <- new.env(parent = emptyenv())
  string_src <- svgstring_(env, width = width, height = height, bg = bg,
    pointsize = pointsize, standalone = standalone, aliases = aliases)

  function() {
    svgstr <- if(env$is_closed) env$svg_string else get_svg_content(string_src)
    structure(svgstr, class = "svg")
  }
}

#' @export
print.svg <- function(x, ...) cat(x, "\n", sep = "")
