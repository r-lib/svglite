
r_font_families <- c("sans", "serif", "mono", "symbol")
r_font_faces <- c("plain", "bold", "italic", "bolditalic", "symbol")

alias_lookup <- function() {
  if (.Platform$OS.type == "windows") {
    serif_font <- "Times New Roman"
    symbol_font <- "Standard Symbols L"
  } else {
    serif_font <- "Times"
    symbol_font <- "Symbol"
  }
  c(
    sans = "Arial",
    serif = serif_font,
    mono = "Courier",
    symbol = symbol_font
  )
}

#' @importFrom systemfonts font_info
match_family <- function(font, bold = FALSE, italic = FALSE) {
  font_info(font, bold = bold, italic = italic)$family[1]
}

validate_aliases <- function(system_fonts, user_fonts) {
  system_fonts <- compact(lapply(system_fonts, compact))
  user_fonts <- compact(lapply(user_fonts, compact))

  system_fonts <- lapply(system_fonts, validate_system_alias)
  user_fonts <- ilapply(user_fonts, validate_user_alias)

  aliases <- c(names(system_fonts), names(user_fonts))
  if (any(duplicated(aliases))) {
    stop("Cannot supply both system and font alias", call. = FALSE)
  }

  # Add missing system fonts for base families
  missing_aliases <- setdiff(r_font_families, aliases)
  system_fonts[missing_aliases] <- lapply(alias_lookup()[missing_aliases], match_family)

  list(
    system = system_fonts,
    user = user_fonts
  )
}

validate_system_alias <- function(alias) {
  if (!is_scalar_character(alias)) {
    stop("System fonts must be scalar character vector", call. = FALSE)
  }

  matched <- match_family(alias)
  if (alias != matched) {
    warning(
      call. = FALSE,
      "System font `", alias, "` not found. ",
      "Closest match: `", matched, "`"
    )
  }
  matched
}

is_user_alias <- function(x) {
  is.list(x) &&
    (is_scalar_character(x$file) || is_scalar_character(x$ttf)) &&
    (is_scalar_character(x$alias) || is_scalar_character(x$name))
}

validate_user_alias <- function(default_name, family) {
  if (!all(names(family) %in% r_font_faces)) {
    stop("Faces must contain only: `plain`, `bold`, `italic`, `bolditalic`, `symbol`",
      call. = FALSE
    )
  }

  is_alias_object <- vapply_lgl(family, is_user_alias)
  is_alias_plain <- vapply_lgl(family, is_scalar_character)

  is_valid_alias <- is_alias_object | is_alias_plain
  if (any(!is_valid_alias)) {
    stop(
      call. = FALSE,
      "The following faces are invalid for `", default_name, "`: ",
      paste0(names(family)[!is_valid_alias], collapse = ", ")
    )
  }

  names <- ifelse(is_alias_plain, default_name, family)
  names <- lapply_if(names, is_alias_object, function(obj) {
    obj$alias %||% obj$name
  })
  files <- lapply_if(family, is_alias_object, function(obj) {
    obj$file %||% obj$ttf
  })

  file_exists <- vapply_lgl(files, file.exists)
  if (any(!file_exists)) {
    missing <- unlist(files)[!file_exists]
    stop(
      call. = FALSE,
      "Could not find font file: ",
      paste0(missing, collapse = ", ")
    )
  }

  zip(list(name = names, file = files))
}
#' Create a font-face specification
#'
#' Webfonts in SVG and HTML can either be specified manually using the
#' `@font-face` at-rule, or imported from e.g. Google Fonts using the `@import`
#' at-rule. `font_face()` helps you create a valid `@font-face` block for the
#' `web_fonts` argument in [svglite()] and [svgstring()] functions.
#'
#' @param family The font family name this font should respond to.
#' @param woff2,woff,ttf,otf,eot,svg URLs to the font in different formats. At
#'   least one must be given. Best browser support is provided by the woff
#'   format.
#' @param local One or more font names that local installations of the font may
#'   have. If a local font is found with either of the given names it will be
#'   used and no download will happen.
#' @param weight An optional value for the `font-weight` descriptor
#' @param style An optional value for the `font-style` descriptor
#' @param range An optional value for the `unicode-range` descriptor Will give
#'   the range of unicode values that this font will support
#' @param variant An optional value for the `font-variant` descriptor
#' @param stretch An optional value for the `font-stretch` descriptor
#' @param feature_setting An optional value for the `font-feature-settings`
#'   descriptor It is recommended to avoid using this if possible
#' @param variation_setting An optional value for the `font-variation-settings`
#'   descriptor.
#'
#' @return A character string with the `@font-face` block.
#'
#' @export
#' @examples
#' font_face(
#'   family = "MyHelvetica",
#'   ttf = "MgOpenModernaBold.ttf",
#'   local = c("Helvetica Neue Bold", "HelveticaNeue-Bold"),
#'   weight = "bold"
#' )
#'
font_face <- function(family, woff2 = NULL, woff = NULL, ttf = NULL, otf = NULL,
                      eot = NULL, svg = NULL, local = NULL, weight = NULL,
                      style = NULL, range = NULL, variant = NULL, stretch = NULL,
                      feature_setting = NULL, variation_setting = NULL) {
  sources <- c(
    if (!is.null(local)) paste0('local("', local, '")'),
    if (!is.null(woff2)) paste0('url("', woff2, '") format("woff2")'),
    if (!is.null(woff)) paste0('url("', woff, '") format("woff")'),
    if (!is.null(otf)) paste0('url("', otf, '") format("opentype")'),
    if (!is.null(ttf)) paste0('url("', ttf, '") format("truetype")'),
    if (!is.null(eot)) paste0('url("', eot, '") format("embedded-opentype")'),
    if (!is.null(svg)) paste0('url("', svg, '") format("woff")')
  )
  if (length(sources) == 0) {
    stop("At least one font source must be given")
  }

  x <- c(
    '    @font-face {\n',
    '      font-family: "', family, '";\n',
    '      src: ', paste0(paste(sources, collapse = ",\n           "), ';\n'),
    if (!is.null(range)) paste0(
    '      unicode-range: ', range[1], ';\n'),
    if (!is.null(variant)) paste0(
    '      font-variant: ', variant[1], ';\n'),
    if (!is.null(feature_setting)) paste0(
    '      font-feature-settings: ', feature_setting[1], ';\n'),
    if (!is.null(variation_setting)) paste0(
    '      font-variation-settings: ', variation_setting[1], ';\n'),
    if (!is.null(stretch)) paste0(
    '      font-stretch: ', stretch[1], ';\n'),
    if (!is.null(weight)) paste0(
    '      font-weight: ', weight[1], ';\n'),
    if (!is.null(style)) paste0(
    '      font-style: ', style[1], ';\n'),
    '    }'
  )
  x <- paste(x, collapse = "")
  class(x) <- c("font_face", "character")
  x
}
#' @export
print.font_face <- function(x, ...) {
  cat(x)
  invisible(x)
}
is_font_face <- function(x) inherits(x, "font_face")

validate_web_fonts <- function(x) {
  if (length(x) == 0) {
    return("")
  }
  paste0(paste(
    ifelse(vapply(x, is_font_face, logical(1)), x, paste0('    @import url("', x, '");')),
    collapse = "\n"
  ), "\n")
}
