devSVG <- function (file = "Rplots.svg", width = 10, height = 8, bg = "white", fg = "black", onefile = TRUE, xmlHeader = TRUE)
{
      dev <- .C("do_SVG", as.character(file),
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