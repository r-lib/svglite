#' An SVG Graphics Driver
#'
#' This function produces graphics compliant to the current w3 svg XML standard.
#' The driver output is currently NOT specifying a DOCTYPE DTD.
#'
#' @param file The file where output will appear.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param pointsize Default point size.
#' @param standalone Produce a standalone svg file? If \code{FALSE}, omits
#'   xml header and default namespace.
#' @param font_aliases List of font families to be aliased. By
#'   default, \code{sans} is aliased to "Arial", \code{serif} to
#'   "Times" or "Times New Roman" (the latter on Windows), and
#'   \code{mono} to "courier".
#' @param font_extra List of fonts or collection of fonts. These are
#'   used by svglite to compute text metrics in a reproducible way.
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
#' @keywords device
#' @useDynLib svglite
#' @importFrom Rcpp sourceCpp
#' @importFrom gdtools raster_view
#' @export
svglite <- function(file = "Rplots.svg", width = 10, height = 8,
                    bg = "white", pointsize = 12, standalone = TRUE,
                    font_aliases = NULL, font_extra = NULL) {
  font_spec <- font_spec(aliases = font_aliases, extra = font_extra)
  invisible(svglite_(file, bg, width, height, pointsize, standalone, font_spec))
}

#' Access current SVG as a string.
#'
#' This is a variation on \code{\link{svglite}} that makes it easy to access
#' the current value as a string.
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
                      font_aliases = list(), font_extra = list()) {
  font_spec <- font_spec(aliases = font_aliases, extra = font_extra)

  env <- new.env(parent = emptyenv())
  string_src <- svgstring_(env, width = width, height = height, bg = bg,
    pointsize = pointsize, standalone = standalone, font_spec = font_spec)

  function() {
    svgstr <- if(env$is_closed) env$svg_string else get_svg_content(string_src)
    structure(svgstr, class = "svg")
  }
}

#' @export
print.svg <- function(x, ...) cat(x, "\n", sep = "")
