context("Points")
library(xml2)

style_attr <- function(nodes, attr) {
  style <- xml_attr(nodes, "style")
  ifelse(grepl(sprintf("%s: [^;]*;", attr), style),
         gsub(sprintf(".*%s: ([^;]*);.*", attr), "\\1", style),
         NA_character_)
}

test_that("radius is given in points", {
  x <- xmlSVG({
    plot.new()
    points(0.5, 0.5, cex = 20)
    text(0.5, 0.5, cex = 20)
  })
  circle <- xml_find_all(x, ".//circle")
  expect_equal(xml_attr(circle, "r"), "54.00pt")
})

test_that("points are given stroke and fill", {
  x <- xmlSVG({
    plot.new()
    points(0.5, 0.5, pch = 21, col = "red", bg = "blue", cex = 20)
  })
  circle <- xml_find_all(x, ".//circle")
  expect_equal(style_attr(circle, "stroke"), rgb(1, 0, 0))
  expect_equal(style_attr(circle, "fill"), rgb(0, 0, 1))
})

test_that("points get alpha stroke and fill given stroke and fill", {
  x <- xmlSVG({
    plot.new()
    points(0.5, 0.5, pch = 21, col = rgb(1, 0, 0, 0.1), bg = rgb(0, 0, 1, 0.1), cex = 20)
  })
  circle <- xml_find_all(x, ".//circle")
  expect_equal(style_attr(circle, "stroke"), rgb(1, 0, 0))
  expect_equal(style_attr(circle, "stroke-opacity"), "0.10")
  expect_equal(style_attr(circle, "fill"), rgb(0, 0, 1))
  expect_equal(style_attr(circle, "fill-opacity"), "0.10")
})

test_that("points are given stroke and fill", {
  x <- xmlSVG({
    plot.new()
    points(0.5, 0.5, pch = 21, col = "red", bg = NA, cex = 20)
  })
  style <- xml_text(xml_find_one(x, "//style"))
  expect_match(style, "fill: none;")

  circle <- xml_find_all(x, ".//circle")
  expect_equal(style_attr(circle, "fill"), NA_character_)
})

