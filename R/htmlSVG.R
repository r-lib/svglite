#' Run plotting code and view svg in RStudio Viewer or web broswer.
#'
#' This is useful primarily for testing. Requires the \code{htmltools}
#' package.
#'
#' @param code Plotting code to execute.
#' @param ... Other arguments passed on to \code{\link{devSVG}}.
#' @export
#' @examples
#' if (require("htmltools")) {
#'   htmlSVG(plot(1, axes = FALSE))
#' }
htmlSVG <- function(code, ...) {
  tmp <- tempfile()
  devSVG(tmp, width = 5, height = 5, useNS = FALSE, ...)
  tryCatch(code,
    finally = dev.off()
  )

  htmltools::browsable(
    htmltools::HTML(paste0(readLines(tmp),collapse="\n"))
  )
}


