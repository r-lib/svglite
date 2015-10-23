context("Paths")
library(xml2)

test_that("paths with winding fill mode", {
  x <- xmlSVG({
    plot.new()
    polypath(c(.1, .1, .9, .9, NA, .2, .2, .8, .8),
             c(.1, .9, .9, .1, NA, .2, .8, .8, .2),
             col = rgb(0.5, 0.5, 0.5, 0.3), border = rgb(1, 0, 0, 0.3),
             rule = "winding")
  })
  path <- xml_find_all(x, ".//path")
  expect_equal(xml_attr(path, "fill-rule"), "nonzero")
  expect_equal(xml_attr(path, "fill"), rgb(0.5, 0.5, 0.5))
  expect_equal(xml_attr(path, "fill-opacity"), "0.30")
  expect_equal(xml_attr(path, "stroke"), rgb(1, 0, 0))
  expect_equal(xml_attr(path, "stroke-opacity"), "0.30")
})

test_that("paths with evenodd fill mode", {
  x <- xmlSVG({
    plot.new()
    polypath(c(.1, .1, .9, .9, NA, .2, .2, .8, .8),
             c(.1, .9, .9, .1, NA, .2, .8, .8, .2),
             col = rgb(0.5, 0.5, 0.5, 0.3), border = rgb(1, 0, 0, 0.3),
             rule = "evenodd")
  })
  path <- xml_find_all(x, ".//path")
  expect_equal(xml_attr(path, "fill-rule"), "evenodd")
  expect_equal(xml_attr(path, "fill"), rgb(0.5, 0.5, 0.5))
  expect_equal(xml_attr(path, "fill-opacity"), "0.30")
  expect_equal(xml_attr(path, "stroke"), rgb(1, 0, 0))
  expect_equal(xml_attr(path, "stroke-opacity"), "0.30")
})

test_that("paths with no filling color", {
  x <- xmlSVG({
    plot.new()
    polypath(c(.1, .1, .9, .9, NA, .2, .2, .8, .8),
             c(.1, .9, .9, .1, NA, .2, .8, .8, .2),
             col = NA, border = rgb(1, 0, 0, 0.3),
             rule = "winding")
  })
  path <- xml_find_all(x, ".//path")
  expect_equal(xml_attr(path, "fill-rule"), "nonzero")
  expect_equal(xml_attr(path, "fill"), "none")
  expect_equal(xml_attr(path, "stroke"), rgb(1, 0, 0))
  expect_equal(xml_attr(path, "stroke-opacity"), "0.30")
})
