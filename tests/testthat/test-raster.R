context("raster")

test_that("raster exists", {
  x <- xmlSVG({
    r <- as.raster(matrix(hcl(0, 80, seq(40, 80, 10)), nrow = 5, ncol = 4))
    plot(r)
  }, standalone = TRUE)
  ns <- xml_ns(x)

  img <- xml_attr(xml_find_all(x, ".//d1:image", ns = ns), "xlink:href", ns = ns)
  expect_more_than(nchar(img), 1000)
})
