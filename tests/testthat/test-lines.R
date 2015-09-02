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


test_that("lines lty becomes stroke-dasharray", {
  # base graphics
  x <- xmlSVG(plot(1:3, lty = 1, type = "l"))
  expect_equal(
    xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray"),
    NA_character_
  )

  dash_array <- function(lty) {
    x <- xmlSVG(plot(1:3, lty = lty, type = "l"))
    xml_attr(suppressWarnings(xml_find_one(x, "//polyline")), "stroke-dasharray")
  }
  expect_equal(dash_array(2), "5,5")
  expect_equal(dash_array(3), "1,5")
  expect_equal(dash_array(4), "1,5,5,5")
  expect_equal(dash_array(5), "10,5")
  expect_equal(dash_array(6), "10,5,5,5")
})
