#' Check font aliases for missing families and faces
#' 
#' @param aliases A list of families of faces. Typically created with
#'   \code{fontquiver::font_families()}.
#' @export
check_aliases <- function(aliases) {
  all_missing <- 0 == vapply(aliases, function(alias) length(compact(alias)), integer(1))
  missing_families <- names(all_missing)[all_missing]

  # Include families not in the list
  missing_families <- c(missing_families, setdiff(r_font_families, names(aliases)))
  message("Missing families:\n\n  ", paste(missing_families, collpase = "\n  "), "\n")

  missing_faces <- lapply(aliases[!all_missing], keep, is.null)
  missing_faces <- unlist(missing_faces, recursive = FALSE)
  missing_faces <- names(missing_faces)
  message("Missing faces:\n\n  ", paste(missing_faces, collapse = "\n  "), "\n")

  invisible(list(families = missing_families, faces = missing_faces))
}

r_font_families <- c("sans", "serif", "mono", "symbol")

font_spec <- function(aliases, extra) {
  extra <- extra %||% list()
  list(
    aliases = validate_aliases(aliases),
    extra = fontquiver::splice_fonts(extra)
  )
}

str_prettify <- function(str) {
  if (is.null(str)) return(NULL)
  vapply_chr(str, function(s) {
    s <- strsplit(s, "-|_| ")[[1]]
    paste0(toupper(substring(s, 1, 1)), substring(s, 2), collapse=" ")
  })
}

validate_aliases <- function(aliases) {
  unknown <- setdiff(names(aliases), r_font_families)
  if (length(unknown)) {
    stop(call. = FALSE,
      "Aliases must contain only those elements: ",
      paste(r_font_families, collapse = ", ")
    )
  }

  aliases <- compact(lapply(aliases, compact))
  contains_font_file <- lapply(aliases, vapply_lgl, inherits, "font_file")
  is_valid_user_alias <- vapply_lgl(contains_font_file, all)
  is_valid_system_alias <- vapply_lgl(aliases, is_scalar_character)
  is_valid_alias <- is_valid_user_alias | is_valid_system_alias
  if (!all(is_valid_alias)) {
    stop(call. = FALSE,
      "Some aliases are not valid: ",
      paste(names(aliases)[!is_valid_alias], collapse = ", ")
    )
  }

  missing_aliases <- setdiff(r_font_families, names(aliases))
  aliases[missing_aliases] <- alias_lookup[missing_aliases]

  aliases
}

alias_lookup <- c(
  sans = "Arial",
  serif = "Times New Roman",
  mono = "courier",
  symbol = "symbol"
)
