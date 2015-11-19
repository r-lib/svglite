#' An SVG Graphics Driver
#'
#' This function produces graphics compliant to the current w3 svg XML standard.
#' The driver output is currently NOT specifying a DOCTYPE DTD.
#'
#' @param file The file where output will appear. There are two special values
#'   that can be recognized by the device: \code{file = ":terminal:"}
#'   prints the content of SVG to the R terminal, and \code{file = ":string:"}
#'   saves the SVG to a character string which can be retrieved by
#'   \code{\link{stringSVG}()} after closing the device. These two special values
#'   will not generate temporary files on the hard disk.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param pointsize Default point size.
#' @param standalone Produce a standalone svg file? If \code{FALSE}, omits
#'   xml header and default namespace.
#' @references \emph{W3C Scalable Vector Graphics (SVG)}:
#'   \url{http://www.w3.org/Graphics/SVG/Overview.htm8}
#' @author This driver was written by T Jake Luciani
#'   \email{jakeluciani@@yahoo.com} 2012: updated by Matthieu Decorde
#'   \email{matthieu.decorde@@ens-lyon.fr}
#' @seealso \code{\link{pictex}}, \code{\link{postscript}}, \code{\link{Devices}}
#' @examples
#' ## Save to file
#' svglite("Rplots.svg")
#' plot(1:11, (-5:5)^2, type = 'b', main = "Simple Example")
#' dev.off()
#'
#' ## Print to R console
#' svglite(":terminal:")
#' plot.new()
#' dev.off()
#'
#' ## Write to a string stream
#' svglite(":string:")
#' plot.new()
#' dev.off()
#' svgstr <- stringSVG()
#' cat(svgstr)
#'
#' @keywords device
#' @useDynLib svglite
#' @importFrom Rcpp sourceCpp
#' @importFrom gdtools raster_view
#' @export
svglite <- function(file = "Rplots.svg", width = 10, height = 8, bg = "white",
                   pointsize = 12, standalone = TRUE) {

  invisible(devSVG_(file, bg, width, height, pointsize, standalone))
}


.pkg_env <- new.env()
## This string will be modified by device functions
.pkg_env$svg_string <- ""


#' Extract the content of SVG as a string
#'
#' This function is used to extract the content of SVG produced by
#' the most recent call of \code{svglite(":string:")},
#' which directly writes the SVG to a string stream
#' and does not generate any temporary files.
#'
#' @return A character string representing the last plot created by \code{svglite(":string:")}.
#' @examples
#' svglite(":string:")
#' plot(rnorm(5), rnorm(5))
#' dev.off()
#' svgstr = stringSVG()
#' cat(svgstr)
#' @export
stringSVG <- function() {
  .pkg_env$svg_string
}
