# throw on malformed alias

    Code
      validate_aliases(list(mono = letters), list())
    Condition
      Error in `FUN()`:
      ! `alias` must be a single string, not a character vector.

---

    Code
      validate_aliases(list(sans = "foobar"), list())
    Condition
      Warning:
      System font "foobar" not found.  Closest match is "Helvetica"
    Output
      $system
      $system$sans
      [1] "Helvetica"
      
      $system$serif
      [1] "Times"
      
      $system$mono
      [1] "Courier"
      
      $system$symbol
      [1] "Symbol"
      
      
      $user
      list()
      

