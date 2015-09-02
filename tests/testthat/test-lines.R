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
  polygon <- xml_find_one(x, ".//polyline")
  expect_equal(xml_attr(polygon, "fill"), rgb(1, 0, 0))
  expect_equal(xml_attr(polygon, "stroke"), rgb(0, 0, 1))
})

test_that("last point of polygon joined to first point", {
  x <- xmlSVG({
    plot.new()
    polygon(c(0.5, 1, 0.5), c(0, 1, 1))
  })
  points <- strsplit(xml_attr(xml_find_one(x, ".//polyline"), "points"), " ")[[1]]
  expect_equal(length(points), 4)
})

test_that("blank lines are omitted", {
  x <- xmlSVG(mini_plot(1:3, lty = "blank", type = "l"))
  expect_equal(length(xml_find_all(x, "//polyline")), 0)
})

dash_array <- function(...) {
  x <- xmlSVG(mini_plot(1:3, ..., type = "l"))
  dash <- xml_attr(xml_find_one(x, "//polyline"), "stroke-dasharray")
  as.integer(strsplit(dash, " ")[[1]])
}

test_that("lines lty becomes stroke-dasharray", {
  expect_equal(dash_array(lty = 1), NA_integer_)
  expect_equal(dash_array(lty = 2), c(4, 4))
  expect_equal(dash_array(lty = 3), c(1, 3))
  expect_equal(dash_array(lty = 4), c(1, 3, 4, 3))
  expect_equal(dash_array(lty = 5), c(7, 3))
  expect_equal(dash_array(lty = 6), c(2, 2, 6, 2))
  expect_equal(dash_array(lty = "1F"), c(1, 15))
  expect_equal(dash_array(lty = "1234"), c(1, 2, 3, 4))
})

test_that("stroke-dasharray scales with lwd", {
  expect_equal(dash_array(lty = 2), c(4, 4))
  expect_equal(dash_array(lty = 2, lwd = 2), c(8, 8))
})
