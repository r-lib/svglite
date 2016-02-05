context("Rect")
library(xml2)

style_attr <- function(nodes, attr) {
  style <- xml_attr(nodes, "style")
  ifelse(grepl(sprintf("%s: [^;]*;", attr), style),
         gsub(sprintf(".*%s: ([^;]*);.*", attr), "\\1", style),
         NA_character_)
}

test_that("rects equivalent regardless of direction", {
  x1 <- xmlSVG({
    plot.new()
    rect(0.2, 0.2, 0.8, 0.8)
  })
  x2 <- xmlSVG({
    plot.new()
    rect(0.8, 0.8, 0.2, 0.2)
  })

  rect1 <- xml_attrs(xml_find_all(x1, "./rect")[[2]])
  rect2 <- xml_attrs(xml_find_all(x2, "./rect")[[2]])

  expect_equal(rect1, rect2)
})

test_that("fill and stroke colors", {
  x <- xmlSVG({
    plot.new()
    rect(0.2, 0.2, 0.8, 0.8, col = "blue", border = "red")
  })
  rectangle <- xml_find_all(x, "./rect")[[2]]
  expect_equal(style_attr(rectangle, "fill"), rgb(0, 0, 1))
  expect_equal(style_attr(rectangle, "stroke"), rgb(1, 0, 0))
})
