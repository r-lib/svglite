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
#' influence on the overall aspect of the plots. This means that for
#' optimal display, the font must be available on both the computer
#' used to create the svg, and the computer used to render the
#' svg. See the \code{fonts} vignette for more information.
#'
#' @param filename The file where output will appear.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param pointsize Default point size.
#' @param standalone Produce a standalone svg file? If \code{FALSE}, omits
#'   xml header and default namespace.
#' @param system_fonts Named list of font names to be aliased with
#'   fonts installed on your system. If unspecified, the R default
#'   families \code{sans}, \code{serif}, \code{mono} and \code{symbol}
#'   are aliased to the family returned by
#'   \code{\link[systemfonts]{font_info}()}.
#' @param user_fonts Named list of fonts to be aliased with font files
#'   provided by the user rather than fonts properly installed on the
#'   system. The aliases can be fonts from the fontquiver package,
#'   strings containing a path to a font file, or a list containing
#'   \code{name} and \code{file} elements with \code{name} indicating
#'   the font alias in the SVG output and \code{file} the path to a
#'   font file.
#' @param id A character vector of ids to assign to the generated SVG's. If
#'   creating more SVG files than supplied ids the exceeding SVG's will not have
#'   an id tag and a warning will be thrown.
#' @param file Identical to `filename`. Provided for backward compatibility.
#' @references \emph{W3C Scalable Vector Graphics (SVG)}:
#'   \url{http://www.w3.org/Graphics/SVG/Overview.htm8}
#' @author This driver was written by T Jake Luciani
#'   \email{jakeluciani@@yahoo.com} 2012: updated by Matthieu Decorde
#'   \email{matthieu.decorde@@ens-lyon.fr}
#' @seealso \code{\link{pictex}}, \code{\link{postscript}}, \code{\link{Devices}}
#' @examples
#' # Save to file
#' svglite(tempfile("Rplots.svg"))
#' plot(1:11, (-5:5)^2, type = 'b', main = "Simple Example")
#' dev.off()
#'
#' # Supply system font aliases. First check the font can be located:
#' systemfonts::match_font("Verdana")
#'
#' # Then supply a list of aliases:
#' fonts <- list(sans = "Verdana", mono = "Times New Roman")
#' svglite(tempfile("Rplots.svg"), system_fonts = fonts)
#' plot.new()
#' text(0.5, 0.5, "Some text", family = "mono")
#' dev.off()
#'
#' # See the fonts vignettes for more options to deal with fonts
#'
#' @keywords device
#' @useDynLib svglite
#' @importFrom Rcpp sourceCpp
#' @importFrom systemfonts match_font
#' @export
svglite <- function(filename = "Rplot%03d.svg", width = 10, height = 8,
                    bg = "white", pointsize = 12, standalone = TRUE,
                    system_fonts = list(), user_fonts = list(), id = NULL, file) {
  if (!missing(file)) {
    filename <- file
  }
  if (invalid_filename(filename))
    stop("invalid 'file': ", filename)
  aliases <- validate_aliases(system_fonts, user_fonts)
  if (is.null(id)) {
    id <- character(0)
  }
  id <- as.character(id)
  invisible(svglite_(filename, bg, width, height, pointsize, standalone, aliases, id))
}

#' Access current SVG as a string.
#'
#' This is a variation on \code{\link{svglite}} that makes it easy to access
#' the current value as a string.
#'
#' See \code{\link{svglite}()} documentation for information about
#' specifying fonts.
#'
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
                      system_fonts = list(), user_fonts = list(), id = NULL) {
  aliases <- validate_aliases(system_fonts, user_fonts)
  if (is.null(id)) {
    id <- character(0)
  }
  id <- as.character(id)

  env <- new.env(parent = emptyenv())
  string_src <- svgstring_(env, width = width, height = height, bg = bg,
    pointsize = pointsize, standalone = standalone, aliases = aliases, id = id)

  function() {
    svgstr <- env$svg_string
    if(!env$is_closed) {
      svgstr <- c(svgstr, get_svg_content(string_src))
    }
    structure(svgstr, class = "svg")
  }
}

#' @export
print.svg <- function(x, ...) cat(x, sep = "\n")
