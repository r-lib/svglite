context("devSVG")
library(xml2)

test_that("adds default background", {
  x <- xmlSVG(plot.new())
  expect_equal(xml_attr(xml_find_one(x, ".//rect"), "fill"), "#FFFFFF")
})

test_that("can only have one page", {
  devSVG(tempfile())
  on.exit(dev.off())

  plot.new()
  expect_error(plot.new(), "only supports one page")
})
