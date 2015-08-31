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
#' @references \emph{W3C Scalable Vector Graphics (SVG)} \url{http://www.w3.org/Graphics/SVG/Overview.htm8}
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
devSVG <- function (file = "Rplots.svg", width = 10, height = 8, bg = "white", fg = "black", onefile = TRUE, xmlHeader = TRUE)
{
      dev <- .C(do_SVG, as.character(file),
             as.character(bg),as.character(fg),
             as.double(width),as.double(height),
             as.logical(FALSE), as.logical(xmlHeader),
             as.logical(onefile),PACKAGE="RSvgDevice"
             )

      invisible(dev)
}

#SvgDevicePoint <- function(x,y)
#{
#       tmp <- .C('GetSvgDevicePoint',as.double(x),as.double(y))
#
#       c(tmp[[1]],tmp[[2]])
#}
#
#SvgUserPoint <- function(x,y)
#{
#       tmp <- .C('GetSvgUserPoint',as.double(x),as.double(y))
#
#       c(tmp[[1]],tmp[[2]])
#}
#
#SvgDeviceBoundry <- function()
#{
#       w <- 0.0
#       h <- 0.0
#       tmp <- .C('GetSvgDeviceBoundry',w,h)
#
#       c(tmp[[1]],tmp[[2]])
#}
#
#SvgDevicePoints <- function(x,y)
#{
#       tmp <- .C('GetSvgDevicePoints',as.double(x),
#                               as.double(y),n=as.integer(length(x)))
#
#       list(x=tmp[[1]],y=tmp[[2]])
#}
#
#MetaSvg <- function(size, box.size, points, values)
#{
#       str <- paste("<MetaSvg width=\"",size[1],"\" height=\"",
#              size[2],"\" rect.width=\"",box.size[1],"\" rect.height=\"",
#              box.size[2],"\">",sep="")
#
#       if(length(points) > 0){
#              buf <- paste("<point x=\"",points$x,"\" y=\"",
#              points$y,"\" value=\"",values,"\"/>",sep="",collapse="\n")
#
#       }
#
#       str <- paste(str,buf,"</MetaSvg>",sep="")
#}
