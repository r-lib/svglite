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
#'   htmlSVG(plot(1:10))
#'   htmlSVG(hist(rnorm(100)))
#' }
htmlSVG <- function(code, ...) {
  tmp <- tempfile()
  devSVG(tmp, width = par("din")[1], height = par("din")[2], ...)
  tryCatch(code,
    finally = dev.off()
  )
  htmltools::browsable(
    htmltools::HTML(paste0(readLines(tmp), collapse = "\n"))
  )
}


