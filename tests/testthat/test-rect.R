library(xml2)

test_that("rects equivalent regardless of direction", {
  x1 <- xmlSVG({
    plot.new()
    rect(0.2, 0.2, 0.8, 0.8)
  })
  x2 <- xmlSVG({
    plot.new()
    rect(0.8, 0.8, 0.2, 0.2)
  })

  rect1 <- xml_attrs(xml_find_all(x1, "./g/rect")[[1]])
  rect2 <- xml_attrs(xml_find_all(x2, "./g/rect")[[1]])

  expect_equal(rect1, rect2)
})

test_that("fill and stroke colors", {
  x <- xmlSVG({
    plot.new()
    rect(0.2, 0.2, 0.8, 0.8, col = "blue", border = "red")
  })
  rectangle <- xml_find_all(x, "./g/g/rect")[[1]]
  expect_equal(style_attr(rectangle, "fill"), rgb(0, 0, 1))
  expect_equal(style_attr(rectangle, "stroke"), rgb(1, 0, 0))
})
