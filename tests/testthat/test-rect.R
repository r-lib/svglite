context("rect")

test_that("rects equivalent regardless of direction", {
  x1 <- xmlSVG({
    plot.new()
    rect(0.2, 0.2, 0.8, 0.8)
  })
  x2 <- xmlSVG({
    plot.new()
    rect(0.8, 0.8, 0.2, 0.2)
  })

  rect1 <- xml_attrs(xml_find_all(x1, ".//rect")[[2]])
  rect2 <- xml_attrs(xml_find_all(x2, ".//rect")[[2]])

  expect_equal(rect1, rect2)
})

