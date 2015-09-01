context("Points")

test_that("points are given stroke and fill", {
  x <- xmlSVG({
    plot.new()
    points(0.5, 0.5, pch = 21, col = "red", bg = "blue", cex = 20)
  })
  circle <- xml_find_all(x, ".//circle")
  expect_equal(xml_attr(circle, "stroke"), rgb(1, 0, 0))
  expect_equal(xml_attr(circle, "fill"), rgb(0, 0, 1))
})

test_that("points get alpha stroke and fill given stroke and fill", {
  x <- xmlSVG({
    plot.new()
    points(0.5, 0.5, pch = 21, col = rgb(1, 0, 0, 0.1), bg = rgb(0, 0, 1, 0.1), cex = 20)
  })
  circle <- xml_find_all(x, ".//circle")
  expect_equal(xml_attr(circle, "stroke"), "rgba(255, 0, 0, 0.10)")
  expect_equal(xml_attr(circle, "fill"), "rgba(0, 0, 255, 0.10)")
})
