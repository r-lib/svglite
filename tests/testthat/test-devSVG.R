library(xml2)

style_attr <- function(nodes, attr) {
  style <- xml_attr(nodes, "style")
  ifelse(grepl(sprintf("%s: [^;]*;", attr), style),
    gsub(sprintf(".*%s: ([^;]*);.*", attr), "\\1", style),
    NA_character_
  )
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
  set.seed(42)
  df <- data.frame(x = rnorm(20), y = rnorm(20))
  plot_one <- function() plot(df$x, df$y)
  plot_two <- function() plot(df$x, df$y + 10)

  # strings
  s_multiple <- svgstring()
  plot_one()
  plot_two()
  dev.off()

  s_1 <- svgstring()
  plot_one()
  dev.off()
  s_2 <- svgstring()
  plot_two()
  dev.off()

  # index also in s_x to drop the class attribute
  expect_length(s_multiple(), 2L)
  expect_identical(s_multiple()[1L], s_1()[1L], label = "svgstring first plot")
  expect_identical(s_multiple()[2L], s_2()[1L], label = "svgstring second plot")

  # same with devices
  dir <- tempdir()
  f_multiple <- file.path(dir, "test-multiple-%03d.svg")
  f_single_1 <- file.path(dir, "test-single-1.svg")
  f_single_2 <- file.path(dir, "test-single-2.svg")
  f_multiple_1 <- sprintf(f_multiple, 1L)
  f_multiple_2 <- sprintf(f_multiple, 2L)
  on.exit(file.remove(f_multiple_1, f_multiple_2, f_single_1, f_single_2))

  svglite(f_multiple)
  plot_one()
  plot_two()
  dev.off()

  svglite(f_single_1)
  plot_one()
  dev.off()

  svglite(f_single_2)
  plot_two()
  dev.off()

  expect_identical(readLines(f_multiple_1), readLines(f_single_1), label = "svglite first plot")
  expect_identical(readLines(f_multiple_2), readLines(f_single_2), label = "svglite second plot")
})

test_that("ensure text leading white space will be rendered", {
  x <- xmlSVG(plot.new())
  expect_true(
    grepl(
      "white-space: pre",
      xml_text(xml_find_first(x, ".//defs/style[@type = 'text/css']"))
    )
  )
  expect_equal(style_attr(xml_find_first(x, ".//rect"), "fill"), "#FFFFFF")
})
