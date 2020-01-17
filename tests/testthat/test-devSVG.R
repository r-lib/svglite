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
  expect_equal(style_attr(xml_find_first(x, ".//rect"), "fill"), "#FFFFFF")
})

test_that("adds background set by device driver", {
  x <- xmlSVG(plot.new(), bg = "red")
  expect_equal(style_attr(xml_find_first(x, ".//rect"), "fill"), rgb(1, 0, 0))
})

test_that("default background respects par", {
  x <- xmlSVG({
    par(bg = "red")
    plot.new()
  })
  expect_equal(style_attr(xml_find_first(x, ".//rect"), "fill"), rgb(1, 0, 0))
})

test_that("if bg is transparent in par(), use device driver background", {
  x <- xmlSVG({
    par(bg = NA)
    plot.new()
  }, bg = "blue")
  style <- xml_text(xml_find_first(x, "//style"))
  expect_match(style, "fill: none;")
  expect_equal(style_attr(xml_find_first(x, ".//rect"), "fill"), rgb(0, 0, 1))
})

test_that("creating multiple pages is identical to creating multiple individual svgs", {

  plot_one <- function() plot(1, 1)
  plot_two <- function() plot(1, 2)
  # strings
  s_multiple <- svgstring(onefile = FALSE)
  plot_one()
  plot_two()
  dev.off()

  s_1 <- svgstring(onefile = FALSE)
  plot_one()
  dev.off()
  s_2 <- svgstring(onefile = FALSE)
  plot_two()
  dev.off()

  # index also in s_x to drop the class attribute
  expect_length(s_multiple(), 2L)
  expect_identical(s_multiple()[1L], s_1()[1L], label = "svgstring first plot")
  expect_identical(s_multiple()[2L], s_2()[1L], label = "svgstring second plot")

  # do the same with devices
  f_multiple <- "test-multiple%03d.svg"
  f_single_1 <- "test-multiple-ref1.svg"
  f_single_2 <- "test-multiple-ref2.svg"

  svglite(f_multiple, onefile = FALSE)
  plot_one()
  plot_two()
  on.exit(dev.off())

  svglite(f_single_1, onefile = FALSE)
  plot_one()
  on.exit(dev.off())

  svglite(f_single_2, onefile = FALSE)
  plot_two()
  on.exit(dev.off())

  f_multiple_1 <- sprintf(f_multiple, 1)
  f_multiple_2 <- sprintf(f_multiple, 2)
  svg_multiple_1 <- readChar(f_multiple_1, file.info(f_multiple_1)$size)
  svg_multiple_2 <- readChar(f_multiple_2, file.info(f_multiple_2)$size)
  svg_single_1 <- readChar(f_single_1, file.info(f_single_1)$size)
  svg_single_2 <- readChar(f_single_2, file.info(f_single_2)$size)
  expect_equal(svg_multiple_1, svg_single_1)
  expect_equal(svg_multiple_2, svg_single_2)

  file.remove(f_multiple_1, f_multiple_2, f_single_1, f_single_2)
})
