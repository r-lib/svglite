library("grid")

test_that("gradient with repeat extend produces a quoted spreadMethod attribute", {
  x <- xmlSVG(
    {
      grid.newpage()
      grid.rect(
        gp = gpar(
          fill = linearGradient(
            c("red", "blue"),
            x1 = unit(0, "npc"),
            x2 = unit(0.2, "npc"),
            extend = "repeat"
          )
        )
      )
    }
  )
  gradient <- xml2::xml_find_first(x, "//linearGradient")
  expect_equal(xml2::xml_attr(gradient, "spreadMethod"), "repeat")
})

test_that("clip path records all glyphs, not just the first", {
  font_path <- systemfonts::match_fonts("sans")$path[[1]]
  font <- grDevices::glyphFont(font_path, 0, "sans", 400, "normal")
  gid <- systemfonts::shape_string("A", family = "sans")$shape$index
  skip_if(length(gid) == 0, "Could not shape glyph")

  glyphs <- grDevices::glyphInfo(
    id = rep(gid, 2),
    x = c(100, 300),
    y = c(300, 300),
    font = 1L,
    size = 40,
    fontList = grDevices::glyphFontList(sans = font),
    width = grDevices::glyphWidth(30),
    height = grDevices::glyphHeight(40),
    hAnchor = grDevices::glyphAnchor(0, "left"),
    vAnchor = grDevices::glyphAnchor(0, "bottom")
  )
  svg <- stringSVG(
    {
      grid.newpage()
      pushViewport(viewport(clip = glyphGrob(glyphs)))
      grid.rect(gp = gpar(fill = "red"))
    },
    standalone = TRUE
  )

  clip_path <- regmatches(
    svg,
    regexpr("(?s)<clipPath id='cp-\\d+'>.*?</clipPath>", svg, perl = TRUE)
  )
  # Each "A" outline contributes 2 subpaths (outer contour and counter)
  n_subpaths <- lengths(regmatches(
    clip_path,
    gregexpr("M", clip_path, fixed = TRUE)
  ))
  expect_equal(n_subpaths, 4)
})

test_that("fractional text rotation is preserved", {
  x <- xmlSVG({
    plot.new()
    text(0.5, 0.5, "rot", srt = 22.5)
  })
  transform <- xml2::xml_attr(xml2::xml_find_first(x, "//text"), "transform")
  expect_match(transform, "rotate\\(-22\\.5")
})

test_that("svgstring is valid XML while a clip region is active", {
  s <- svgstring()
  plot.new()
  clip(0.2, 0.8, 0.2, 0.8)
  svg <- s()
  dev.off()

  expect_no_error(xml2::read_xml(paste(svg, collapse = "\n")))
})

test_that("upscaling of non-interpolated rasters is size-capped", {
  # Uncapped, this would upscale a 2x2 raster to billions of pixels
  svg <- stringSVG({
    plot.new()
    plot.window(xlim = c(-1e7, 1e7), ylim = c(0, 1))
    rasterImage(matrix("red", 2, 2), -1e7, 0, 1e7, 1, interpolate = FALSE)
  })
  expect_match(svg, "data:image/png;base64,", fixed = TRUE)
  expect_lt(nchar(svg), 1e6)
})

test_that("masks, tiling patterns, groups, and clip paths produce valid SVG", {
  x <- xmlSVG({
    grid.newpage()
    pushViewport(viewport(mask = as.mask(circleGrob(r = 0.4))))
    grid.rect(gp = gpar(fill = "blue"))
    popViewport()
  })
  expect_length(xml2::xml_find_all(x, "//mask"), 1)
  expect_length(xml2::xml_find_all(x, "//*[@mask]"), 1)

  x <- xmlSVG({
    grid.newpage()
    grid.rect(
      gp = gpar(
        fill = pattern(
          circleGrob(r = 0.05),
          width = 0.1,
          height = 0.1
        )
      )
    )
  })
  expect_length(xml2::xml_find_all(x, "//pattern"), 1)
  expect_length(xml2::xml_find_all(x, "//*[contains(@style, 'url(#pat-')]"), 1)

  x <- xmlSVG({
    grid.newpage()
    grid.group(
      rectGrob(gp = gpar(fill = "red")),
      op = "multiply",
      circleGrob(gp = gpar(fill = "blue"))
    )
  })
  expect_length(xml2::xml_find_all(x, "//g[starts-with(@id, 'group-')]"), 1)
  expect_length(xml2::xml_find_all(x, "//use"), 1)

  x <- xmlSVG({
    grid.newpage()
    pushViewport(viewport(clip = circleGrob(r = 0.4)))
    grid.rect(gp = gpar(fill = "red"))
    popViewport()
  })
  expect_length(xml2::xml_find_all(x, "//clipPath"), 2)
})

test_that("device recovers from errors in R callbacks", {
  # Errors in pattern/mask/group/clip callbacks cannot be caught by tryCatch
  # (the graphics engine's own context intercepts the jump), so the erroring
  # draw and the subsequent recovery must run as separate top-level
  # expressions. An interactive R session reading from stdin provides that.
  skip_on_cran()
  skip_if_not_installed("pkgload")

  svg_out <- tempfile(fileext = ".svg")
  script <- tempfile(fileext = ".R")
  pkg_root <- dirname(dirname(test_path()))
  writeLines(
    c(
      sprintf('pkgload::load_all("%s", quiet = TRUE)', pkg_root),
      'library(grid)',
      'drawDetails.errgrob <- function(x, recording) stop("boom in callback")',
      's <- svgstring()',
      'grid.newpage()',
      'pushViewport(viewport(mask = as.mask(circleGrob())))',
      'grid.rect(gp = gpar(fill = "blue"))',
      'popViewport()',
      'pushViewport(viewport(mask = as.mask(grobTree(circleGrob(), grob(cl = "errgrob")))))',
      'grid.rect(gp = gpar(fill = "red"))',
      'popViewport()',
      'grid.text("after error", 0.5, 0.5)',
      sprintf('writeLines(paste(s(), collapse = "\\n"), "%s")', svg_out),
      'dev.off()'
    ),
    script
  )

  system2(
    file.path(R.home("bin"), "R"),
    c("--no-save", "--interactive", "--vanilla"),
    stdin = script,
    stdout = FALSE,
    stderr = FALSE
  )

  expect_true(file.exists(svg_out))
  x <- xml2::read_xml(paste(readLines(svg_out), collapse = "\n"))
  # The failed mask definition was discarded, but the earlier mask and the
  # text drawn after the error are present
  expect_length(xml2::xml_find_all(x, "//*[local-name() = 'mask']"), 1)
  expect_length(
    xml2::xml_find_all(x, "//*[local-name() = 'text'][text() = 'after error']"),
    1
  )
})
