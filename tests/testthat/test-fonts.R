
test_that("partial aliases are checked", {
  aliases <- list(
    sans = fontquiver::font_faces("Bitstream Vera", "Sans"),
    mono = fontquiver::font_faces("Bitstream Vera", "Mono")
  )
  missing_faces <- check_aliases(aliases)$families
  expect_equal(missing_faces, c("serif", "symbol"))
})

test_that("throw on malformed alias", {
  expect_error(validate_aliases(list(mono = letters)), "not valid")
})
