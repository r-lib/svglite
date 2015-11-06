context("devSVG")
library(xml2)

style_attr <- function(nodes, attr) {
  style <- xml_attr(nodes, "style")
  ifelse(grepl(sprintf("%s: [^;]*;", attr), style),
         gsub(sprintf(".*%s: ([^;]*);.*", attr), "\\1", style),
         NA_character_)
}

test_that("adds default background", {
  x <- xmlSVG(plot.new())
  expect_equal(style_attr(xml_find_one(x, ".//rect"), "fill"), "#FFFFFF")
})

test_that("adds background set by device driver", {
  x <- xmlSVG(plot.new(), bg = "red")
  expect_equal(style_attr(xml_find_one(x, ".//rect"), "fill"), rgb(1, 0, 0))
})

test_that("default background respects par", {
  x <- xmlSVG({
    par(bg = "red")
    plot.new()
  })
  expect_equal(style_attr(xml_find_one(x, ".//rect"), "fill"), rgb(1, 0, 0))
})

test_that("if bg is transparent in par(), use device driver background", {
  x <- xmlSVG({
    par(bg = NA)
    plot.new()
  }, bg = "blue")
  style <- xml_text(xml_find_one(x, "//style"))
  expect_match(style, "fill: none;")
  expect_equal(style_attr(xml_find_one(x, ".//rect"), "fill"), rgb(0, 0, 1))
})

test_that("can only have one page", {
  svglite(tempfile())
  on.exit(dev.off())

  plot.new()
  expect_error(plot.new(), "only supports one page")
})
