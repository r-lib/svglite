context("raster")

test_that("raster exists", {
  x <- xmlSVG({
    r <- as.raster(matrix(hcl(0, 80, seq(40, 80, 10)), nrow = 5, ncol = 4))
    plot(r)
  })

  img <- xml_attr(xml_find_all(x, ".//image"), "xlink:href")
  expect_less_than(22, nchar(img) )
})
