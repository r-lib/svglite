
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
  system_fonts[missing_aliases] <- lapply(alias_lookup()[missing_aliases], gdtools::match_family)

  list(
    system = system_fonts,
    user = user_fonts
  )
}

validate_system_alias <- function(alias) {
  if (!is_scalar_character(alias)) {
    stop("System fonts must be scalar character vector", call. = FALSE)
  }

  matched <- gdtools::match_family(alias)
  if (alias != matched) {
    warning(call. = FALSE,
      "System font `", alias, "` not found. ",
      "Closest match: `", matched, "`")
  }
  matched
}

is_font_file <- function(x) {
  is.list(x) &&
    (is.character(x$file) || is.character(x$ttf)) &&
    is.character(x$name)
}

validate_user_alias <- function(default_name, family) {
  if (!all(names(family) %in% r_font_faces)) {
    stop("Faces must contain only: `plain`, `bold`, `italic`, `bolditalic`, `symbol`",
      call. = FALSE)
  }

  is_file_object <- vapply_lgl(family, is_font_file)
  is_file_plain <- vapply_lgl(family, is_scalar_character)

  is_valid_alias <- is_file_object | is_file_plain
  if (any(!is_valid_alias)) {
    stop(call. = FALSE,
      "The following faces are invalid for `", default_name, "`: ",
      paste0(names(family)[!is_valid_alias], collapse = ", ")
    )
  }

  names <- ifelse(is_file_plain, default_name, family)
  names <- lapply_if(names, is_file_object, `[[`, "name")
  files <- lapply_if(family, is_file_object, function(obj) {
    obj$file %||% obj$ttf
  })

  file_exists <- vapply_lgl(files, file.exists)
  if (any(!file_exists)) {
    missing <- unlist(files)[!file_exists]
    stop(call. = FALSE,
      "Could not find font file: ",
      paste0(missing, collapse = ", ")
    )
  }

  zip(list(name = names, file = files))
}
