#' Run plotting code and view svg in RStudio Viewer or web broswer.
#'
#' This is useful primarily for testing. Requires the \code{htmltools}
#' package.
#'
#' @inheritParams svglite
#' @param code Plotting code to execute.
#' @param ... Other arguments passed on to \code{\link{svglite}}.
#' @export
#' @examples
#' if (require("htmltools")) {
#'   htmlSVG(plot(1:10))
#'   htmlSVG(hist(rnorm(100)))
#' }
htmlSVG <- function(code, ...) {
  svg <- inlineSVG(code, ...)
  htmltools::browsable(
    htmltools::HTML(svg)
  )
}

#' Run plotting code and return svg
#'
#' This is useful primarily for testing. Requires the \code{xml2} package.
#'
#' @return A \code{xml2::xml_document} object.
#' @inheritParams htmlSVG
#' @inheritParams svglite
#' @export
#' @examples
#' if (require("xml2")) {
#'   x <- xmlSVG(plot(1, axes = FALSE))
#'   x
#'   xml_find_all(x, ".//text")
#' }
xmlSVG <- function(code, ..., standalone = FALSE, height = 7, width = 7) {
  plot <- inlineSVG(code, ...,
    standalone = standalone,
    height = height,
    width = width
  )
  xml2::read_xml(plot)
}

#' Run plotting code and open svg in OS/system default svg viewer or editor.
#'
#' This is useful primarily for testing or post-processing the SVG.
#'
#' @inheritParams htmlSVG
#' @inheritParams svglite
#' @export
#' @examples
#' if (interactive()) {
#'   editSVG(plot(1:10))
#'   editSVG(contour(volcano))
#' }
editSVG <- function(code, ..., width = NA, height = NA) {
  dim <- plot_dim(c(width, height))

  tmp <- tempfile(fileext = ".svg")
  svglite(tmp, width = dim[1], height = dim[2], ...)
  tryCatch(code,
    finally = grDevices::dev.off()
  )

  system(sprintf("open %s", shQuote(tmp)))
}

#' Run plotting code and return svg as string
#'
#' This is useful primarily for testing but can be used as an
#' alternative to \code{\link{svgstring}()}.
#'
#' @inheritParams htmlSVG
#' @export
#' @examples
#' stringSVG(plot(1:10))
stringSVG <- function(code, ...) {
  svg <- inlineSVG(code, ...)
  structure(svg, class = "svg")
}

inlineSVG <- function(code, ..., width = NA, height = NA) {
  dim <- plot_dim(c(width, height))

  svg <- svgstring(width = dim[1], height = dim[2], ...)
  tryCatch(code,
    finally = grDevices::dev.off()
  )

  out <- svg()
  class(out) <- NULL
  out
}
