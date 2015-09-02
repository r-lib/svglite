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

test_that("polygons do have fill", {
  x <- xmlSVG({
    plot.new()
    polygon(c(0.5, 1, 0.5), c(0.5, 1, 1), col = "red", border = "blue")
  })
  polygon <- xml_find_one(x, ".//polygon")
  expect_equal(xml_attr(polygon, "fill"), rgb(1, 0, 0))
  expect_equal(xml_attr(polygon, "stroke"), rgb(0, 0, 1))
})

test_that("blank lines are omitted", {
  x <- xmlSVG(mini_plot(1:3, lty = "blank", type = "l"))
  expect_equal(length(xml_find_all(x, "//polyline")), 0)
})

test_that("lines lty becomes stroke-dasharray", {
  dash_array <- function(lty) {
    x <- xmlSVG(mini_plot(1:3, lty = lty, type = "l"))
    xml_attr(xml_find_one(x, "//polyline"), "stroke-dasharray")
  }
  expect_equal(dash_array(1), NA_character_)
  expect_equal(dash_array(2), "4,4")
  expect_equal(dash_array(3), "1,3")
  expect_equal(dash_array(4), "1,3,4,3")
  expect_equal(dash_array(5), "7,3")
  expect_equal(dash_array(6), "2,2,6,2")
  expect_equal(dash_array("1F"), "1,15")
  expect_equal(dash_array("1234"), "1,2,3,4")
})
