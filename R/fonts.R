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
    cli::cli_abort(c(
      "Cannot provided multiple fonts with the same alias",
      i = "Problematic aliases: {unique(aliases[duplicated(aliases)])}"
    ))
  }

  # Add missing system fonts for base families
  missing_aliases <- setdiff(r_font_families, aliases)
  system_fonts[missing_aliases] <- lapply(
    alias_lookup()[missing_aliases],
    match_family
  )

  list(
    system = system_fonts,
    user = user_fonts
  )
}

validate_system_alias <- function(alias) {
  check_string(alias, allow_empty = FALSE)

  matched <- match_family(alias)
  if (alias != matched) {
    cli::cli_warn(
      "System font {.val {alias}} not found.  Closest match is {.val {matched}}"
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
    cli::cli_abort(
      "{.arg family} must can only include elements named {r_font_faces}"
    )
  }

  is_alias_object <- vapply(family, is_user_alias, logical(1))
  is_alias_plain <- vapply(family, is_scalar_character, logical(1))

  is_valid_alias <- is_alias_object | is_alias_plain
  if (any(!is_valid_alias)) {
    cli::cli_abort(
      "The following faces are invalid for {.val {default_name}}: {.val {names(family)[!is_valid_alias]}}"
    )
  }

  names <- ifelse(is_alias_plain, default_name, family)
  names <- lapply_if(names, is_alias_object, function(obj) {
    obj$alias %||% obj$name
  })
  files <- lapply_if(family, is_alias_object, function(obj) {
    obj$file %||% obj$ttf
  })

  file_exists <- vapply(files, file.exists, logical(1))
  if (any(!file_exists)) {
    missing <- unlist(files)[!file_exists]
    cli::cli_abort("Could not find the following font file{?s}: {missing}")
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
#' @param woff2,woff,ttf,otf URLs to the font in different formats. At
#'   least one must be given. Best browser support is provided by the woff
#'   format.
#' @param eot,svg `r lifecycle::badge("deprecated")`
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
#' @param embed Should the font data be embedded directly in the SVG
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
font_face <- function(
  family,
  woff2 = NULL,
  woff = NULL,
  ttf = NULL,
  otf = NULL,
  eot = deprecated(),
  svg = deprecated(),
  local = NULL,
  weight = NULL,
  style = NULL,
  range = NULL,
  variant = NULL,
  stretch = NULL,
  feature_setting = NULL,
  variation_setting = NULL,
  embed = FALSE
) {
  if (lifecycle::is_present(eot)) {
    lifecycle::deprecate_stop("2.2.0", "font_face(eot)")
  }
  if (lifecycle::is_present(svg)) {
    lifecycle::deprecate_stop("2.2.0", "font_face(svg)")
  }

  sources <- list(
    local = local,
    woff2 = woff2,
    woff = woff,
    otf = otf,
    ttf = ttf
  )

  sources <- lapply(seq_along(sources), function(i) {
    location <- sources[[i]]
    type <- names(sources)[i]
    if (embed) {
      if (type == "local") {
        location <- systemfonts::font_info(location)$path[1]
        type <- tolower(tools::file_ext(location))
        if (!type %in% c("woff2", "woff", "otf", "ttf")) {
          stop(paste0("Unsupported file type for embedding: ", type))
        }
      }
      mime <- switch(
        type,
        woff2 = "font/woff2",
        woff = "font/woff",
        otf = "font/otf",
        ttf = "font/ttf"
      )
      location <- paste0(
        "data:",
        mime,
        ";charset=utf-8;base64,",
        base64enc::base64encode(location)
      )
    } else {
      location <- paste0('"', location, '"')
    }
    format <- switch(
      type,
      local = '',
      woff2 = ' format("woff2")',
      woff = ' format("woff")',
      otf = ' format("opentype")',
      ttf = ' format("truetype")'
    )
    prefix <- if (type == "local") "local" else "url"
    paste0(prefix, "(", location, ")", format)
  })
  sources <- unlist(sources)
  if (length(sources) == 0) {
    cli::cli_abort("At least one font source must be given")
  }

  # fmt: skip
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
  x <- lapply(x, function(f) {
    if (is_font_face(f)) {
      return(f)
    }
    if (grepl("^\\s*@import", f)) {
      return(sub("^\\s*@import", "    @import", f))
    }
    if (grepl("^https?://", f) || grepl("^data:", f)) {
      return(paste0('    @import url("', f, '");'))
    }
    if (grepl("^<link", f)) {
      cli::cli_warn(c(
        "Web fonts should not be specified as link tags.",
        i = "Use either `@import` or a bare URL"
      ))
      return()
    }
    x <- fonts_as_import(x, type = "import")
    if (length(x) == 0) return()
    paste0("    ", x)
  })
  paste0(unlist(x), collapse = "\n")
}

#' Add web font imports to an already created SVG file
#'
#' This function allows you to add web fonts after creation. The result is the
#' same as using the `web_fonts` argument in [svglite()]. Only SVGs created with
#' svglite can get web fonts added.
#'
#' @param filename The svgfile(s) or `svg` object(s) (as created by
#' [svgstring()]) to edit
#' @inheritParams svglite
#'
#' @return Invisibly returns `filename`. If any of elements of this were inline
#' SVGs then these have been modified to include the imports
#'
#' @export
#'
add_web_fonts <- function(filename, web_fonts) {
  web_fonts <- validate_web_fonts(web_fonts)
  for (i in seq_along(filename)) {
    f <- filename[[i]]
    is_string <- grepl("^<(\\?xml|svg)", f)
    if (is_string) {
      svg <- strsplit(f, "\n")[[1]]
    } else {
      if (!grepl("\\.svg(z|\\.gz)?$", f)) {
        cli::cli_abort("{.file {f}} is not an SVG file")
      }
      svg <- readLines(f)
    }
    if (!any(grepl("<g.*class='svglite'>", svg))) {
      if (is_string) {
        cli::cli_warn(
          "SVG was not created by svglite. Not inserting font import"
        )
      } else {
        cli::cli_warn(
          "{.file {f}} was not created by svglite. Not inserting font import"
        )
      }
      next
    }
    style <- grep("<style type='text/css'><![CDATA[", svg, fixed = TRUE)
    if (length(style) == 0) {
      if (is_string) {
        cli::cli_warn("Can't find style tag in SVG. Not inserting font import")
      } else {
        cli::cli_warn(
          "Can't find style tag in {.file {f}}. Not inserting font import"
        )
      }
      next
    }
    svg <- c(svg[seq_len(style[1])], web_fonts, svg[-seq_len(style[1])])
    if (is_string) {
      filename[[i]][] <- paste0(svg, collapse = "\n")
    } else {
      out <- if (grepl("\\.(gz|svgz)$", f)) gzfile(f, "w") else file(f, "w")
      writeLines(svg, out)
      close(out)
    }
  }
  invisible(filename)
}
