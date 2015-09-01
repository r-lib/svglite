#' Run plotting code and return svg
#'
#' This is useful primarily for testing. Requires the \code{xml2} package.
#'
#' @return A \code{xml2::xml_document} object.
#' @param code Plotting code to execute.
#' @param ... Other arguments passed on to \code{\link{devSVG}}.
#' @export
#' @examples
#' if (require("xml2")) {
#'   x <- xmlSVG(plot(1, axes = FALSE))
#'   x
#'   xml_find_all(x, ".//text")
#' }
xmlSVG <- function(code, ...) {
  tmp <- tempfile()
  devSVG(tmp, width = 5, height = 5, standalone = FALSE, ...)
  tryCatch(code,
    finally = dev.off()
  )

  xml2::read_xml(tmp)
}


