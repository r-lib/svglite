context("Lines")
library(xml2)

style_attr <- function(nodes, attr) {
  style <- xml_attr(nodes, "style")
  ifelse(grepl(sprintf("%s: [^;]*;", attr), style),
         gsub(sprintf(".*%s: ([^;]*);.*", attr), "\\1", style),
         NA_character_)
}

test_that("segments don't have fill", {
  x <- xmlSVG({
    plot.new()
    segments(0.5, 0.5, 1, 1)
  })
  style <- xml_text(xml_find_one(x, "//style"))
  expect_match(style, "fill: none;")
  expect_equal(style_attr(xml_find_one(x, ".//line"), "fill"), NA_character_)
})

test_that("lines don't have fill", {
  x <- xmlSVG({
    plot.new()
    lines(c(0.5, 1, 0.5), c(0.5, 1, 1))
  })
  expect_equal(style_attr(xml_find_one(x, ".//polyline"), "fill"), NA_character_)
})

test_that("polygons do have fill", {
  x <- xmlSVG({
    plot.new()
    polygon(c(0.5, 1, 0.5), c(0.5, 1, 1), col = "red", border = "blue")
  })
  polygon <- xml_find_one(x, ".//polyline")
  expect_equal(style_attr(polygon, "fill"), rgb(1, 0, 0))
  expect_equal(style_attr(polygon, "stroke"), rgb(0, 0, 1))
})

test_that("polygons without border", {
  x <- xmlSVG({
    plot.new()
    polygon(c(0.5, 1, 0.5), c(0.5, 1, 1), col = "red", border = NA)
  })
  polygon <- xml_find_one(x, ".//polyline")
  expect_equal(style_attr(polygon, "fill"), rgb(1, 0, 0))
  expect_equal(style_attr(polygon, "stroke"), "none")
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
  dash <- style_attr(xml_find_one(x, "//polyline"), "stroke-dasharray")
  as.integer(strsplit(dash, ",")[[1]])
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

test_that("line end shapes", {
  x1 <- xmlSVG({
    plot.new()
    lines(c(0.3, 0.7), c(0.5, 0.5), lwd = 15, lend = "round")
  })
  x2 <- xmlSVG({
    plot.new()
    lines(c(0.3, 0.7), c(0.5, 0.5), lwd = 15, lend = "butt")
  })
  x3 <- xmlSVG({
    plot.new()
    lines(c(0.3, 0.7), c(0.5, 0.5), lwd = 15, lend = "square")
  })
  style <- xml_text(xml_find_one(x1, "//style"))
  expect_match(style, "stroke-linecap: round;")
  expect_equal(style_attr(xml_find_one(x1, ".//polyline"), "stroke-linecap"), NA_character_)
  expect_equal(style_attr(xml_find_one(x2, ".//polyline"), "stroke-linecap"), "butt")
  expect_equal(style_attr(xml_find_one(x3, ".//polyline"), "stroke-linecap"), "square")
})

test_that("line join shapes", {
  x1 <- xmlSVG({
    plot.new()
    lines(c(0.3, 0.5, 0.7), c(0.1, 0.9, 0.1), lwd = 15, ljoin = "round")
  })
  x2 <- xmlSVG({
    plot.new()
    lines(c(0.3, 0.5, 0.7), c(0.1, 0.9, 0.1), lwd = 15, ljoin = "mitre", lmitre = 10)
  })
  x3 <- xmlSVG({
    plot.new()
    lines(c(0.3, 0.5, 0.7), c(0.1, 0.9, 0.1), lwd = 15, ljoin = "mitre", lmitre = 4)
  })
  x4 <- xmlSVG({
    plot.new()
    lines(c(0.3, 0.5, 0.7), c(0.1, 0.9, 0.1), lwd = 15, ljoin = "bevel")
  })
  style <- xml_text(xml_find_one(x1, "//style"))
  expect_match(style, "stroke-linejoin: round;")
  expect_match(style, "stroke-miterlimit: 10.00;")
  expect_equal(style_attr(xml_find_all(x1, ".//polyline"), "stroke-linejoin"), NA_character_)
  expect_equal(style_attr(xml_find_all(x2, ".//polyline"), "stroke-linejoin"), "miter")
  expect_equal(style_attr(xml_find_all(x2, ".//polyline"), "stroke-miterlimit"), NA_character_)
  expect_equal(style_attr(xml_find_all(x3, ".//polyline"), "stroke-linejoin"), "miter")
  expect_equal(style_attr(xml_find_all(x3, ".//polyline"), "stroke-miterlimit"), "4.00")
  expect_equal(style_attr(xml_find_all(x4, ".//polyline"), "stroke-linejoin"), "bevel")
})
