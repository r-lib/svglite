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
#' @param system_fonts `r lifecycle::badge('superseded')` *Consider using
#'   [systemfonts::register_font()] instead*. Named list of font
#'   names to be aliased with fonts installed on your system. If unspecified,
#'   the R default families \code{sans}, \code{serif}, \code{mono} and
#'   \code{symbol} are aliased to the family returned by
#'   \code{\link[systemfonts]{font_info}()}.
#' @param user_fonts `r lifecycle::badge('superseded')` *Consider using
#'   [systemfonts::register_font()] instead*. Named list of fonts to
#'   be aliased with font files provided by the user rather than fonts properly
#'   installed on the system. The aliases can be fonts from the fontquiver
#'   package, strings containing a path to a font file, or a list containing
#'   \code{name} and \code{file} elements with \code{name} indicating
#'   the font alias in the SVG output and \code{file} the path to a
#'   font file.
#' @param web_fonts A list containing web fonts to use in the SVG. The fonts
#'   will still need to be available locally on the computer running the code,
#'   but viewers of the final SVG will not need the font if specified as a web
#'   font. Web fonts can either be specified using [font_face()] or given as a
#'   single string in which case they are taken to be URL's for an `@import`
#'   directive to e.g. Google Fonts.
#' @param id A character vector of ids to assign to the generated SVG's. If
#'   creating more SVG files than supplied ids the exceeding SVG's will not have
#'   an id tag and a warning will be thrown.
#' @param fix_text_size Should the width of strings be fixed so that it doesn't
#'   change between svg renderers depending on their font rendering? Defaults to
#'   `TRUE`. If `TRUE` each string will have the `textLength` CSS property set
#'   to the width calculated by systemfonts and
#'   `lengthAdjust='spacingAndGlyphs'`. Setting this to `FALSE` can be
#'   beneficial for heavy post-processing that may change content or style of
#'   strings, but may lead to inconsistencies between strings and graphic
#'   elements that depend on the dimensions of the string (e.g. label borders
#'   and background).
#' @param scaling A scaling factor to apply to the rendered line width and text
#'   size. Useful for getting the right sizing at the dimension that you
#'   need.
#' @param always_valid Should the svgfile be a valid svg file while it is being
#'   written to? Setting this to `TRUE` will incur a considerable performance
#'   hit (>50% additional rendering time) so this should only be set to `TRUE`
#'   if the file is being parsed while it is still being written to.
#' @param file Identical to `filename`. Provided for backward compatibility.
#' @references \emph{W3C Scalable Vector Graphics (SVG)}:
#'   \url{https://www.w3.org/Graphics/SVG/}
#' @author This driver was written by T Jake Luciani
#'   \email{jakeluciani@@yahoo.com} 2012: updated by Matthieu Decorde
#'   \email{matthieu.decorde@@ens-lyon.fr}
#' @seealso \code{\link{pictex}}, \code{\link{postscript}}, \code{\link{Devices}}
#'
#' @examples
#' # Save to file
#' svglite(tempfile("Rplots.svg"))
#' plot(1:11, (-5:5)^2, type = 'b', main = "Simple Example")
#' dev.off()
#'
#' @keywords device
#' @useDynLib svglite, .registration = TRUE
#' @importFrom systemfonts match_font
#' @export
svglite <- function(filename = "Rplot%03d.svg", width = 10, height = 8,
                    bg = "white", pointsize = 12, standalone = TRUE,
                    system_fonts = list(), user_fonts = list(), web_fonts = list(), id = NULL,
                    fix_text_size = TRUE, scaling = 1, always_valid = FALSE, file) {
  if (!missing(file)) {
    filename <- file
  }
  if (invalid_filename(filename))
    stop("invalid 'file': ", filename)
  aliases <- validate_aliases(system_fonts, user_fonts)
  web_fonts <- validate_web_fonts(web_fonts)
  if (is.null(id)) {
    id <- character(0)
  }
  id <- as.character(id)
  invisible(svglite_(filename, bg, width, height, pointsize, standalone, aliases, web_fonts, id, fix_text_size, scaling, always_valid))
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
                      system_fonts = list(), user_fonts = list(),
                      web_fonts = list(), id = NULL, fix_text_size = TRUE,
                      scaling = 1) {
  aliases <- validate_aliases(system_fonts, user_fonts)
  web_fonts <- validate_web_fonts(web_fonts)
  if (is.null(id)) {
    id <- character(0)
  }
  id <- as.character(id)

  env <- new.env(parent = emptyenv())
  string_src <- svgstring_(env, width = width, height = height, bg = bg,
    pointsize = pointsize, standalone = standalone, aliases = aliases,
    webfonts = web_fonts, id = id, fix_text_size, scaling)

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
