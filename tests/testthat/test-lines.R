context("Lines")

library(xml2)
test_that("segments don't have fill", {
  x <- xmlSVG({
    plot.new()
    segments(0.5, 0.5, 1, 1)
  })
  expect_equal(xml_attr(xml_find_one(x, ".//line"), "fill"), NA_character_)
})

test_that("lines don't have fill", {
  x <- xmlSVG({
    plot.new()
    lines(c(0.5, 1, 0.5), c(0.5, 1, 1))
  })
  expect_equal(xml_attr(xml_find_one(x, ".//polyline"), "fill"), "none")
})
