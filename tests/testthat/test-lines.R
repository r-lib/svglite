context("Lines")

library(xml2)
library(ggplot2)

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
  stroke_dash <- c( "5,5", "1,5", "1,5,5,5", "10,5", "10,5,5,5" )

  # base graphics
  x <- xmlSVG( plot( 1:3, lty = 1, type = "l" ) )
  expect_equal(
    xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray"),
    NA_character_
  )

  expect_identical(
    sapply(
      2:6,
      function( l ) {
        x <- xmlSVG( plot( 1:3, lty = l, type = "l" ) )
        xml_attr(suppressWarnings(xml_find_one(x,"//polyline")), "stroke-dasharray")
      }
    ),
    stroke_dash
  )

  # lattice
  expect_identical(
    sapply(
      2:6,
      function( l ) {
        x <- xmlSVG( { print( lattice::xyplot( x~x, data.frame( x = 1:3 ), lty = l, type = "l" ) ) } )
        xml_attr(suppressWarnings(xml_find_all(x,"//polyline")[2]), "stroke-dasharray")
      }
    ),
    stroke_dash
  )

  # ggplot2
  expect_identical(
    sapply(
      2:6,
      function( l ) {
        x <- xmlSVG({
          print( ggplot( data.frame( x = 1:3 ), aes( x = x,  y = x ) ) + geom_line( lty = l ) )
        })
        # this is brittle
        xml_attr(suppressWarnings(xml_find_all(x,"//polyline"))[19], "stroke-dasharray" )
      }
    ),
    stroke_dash
  )
})
