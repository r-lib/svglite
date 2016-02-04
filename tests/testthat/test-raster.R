context("Raster")
library(xml2)

test_that("raster exists", {
  x <- xmlSVG({
    image(matrix(runif(64), nrow = 8), useRaster = TRUE)
  }, standalone = TRUE)
  ns <- xml_ns(x)

  img <- xml_attr(xml_find_all(x, ".//d1:image", ns = ns), "xlink:href", ns = ns)
  expect_more_than(nchar(img), 1000)
})
