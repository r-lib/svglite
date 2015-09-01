#' A SVG Graphics Driver
#'
#' This function produces graphics suitable the current w3 svg XML standard.
#' It currently does not have any font metric information, so the use of
#' \code{\link{plotmath}} is not supported. The driver output is currently NOT
#' specifying a DOCTYPE DTD
#'
#' @param file the file where output will appear.
#' @param width The width of the plot in inches.
#' @param height the height of the plot in inches.
#' @param bg the background color for the plot.
#' @param fg the foreground color for the plot.
#' @param onefile merge plot calls into onefile or separate them to separate pages.
#' @param xmlHeader Print XML header or not.
#' @param useNS Include svg namespace? Not including it makes extracting
#'   elements with xpath much easier.
#' @references \emph{W3C Scalable Vector Graphics (SVG)}:
#'   \url{http://www.w3.org/Graphics/SVG/Overview.htm8}
#' @author This driver was written by T Jake Luciani
#'   \email{jakeluciani@@yahoo.com} 2012: updated by Matthieu Decorde
#'   \email{matthieu.decorde@@ens-lyon.fr}
#' @seealso \code{\link{pictex}}, \code{\link{postscript}}, \code{\link{Devices}}
#' @examples
#' devSVG()
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' @keywords device
#' @useDynLib RSvgDevice do_SVG
#' @export
devSVG <- function(file = "Rplots.svg", width = 10, height = 8, bg = "white",
                   fg = "black", onefile = TRUE, xmlHeader = TRUE,
                   useNS = TRUE) {

  dev <- .C(do_SVG,
    as.character(file),
    as.character(bg),
    as.character(fg),
    as.double(width),
    as.double(height),
    as.logical(FALSE),
    as.logical(xmlHeader),
    as.logical(onefile),
    as.logical(useNS)
  )

  invisible(dev)
}
