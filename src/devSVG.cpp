//  (C) 2002 T Jake Luciani: SVG device, based on PicTex device
//  (C) 2008 Tony Plate: Line type support from RSVGTipsDevice package
//  (C) 2012 Matthieu Decorde: UTF-8 support, XML reserved characters and XML header
//  (C) 2015 RStudio (Hadley Wickham): modernisation & refactoring
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

extern "C" {
#include <png.h>
}

#include <cpp11/list.hpp>
#include <cpp11/strings.hpp>
#include <cpp11/as.hpp>
#include <cpp11/environment.hpp>
#include <cpp11/external_pointer.hpp>
#include <cpp11/protect.hpp>
#include <systemfonts.h>
#include <string>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_set>
#include <R_ext/GraphicsEngine.h>

#include "SvgStream.h"
#include "utils.h"
#include "tinyformat.h"

typedef std::shared_ptr<SvgStream> SvgStreamPtr;

// SVG device metadata
class SVGDesc {
public:
  SvgStreamPtr stream;

  int pageno;
  bool is_inited;
  std::string clipid;  // ID for the clip path
  bool is_clipping;
  double clipx0, clipx1, clipy0, clipy1;  // Save the previous clip path to avoid duplication
  bool standalone;
  bool fix_text_size;
  double scaling;
  bool always_valid;
  const std::string file;
  cpp11::list system_aliases;
  cpp11::list user_aliases;
  const std::string webfonts;
  cpp11::strings ids;

  // Caches
  std::unordered_set<unsigned int> clip_cache;
  unsigned int clip_cache_next_id;
  bool is_recording_clip;

  std::unordered_set<unsigned int> mask_cache;
  unsigned int mask_cache_next_id;
  int current_mask;

  std::unordered_set<unsigned int> pattern_cache;
  unsigned int pattern_cache_next_id;

  SVGDesc(SvgStreamPtr stream_, bool standalone_, cpp11::list aliases_,
          const std::string webfonts_, const std::string& file_, cpp11::strings ids_,
          bool fix_text_size_, double scaling_, bool always_valid_):
      stream(stream_),
      pageno(0),
      is_inited(false),
      is_clipping(false),
      clipx0(0), clipx1(0), clipy0(0), clipy1(0),
      standalone(standalone_),
      fix_text_size(fix_text_size_),
      scaling(scaling_),
      always_valid(always_valid_),
      file(file_),
      system_aliases(cpp11::as_cpp<cpp11::list>(aliases_["system"])),
      user_aliases(cpp11::as_cpp<cpp11::list>(aliases_["user"])),
      webfonts(webfonts_),
      ids(ids_),
      clip_cache_next_id(0),
      is_recording_clip(false),
      mask_cache_next_id(0),
      current_mask(-1),
      pattern_cache_next_id(0) {
  }

  void nextFile() {
    stream->finish(false);
    if (stream->is_file_stream()) {
      SvgStreamPtr newStream(new SvgStreamFile(file, pageno + 1, always_valid));
      stream = newStream;
    }
    clipid.clear();
    set_clipping(false);
  }

  void set_clipping(bool clip) {
    stream->set_clipping(clip);
    is_clipping = clip;
  }
};

inline bool is_black(int col) {
  return (R_RED(col) == 0) && (R_GREEN(col) == 0) && (R_BLUE(col) == 0) &&
    (R_ALPHA(col) == 255);
}

inline bool is_bold(int face) {
  return face == 2 || face == 4;
}
inline bool is_italic(int face) {
  return face == 3 || face == 4;
}
inline bool is_bolditalic(int face) {
  return face == 4;
}
inline bool is_symbol(int face) {
  return face == 5;
}

const static char encode_lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const static char pad_character = '=';
inline std::string base64_encode(const std::uint8_t* buffer, size_t size) {
  std::string encoded_string;
  encoded_string.reserve(((size/3) + (size % 3 > 0)) * 4);
  std::uint32_t temp{};
  int index = 0;
  for (size_t idx = 0; idx < size/3; idx++) {
    temp  = buffer[index++] << 16; //Convert to big endian
    temp += buffer[index++] << 8;
    temp += buffer[index++];
    encoded_string.append(1, encode_lookup[(temp & 0x00FC0000) >> 18]);
    encoded_string.append(1, encode_lookup[(temp & 0x0003F000) >> 12]);
    encoded_string.append(1, encode_lookup[(temp & 0x00000FC0) >> 6 ]);
    encoded_string.append(1, encode_lookup[(temp & 0x0000003F)      ]);
  }
  switch (size % 3) {
  case 1:
    temp  = buffer[index++] << 16; //Convert to big endian
    encoded_string.append(1, encode_lookup[(temp & 0x00FC0000) >> 18]);
    encoded_string.append(1, encode_lookup[(temp & 0x0003F000) >> 12]);
    encoded_string.append(2, pad_character);
    break;
  case 2:
    temp  = buffer[index++] << 16; //Convert to big endian
    temp += buffer[index++] << 8;
    encoded_string.append(1, encode_lookup[(temp & 0x00FC0000) >> 18]);
    encoded_string.append(1, encode_lookup[(temp & 0x0003F000) >> 12]);
    encoded_string.append(1, encode_lookup[(temp & 0x00000FC0) >> 6 ]);
    encoded_string.append(1, pad_character);
    break;
  }
  return encoded_string;
}

static void png_memory_write(png_structp  png_ptr, png_bytep data, png_size_t length) {
  std::vector<uint8_t> *p = (std::vector<uint8_t>*)png_get_io_ptr(png_ptr);
  p->insert(p->end(), data, data + length);
}
inline std::string raster_to_string(unsigned int *raster, int w, int h, double width, double height, bool interpolate) {
  h = h < 0 ? -h : h;
  w = w < 0 ? -w : w;
  bool resize = false;
  int w_fac = 1, h_fac = 1;
  std::vector<unsigned int> raster_resize;

  if (!interpolate && double(w) < width) {
    resize = true;
    w_fac = std::ceil(width / w);
  }
  if (!interpolate && double(h) < height) {
    resize = true;
    h_fac = std::ceil(height / h);
  }

  if (resize) {
    int w_new = w * w_fac;
    int h_new = h * h_fac;
    raster_resize.reserve(w_new * h_new);
    for (int i = 0; i < h; ++i) {
      for (int j = 0; j < w; ++j) {
        unsigned int val = raster[i * w + j];
        for (int wrep = 0; wrep < w_fac; ++wrep) {
          raster_resize.push_back(val);
        }
      }
      for (int hrep = 1; hrep < h_fac; ++hrep) {
        raster_resize.insert(raster_resize.end(), raster_resize.end() - w_new, raster_resize.end());
      }
    }
    raster = raster_resize.data();
    w = w_new;
    h = h_new;
  }

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    return "";
  }
  png_infop info = png_create_info_struct(png);
  if (!info) {
    png_destroy_write_struct(&png, (png_infopp)NULL);
    return "";
  }
  if (setjmp(png_jmpbuf(png))) {
    png_destroy_write_struct(&png, &info);
    return "";
  }
  png_set_IHDR(
    png,
    info,
    w, h,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  std::vector<uint8_t*> rows(h);
  for (int y = 0; y < h; ++y) {
    rows[y] = (uint8_t*)raster + y * w * 4;
  }

  std::vector<std::uint8_t> buffer;
  png_set_rows(png, info, &rows[0]);
  png_set_write_fn(png, &buffer, png_memory_write, NULL);
  png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);
  png_destroy_write_struct(&png, &info);

  return base64_encode(buffer.data(), buffer.size());
}

inline std::string find_alias_field(std::string family, cpp11::list& alias,
                                    const char* face, const char* field) {
  if (alias[face] != R_NilValue) {
    cpp11::list font(alias[face]);
    if (font[field] != R_NilValue)
      return cpp11::as_cpp<std::string>(font[field]);
  }
  return std::string();
}

inline std::string find_user_alias(std::string family,
                                   cpp11::list const& aliases,
                                   int face, const char* field) {
  std::string out;
  if (aliases[family.c_str()] != R_NilValue) {
    cpp11::list alias(aliases[family.c_str()]);
    if (is_bolditalic(face))
      out = find_alias_field(family, alias, "bolditalic", field);
    else if (is_bold(face))
      out = find_alias_field(family, alias, "bold", field);
    else if (is_italic(face))
      out = find_alias_field(family, alias, "italic", field);
    else if (is_symbol(face))
      out = find_alias_field(family, alias, "symbol", field);
    else
      out = find_alias_field(family, alias, "plain", field);
  }
  return out;
}

inline std::string find_system_alias(std::string family,
                                     cpp11::list const& aliases) {
  std::string out;
  if (aliases[family.c_str()] != R_NilValue) {
    cpp11::sexp alias = aliases[family.c_str()];
    if (TYPEOF(alias) == STRSXP && Rf_length(alias) == 1)
      out = cpp11::as_cpp<std::string>(alias);
  }
  return out;
}

inline std::string fontname(const char* family_, int face,
                            cpp11::list const& system_aliases,
                            cpp11::list const& user_aliases, FontSettings& font) {
  std::string family(family_);
  if (face == 5)
    family = "symbol";
  else if (family == "")
    family = "sans";

  std::string alias = find_system_alias(family, system_aliases);
  if (!alias.size()) {
    alias = find_user_alias(family, user_aliases, face, "name");
  }

  if (alias.size()) {
    return alias;
  }

  std::string family_name = "";
  family_name.resize(100);
  if (get_font_family(font.file, font.index, &family_name[0], 100)) {
    family_name.erase(family_name.find('\0'));
    return family_name;
  }
  return family;
}

inline std::string fontfile(const char* family_, int face,
                            cpp11::list user_aliases) {
  std::string family(family_);
  if (face == 5)
    family = "symbol";
  else if (family == "")
    family = "sans";

  return find_user_alias(family, user_aliases, face, "file");
}

inline FontSettings get_font_file(const char* family, int face, cpp11::list user_aliases) {
  const char* fontfamily = family;
  if (is_symbol(face)) {
    fontfamily = "symbol";
  } else if (strcmp(family, "") == 0) {
    fontfamily = "sans";
  }
  std::string alias = fontfile(fontfamily, face, user_aliases);
  if (alias.size() > 0) {
    FontSettings result = {};
    std::strncpy(result.file, alias.c_str(), PATH_MAX);
    result.index = 0;
    result.n_features = 0;
    return result;
  }

  return locate_font_with_features(fontfamily, is_italic(face), is_bold(face));
}

inline void write_escaped(SvgStreamPtr stream, const char* text) {
  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': (*stream) << "&amp;"; break;
    case '<': (*stream) << "&lt;";  break;
    case '>': (*stream) << "&gt;";  break;
    default:  (*stream) << *cur;
    }
  }
}

inline void write_attr_dbl(SvgStreamPtr stream, const char* attr, double value) {
  (*stream) << ' ' << attr << "='" << value << '\'';
}

inline void write_attr_str(SvgStreamPtr stream, const char* attr, const char* value) {
  (*stream) << ' ' << attr << "='" << value << '\'';
}

// Writing clip path attribute
inline void write_attr_clip(SvgStreamPtr stream, std::string clipid) {
  if (!clipid.size())
    return;

  (*stream) << " clip-path='url(#cp" << clipid << ")'";
}

// Writing mask attribute
inline void write_attr_mask(SvgStreamPtr stream, int mask) {
  if (mask < 0)
    return;

  (*stream) << " mask='url(#mask-" << mask << ")'";
}

// Beginning of writing style attributes
inline void write_style_begin(SvgStreamPtr stream) {
  (*stream) << " style='";
}

// End of writing style attributes
inline void write_style_end(SvgStreamPtr stream) {
  (*stream) << "'";
}

// Writing style attributes related to colors
inline void write_style_col(SvgStreamPtr stream, const char* attr, int col) {
  int alpha = R_ALPHA(col);
  if (alpha == 0) {
    (*stream) << attr << ": none;";
    return;
  } else {
    (*stream) << tfm::format("%s: #%02X%02X%02X;", attr, R_RED(col), R_GREEN(col), R_BLUE(col));
    if (alpha != 255)
      (*stream) << ' ' << attr << "-opacity: " << alpha / 255.0 << ';';
  }
}

// Writing style attributes related to stroke
inline void write_style_stroke(SvgStreamPtr stream, int col, bool first = false) {
  // Default is "stroke: #000000;" as declared in <style>
  if (is_black(col)) {
    return;
  }

  if(!first)  (*stream) << ' ';
  write_style_col(stream, "stroke", col);
}

// Writing style attributes related to colors
inline void write_style_fill(SvgStreamPtr stream, const pGEcontext gc, bool first = false) {
  int pattern = -1;
#if R_GE_version >= 13
  pattern = Rf_isNull(gc->patternFill) ? -1 : INTEGER(gc->patternFill)[0];
#endif

  if (pattern != -1) {
    if(!first)  (*stream) << ' ';
    (*stream) << "fill: url(#pat-" << pattern << ");";
    return;
  }

  if (R_ALPHA(gc->fill) != 0) {
    if(!first)  (*stream) << ' ';
    write_style_col(stream, "fill", gc->fill);
  }
}

// Writing style attributes whose values are double type
inline void write_style_dbl(SvgStreamPtr stream, const char* attr, double value, bool first = false) {
  if(!first)  (*stream) << ' ';
  (*stream) << attr << ": " << value << ';';
}

// Writing style attributes whose values are int type
inline void write_style_int(SvgStreamPtr stream, const char* attr, int value, bool first = false) {
  if(!first)  (*stream) << ' ';
  (*stream) << attr << ": " << value << ';';
}

inline void write_style_fontsize(SvgStreamPtr stream, double value, bool first = false) {
  if(!first) (*stream) << ' ';
  // Firefox requires that we provide a unit (even though px is
  // redundant here)
  (*stream) << "font-size: " << value << "px;";
}

// Writing style attributes whose values are strings
inline void write_style_str(SvgStreamPtr stream, const char* attr, const char* value, bool first = false) {
  if(!first)  (*stream) << ' ';
  (*stream) << attr << ": " << value << ';';
}

inline double scale_lty(int lty, double lwd) {
  // Don't rescale if lwd < 1
  // https://github.com/wch/r-source/blob/master/src/library/grDevices/src/cairo/cairoFns.c#L134
  return ((lwd > 1) ? lwd : 1) * (lty & 15);
}

// Writing style attributes related to line types
inline void write_style_linetype(SvgStreamPtr stream, const pGEcontext gc, double scaling, bool first = false) {
  int lty = gc->lty;
  double lwd = gc->lwd * scaling;

  // 1 lwd = 1/96", but units in rest of document are 1/72"
  write_style_dbl(stream, "stroke-width", lwd / 96.0 * 72, first);

  // Default is "stroke: #000000;" as declared in <style>
  write_style_stroke(stream, gc->col);

  // Set line pattern type
  switch (lty) {
  case LTY_BLANK: // never called: blank lines never get to this point
  case LTY_SOLID: // default svg setting, so don't need to write out
    break;
  default:
    // For details
    // https://github.com/wch/r-source/blob/trunk/src/include/R_ext/GraphicsEngine.h#L337
    (*stream) << " stroke-dasharray: ";
    // First number
    (*stream) << scale_lty(lty, lwd);
    lty = lty >> 4;
    // Remaining numbers
    for(int i = 1 ; i < 8 && lty & 15; i++) {
      (*stream) << ',' << scale_lty(lty, lwd);
      lty = lty >> 4;
    }
    stream->put(';');
    break;
  }

  // Set line end shape
  switch(gc->lend)
  {
  case GE_ROUND_CAP: // declared to be default in <style>
    break;
  case GE_BUTT_CAP:
    write_style_str(stream, "stroke-linecap", "butt");
    break;
  case GE_SQUARE_CAP:
    write_style_str(stream, "stroke-linecap", "square");
    break;
  default:
    break;
  }

  // Set line join shape
  switch(gc->ljoin)
  {
  case GE_ROUND_JOIN: // declared to be default in <style>
    break;
  case GE_BEVEL_JOIN:
    write_style_str(stream, "stroke-linejoin", "bevel");
    break;
  case GE_MITRE_JOIN:
    write_style_str(stream, "stroke-linejoin", "miter");
    if (std::abs(gc->lmitre - 10.0) > 1e-3) // 10 is declared to be the default in <style>
      write_style_dbl(stream, "stroke-miterlimit", gc->lmitre);
    break;
  default:
    break;
  }
}

std::string get_id(SVGDesc *svgd) {
  if (svgd->ids.size() == 0) {
    return "";
  }
  if (svgd->ids.size() == 1) {
    return std::string(svgd->ids[0]);
  }
  if (svgd->pageno >= svgd->ids.size()) {
    Rf_warning("No id supplied for page no %i", svgd->pageno + 1);
    return "";
  }
  return std::string(svgd->ids[svgd->pageno]);
}

// Callback functions for graphics device --------------------------------------

void svg_metric_info(int c, const pGEcontext gc, double* ascent,
                     double* descent, double* width, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  if (c < 0) {
    c = -c;
  }

  FontSettings font = get_font_file(gc->fontfamily, gc->fontface, svgd->user_aliases);

  int error = glyph_metrics(c, font.file, font.index, gc->ps * gc->cex * svgd->scaling, 1e4, ascent, descent, width);
  if (error != 0) {
    *ascent = 0;
    *descent = 0;
    *width = 0;
  }
  double mod = 72./1e4;
  *ascent *= mod;
  *descent *= mod;
  *width *= mod;
}

void svg_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (!svgd->is_inited) {
    return;
  }
  SvgStreamPtr stream = svgd->stream;

  double xmin = std::min(x0, x1);
  double xmax = std::max(x0, x1);
  double ymin = std::min(y0, y1);
  double ymax = std::max(y0, y1);

  // Avoid duplication
  if (std::abs(xmin - svgd->clipx0) < 0.01 &&
      std::abs(xmax - svgd->clipx1) < 0.01 &&
      std::abs(ymin - svgd->clipy0) < 0.01 &&
      std::abs(ymax - svgd->clipy1) < 0.01)
    return;

  std::ostringstream s;
  s << std::fixed << std::setprecision(2);
  s << dbl_format(xmin) << "|" << dbl_format(xmax) << "|" <<
    dbl_format(ymin) << "|" << dbl_format(ymax);
  std::string str = s.str();
  const std::uint8_t* buffer = reinterpret_cast<const std::uint8_t*>(str.data());
  std::string clipid = base64_encode(buffer, str.size());

  svgd->clipid = clipid;
  svgd->clipx0 = xmin;
  svgd->clipx1 = xmax;
  svgd->clipy0 = ymin;
  svgd->clipy1 = ymax;

  if (svgd->is_clipping) {
    (*stream) << "</g>\n";
  }
  // Is this clip region already defined?
  if (!stream->has_clip_id(clipid)) {
    stream->add_clip_id(clipid);

    (*stream) << "<defs>\n";
    (*stream) << "  <clipPath id='cp" << svgd->clipid << "'>\n";
    (*stream) << "    <rect x='" << xmin << "' y='" << ymin <<
      "' width='" << (xmax - xmin) << "' height='" << (ymax - ymin) << "' />\n";
    (*stream) << "  </clipPath>\n";
    (*stream) << "</defs>\n";
  }

  (*stream) << "<g";
  write_attr_clip(stream, svgd->clipid);
  (*stream) << ">\n";
  svgd->set_clipping(true);
  stream->flush();
}

void svg_new_page(const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  SvgStreamPtr stream = svgd->stream;

  std::string id = get_id(svgd);
  svgd->clip_cache_next_id = 0;
  svgd->mask_cache_next_id = 0;
  svgd->pattern_cache_next_id = 0;

  if (svgd->pageno > 0) {
    // close existing file, create a new one, and update stream
    svgd->nextFile();
    stream = svgd->stream;
  }

  if (svgd->standalone)
    (*stream) << "<?xml version='1.0' encoding='UTF-8' ?>\n";

  (*stream) << "<svg";
  if (svgd->standalone){
    (*stream) << " xmlns='http://www.w3.org/2000/svg'";
    //http://www.w3.org/wiki/SVG_Links
    (*stream) << " xmlns:xlink='http://www.w3.org/1999/xlink'";
  }

  if (id.size() > 0)
    (*stream) << " id='" << id << "'";

  (*stream) << " class='svglite'";

  (*stream) << " width='" << dd->right << "pt' height='" << dd->bottom << "pt'";

  (*stream) << " viewBox='0 0 " << dd->right << ' ' << dd->bottom << "'>\n";

  // Setting default styles
  (*stream) << "<defs>\n";
  (*stream) << "  <style type='text/css'><![CDATA[\n";
  (*stream) <<      svgd->webfonts;
  (*stream) << "    .svglite line, .svglite polyline, .svglite polygon, .svglite path, .svglite rect, .svglite circle {\n";
  (*stream) << "      fill: none;\n";
  (*stream) << "      stroke: #000000;\n";
  (*stream) << "      stroke-linecap: round;\n";
  (*stream) << "      stroke-linejoin: round;\n";
  (*stream) << "      stroke-miterlimit: 10.00;\n";
  (*stream) << "    }\n";
  (*stream) << "    .svglite text {\n";
  (*stream) << "      white-space: pre;\n";
  (*stream) << "    }\n";
  (*stream) << "  ]]></style>\n";
  (*stream) << "</defs>\n";

  (*stream) << "<rect width='100%' height='100%'";
  write_style_begin(stream);
  write_style_str(stream, "stroke", "none", true);
  (*stream) << ' ';
  if (R_ALPHA(gc->fill) != 0) {
    write_style_col(stream, "fill", gc->fill);
  } else {
    write_style_col(stream, "fill", dd->startfill);
  }
  write_style_end(stream);
  (*stream) << "/>\n";


  // Initialise clipping - make sure the stored clipping is bogus to force
  // svg_clip to do its thing
  svgd->clipx0 = R_PosInf;
  svgd->clipy0 = R_NegInf;
  svgd->clipx1 = R_NegInf;
  svgd->clipy1 = R_PosInf;
  svgd->is_inited = true;

  svg_clip(0, dd->right, dd->bottom, 0, dd);

  svgd->stream->flush();
  svgd->pageno++;
}

void svg_close(pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (svgd->is_inited) {
    svgd->stream->finish(true);
  }
  delete(svgd);
}

void svg_line(double x1, double y1, double x2, double y2,
              const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (!svgd->is_inited || svgd->is_recording_clip) {
    return;
  }
  SvgStreamPtr stream = svgd->stream;

  (*stream) << "<line x1='" << x1 << "' y1='" << y1 << "' x2='" <<
    x2 << "' y2='" << y2 << '\'';

  write_attr_mask(stream, svgd->current_mask);
  write_style_begin(stream);
  write_style_linetype(stream, gc, svgd->scaling, true);
  write_style_end(stream);

  (*stream) << " />\n";
  stream->flush();
}

void svg_poly(int n, double *x, double *y, int filled, const pGEcontext gc,
              pDevDesc dd, const char* node_name) {

  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (n == 0 || !svgd->is_inited || (!filled && svgd->is_recording_clip)) {
    return;
  }
  SvgStreamPtr stream = svgd->stream;

  if (svgd->is_recording_clip) {
    (*stream) << "M " << x[0] << ',' << y[0] << ' ';
    for (int i = 1; i < n; i++) {
      (*stream) << "L " << x[i] << ',' << y[i] << ' ';
    }
    stream->put('Z');

    return;
  }

  (*stream) << "<" << node_name << " points='";

  for (int i = 0; i < n; i++) {
    (*stream) << x[i] << ',' << y[i] << ' ';
  }
  stream->put('\'');

  write_attr_mask(stream, svgd->current_mask);
  write_style_begin(stream);
  write_style_linetype(stream, gc, svgd->scaling, true);
  if (filled) {
    write_style_fill(stream, gc);
  }
  write_style_end(stream);

  (*stream) << " />\n";
  stream->flush();

}

void svg_polyline(int n, double *x, double *y, const pGEcontext gc,
                  pDevDesc dd) {
  svg_poly(n, x, y, 0, gc, dd, "polyline");
}
void svg_polygon(int n, double *x, double *y, const pGEcontext gc,
                 pDevDesc dd) {
  svg_poly(n, x, y, 1, gc, dd, "polygon");
}

void svg_path(double *x, double *y,
              int npoly, int *nper,
              Rboolean winding,
              const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (!svgd->is_inited) {
    return;
  }
  SvgStreamPtr stream = svgd->stream;

  // Create path data
  if (!svgd->is_recording_clip) {
    (*stream) << "<path d='";
  }

  int ind = 0;
  for (int i = 0; i < npoly; i++) {
    // Move to the first point of the sub-path
    (*stream) << "M " << x[ind] << ' ' << y[ind] << ' ';
    ind++;
    // Draw the sub-path
    for (int j = 1; j < nper[i]; j++) {
      (*stream) << "L " << x[ind] << ' ' << y[ind] << ' ';
      ind++;
    }
    // Close the sub-path
    stream->put('Z');
  }
  if (svgd->is_recording_clip) {
    return;
  }
  // Finish path data
  stream->put('\'');

  write_attr_mask(stream, svgd->current_mask);
  write_style_begin(stream);
  // Specify fill rule
  write_style_str(stream, "fill-rule", winding ? "nonzero" : "evenodd", true);
  write_style_fill(stream, gc);
  write_style_linetype(stream, gc, svgd->scaling);
  write_style_end(stream);

  (*stream) << " />\n";
  stream->flush();
}

double svg_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  FontSettings font = get_font_file(gc->fontfamily, gc->fontface, svgd->user_aliases);

  double width = 0.0;

  int error = string_width(str, font.file, font.index, gc->ps * gc->cex * svgd->scaling, 1e4, 1, &width);

  if (error != 0) {
    width = 0.0;
  }

  return width * 72. / 1e4;
}

void svg_rect(double x0, double y0, double x1, double y1,
              const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (!svgd->is_inited) {
    return;
  }
  SvgStreamPtr stream = svgd->stream;

  if (svgd->is_recording_clip) {
    (*stream) << "M " << x0 << ',' << y0 << " L " << x0 << ',' << y1 <<
      " L " << x1 << ',' << y1 << " L " << x1 << ',' << y0;
    stream->put('Z');
    return;
  }

  // x and y give top-left position
  (*stream) << "<rect x='" << fmin(x0, x1) << "' y='" << fmin(y0, y1) <<
    "' width='" << fabs(x1 - x0) << "' height='" << fabs(y1 - y0) << '\'';

  write_attr_mask(stream, svgd->current_mask);
  write_style_begin(stream);
  write_style_linetype(stream, gc, svgd->scaling, true);
  write_style_fill(stream, gc);
  write_style_end(stream);

  (*stream) << " />\n";
  stream->flush();
}

void svg_circle(double x, double y, double r, const pGEcontext gc,
                       pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (!svgd->is_inited) {
    return;
  }
  SvgStreamPtr stream = svgd->stream;

  if (svgd->is_recording_clip) {
    (*stream) << "M " << x-r << ',' << y <<
      " a " << r << ',' << r << " 0 1,1 " << r*2 << ",0 " <<
      " a " << r << ',' << r << " 0 1,1 " << -r*2 << ",0 ";
    stream->put('Z');
    return;
  }

  (*stream) << "<circle cx='" << x << "' cy='" << y << "' r='" << r << "'";

  write_attr_mask(stream, svgd->current_mask);
  write_style_begin(stream);
  write_style_linetype(stream, gc, svgd->scaling, true);
  write_style_fill(stream, gc);
  write_style_end(stream);

  (*stream) << " />\n";
  stream->flush();
}

void svg_text(double x, double y, const char *str, double rot,
              double hadj, const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (!svgd->is_inited || svgd->is_recording_clip) {
    return;
  }
  SvgStreamPtr stream = svgd->stream;

  (*stream) << "<text";

  if (rot == 0) {
    write_attr_dbl(stream, "x", x);
    write_attr_dbl(stream, "y", y);
  } else {
    (*stream) << tfm::format(" transform='translate(%0.2f,%0.2f) rotate(%0.0f)'",
      x, y, -1.0 * rot);
  }

  double fontsize = gc->cex * gc->ps;

  if (hadj == 0.5) {
    write_attr_str(stream, "text-anchor", "middle");
  } else if (hadj == 1) {
    write_attr_str(stream, "text-anchor", "end");
  }

  write_attr_mask(stream, svgd->current_mask);
  write_style_begin(stream);
  write_style_fontsize(stream, fontsize * svgd->scaling, true);

  FontSettings font_info = get_font_file(gc->fontfamily, gc->fontface, svgd->user_aliases);
  int weight = get_font_weight(font_info.file, font_info.index);

  if (weight != 400) {
    if (weight == 700) {
      write_style_str(stream, "font-weight", "bold");
    } else {
      write_style_int(stream, "font-weight", weight);
    }
  }
  if (is_italic(gc->fontface))
    write_style_str(stream, "font-style", "italic");
  if (!is_black(gc->col))
    write_style_col(stream, "fill", gc->col);

  std::string font = fontname(gc->fontfamily, gc->fontface, svgd->system_aliases, svgd->user_aliases, font_info);
  font = "\"" + font + "\"";
  write_style_str(stream, "font-family", font.c_str());

  if (font_info.n_features > 0) {
    (*stream) << " font-feature-settings: ";
    for (int i = 0; i < font_info.n_features; ++i) {
      std::string feature = "";
      feature += font_info.features[i].feature[0];
      feature += font_info.features[i].feature[1];
      feature += font_info.features[i].feature[2];
      feature += font_info.features[i].feature[3];
      (*stream) << "\"" << feature << "\" " << font_info.features[i].setting;
      (*stream) << (i == font_info.n_features - 1 ? ";" : ",");
    }
  }

  write_style_end(stream);

  if (svgd->fix_text_size) {
    double width = svg_strwidth(str, gc, dd);
    (*stream) << " textLength='" << width << "px'";
    (*stream) << " lengthAdjust='spacingAndGlyphs'";
  }
  stream->put('>');

  write_escaped(stream, str);

  (*stream) << "</text>";

  stream->put('\n');
  stream->flush();
}

void svg_size(double *left, double *right, double *bottom, double *top,
              pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

void svg_raster(unsigned int *raster, int w, int h,
                double x, double y,
                double width, double height,
                double rot,
                Rboolean interpolate,
                const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (!svgd->is_inited || svgd->is_recording_clip) {
    return;
  }
  SvgStreamPtr stream = svgd->stream;

  if (height < 0)
    height = -height;

  std::string base64_str = raster_to_string(raster, w, h, width, height, interpolate);

  (*stream) << "<image";
  write_attr_dbl(stream, "width", width);
  write_attr_dbl(stream, "height", height);
  write_attr_dbl(stream, "x", x);
  write_attr_dbl(stream, "y", y - height);
  write_attr_str(stream, "preserveAspectRatio", "none");
  write_attr_mask(stream, svgd->current_mask);
  if (!interpolate) {
    write_attr_str(stream, "image-rendering", "pixelated");
  }

  if( rot != 0 ){
    (*stream) << tfm::format(" transform='rotate(%0.0f,%.2f,%.2f)'", -1.0 * rot, x, y);
  }

  (*stream) << " xlink:href='data:image/png;base64," << base64_str << '\'';
  (*stream) << "/>";

  stream->put('\n');
  stream->flush();
}

SEXP svg_set_pattern(SEXP pattern, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (Rf_isNull(pattern)) {
    return Rf_ScalarInteger(-1);
  }
  int key = svgd->pattern_cache_next_id;
  svgd->pattern_cache_next_id++;

#if R_GE_version >= 13

  SvgStreamPtr stream = svgd->stream;
  std::string extend = "spreadMethod=";

  // Cache current clipping and break out of clipping group
  bool was_clipping = svgd->is_clipping;
  std::string old_clipid = svgd->clipid;
  double clipx0 = svgd->clipx0;
  double clipx1 = svgd->clipx1;
  double clipy0 = svgd->clipy0;
  double clipy1 = svgd->clipy1;
  if (was_clipping) {
    (*stream) << "</g>\n";
  }
  svgd->set_clipping(false);

  (*stream) << "<defs>\n";

  switch(R_GE_patternType(pattern)) {
  case R_GE_linearGradientPattern:
    switch(R_GE_linearGradientExtend(pattern)) {
    case R_GE_patternExtendNone: ;
    case R_GE_patternExtendPad: extend += "'pad'"; break;
    case R_GE_patternExtendReflect: extend += "'reflect'"; break;
    case R_GE_patternExtendRepeat: extend += "'repeat"; break;
    }
    (*stream) << "<linearGradient id='pat-" << key << "' gradientUnits='userSpaceOnUse' " << extend;
    write_attr_dbl(stream, "x1", R_GE_linearGradientX1(pattern));
    write_attr_dbl(stream, "y1", R_GE_linearGradientY1(pattern));
    write_attr_dbl(stream, "x2", R_GE_linearGradientX2(pattern));
    write_attr_dbl(stream, "y2", R_GE_linearGradientY2(pattern));
    (*stream) << ">\n";
    for (int i = 0; i < R_GE_linearGradientNumStops(pattern); ++i) {
      int col = R_GE_linearGradientColour(pattern, i);
      (*stream) << "  <stop offset='" << R_GE_linearGradientStop(pattern, i);
      (*stream) << tfm::format("' stop-color='#%02X%02X%02X'", R_RED(col), R_GREEN(col), R_BLUE(col));
      (*stream) << " stop-opacity='" << double(R_ALPHA(col))/255.0 << "'/>\n";
    }
    (*stream) << "</linearGradient>\n";
    break;
  case R_GE_radialGradientPattern:
    switch(R_GE_radialGradientExtend(pattern)) {
    case R_GE_patternExtendNone: ;
    case R_GE_patternExtendPad: extend += "'pad'"; break;
    case R_GE_patternExtendReflect: extend += "'reflect'"; break;
    case R_GE_patternExtendRepeat: extend += "'repeat"; break;
    }
    (*stream) << "<radialGradient id='pat-" << key << "' gradientUnits='userSpaceOnUse' " << extend;
    write_attr_dbl(stream, "fx", R_GE_radialGradientCX1(pattern));
    write_attr_dbl(stream, "fy", R_GE_radialGradientCY1(pattern));
    write_attr_dbl(stream, "fr", R_GE_radialGradientR1(pattern));
    write_attr_dbl(stream, "cx", R_GE_radialGradientCX2(pattern));
    write_attr_dbl(stream, "cy", R_GE_radialGradientCY2(pattern));
    write_attr_dbl(stream, "r", R_GE_radialGradientR2(pattern));
    (*stream) << ">\n";
    for (int i = 0; i < R_GE_radialGradientNumStops(pattern); ++i) {
      int col = R_GE_radialGradientColour(pattern, i);
      (*stream) << "  <stop offset='" << R_GE_radialGradientStop(pattern, i);
      (*stream) << tfm::format("' stop-color='#%02X%02X%02X'", R_RED(col), R_GREEN(col), R_BLUE(col));
      (*stream) << " stop-opacity='" << double(R_ALPHA(col))/255.0 << "'/>\n";
    }
    (*stream) << "</radialGradient>\n";
    break;
  case R_GE_tilingPattern:
    (*stream) << "<pattern id='pat-" << key << "' patternUnits='userSpaceOnUse' ";
    write_attr_dbl(stream, "width", R_GE_tilingPatternWidth(pattern));
    write_attr_dbl(stream, "height", fabs(R_GE_tilingPatternHeight(pattern)));
    write_attr_dbl(stream, "x", R_GE_tilingPatternX(pattern));
    write_attr_dbl(stream, "y", R_GE_tilingPatternY(pattern));
    (*stream) << ">\n<g transform='translate(" << -R_GE_tilingPatternX(pattern);
    (*stream) << "," << -R_GE_tilingPatternY(pattern) + fabs(R_GE_tilingPatternHeight(pattern)) << ")'>\n";

    int old_mask = svgd->current_mask;
    svgd->current_mask = -1;

    SEXP R_fcall = PROTECT(Rf_lang1(R_GE_tilingPatternFunction(pattern)));
    Rf_eval(R_fcall, R_GlobalEnv);
    UNPROTECT(1);

    svgd->current_mask = old_mask;

    if (svgd->is_clipping) {
      (*stream) << "</g>\n";
    }
    svgd->set_clipping(false);
    (*stream) << "</g>\n</pattern>\n";
    break;
  }

  (*stream) << "</defs>\n";

  // Resume old clipping if it was happening
  if (was_clipping) {
    (*stream) << "<g";
    svgd->clipid = old_clipid;
    svgd->clipx0 = clipx0;
    svgd->clipx1 = clipx1;
    svgd->clipy0 = clipy0;
    svgd->clipy1 = clipy1;
    write_attr_clip(stream, svgd->clipid);
    (*stream) << ">\n";
    svgd->set_clipping(true);
  }

#endif

  svgd->pattern_cache.insert(key);

  return Rf_ScalarInteger(key);
}

void svg_release_pattern(SEXP ref, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (Rf_isNull(ref)) {
    svgd->pattern_cache.clear();
    return;
  }

  unsigned int key = INTEGER(ref)[0];
  auto it = svgd->pattern_cache.find(key);
  // Check if path exists
  if (it != svgd->pattern_cache.end()) {
    svgd->pattern_cache.erase(it);
  }

  return;
}

SEXP svg_set_clip_path(SEXP path, SEXP ref, pDevDesc dd) {
  int key;
  if (Rf_isNull(path)) {
    return Rf_ScalarInteger(-1);
  }
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (Rf_isNull(ref)) {
    key = svgd->clip_cache_next_id;
    svgd->clip_cache_next_id++;
  } else {
    key = INTEGER(ref)[0];
    if (key < 0) {
      return Rf_ScalarInteger(key);
    }
  }

  SvgStreamPtr stream = svgd->stream;
  if (svgd->is_clipping) {
    (*stream) << "</g>\n";
  }

  auto clip_cache_iter = svgd->clip_cache.find(key);
  // Check if path exists
  if (clip_cache_iter == svgd->clip_cache.end()) {

    bool new_clip_is_even_odd = false;
#if R_GE_version >= 15
    new_clip_is_even_odd = R_GE_clipPathFillRule(path) == R_GE_evenOddRule;
#endif


    (*stream) << "<defs>\n";
    (*stream) << "  <clipPath id='cp-" << key << "'>\n";
    (*stream) << "    <path d='";

    svgd->is_recording_clip = true;

    SEXP R_fcall = PROTECT(Rf_lang1(path));
    Rf_eval(R_fcall, R_GlobalEnv);
    UNPROTECT(1);

    svgd->is_recording_clip = false;

    (*stream) << "'";
    if (new_clip_is_even_odd) {
      (*stream) << "fill-rule='evenodd'";
    }
    (*stream) << "/>\n  </clipPath>\n";
    (*stream) << "</defs>\n";

    svgd->clip_cache.insert(key);
  }

  svgd->clipid = "-" + std::to_string(key);
  svgd->clipx0 = 0;
  svgd->clipx1 = 0;
  svgd->clipy0 = 0;
  svgd->clipy1 = 0;

  (*stream) << "<g";
  write_attr_clip(stream, svgd->clipid);
  (*stream) << ">\n";
  svgd->set_clipping(true);

  return Rf_ScalarInteger(key);
}

void svg_release_clip_path(SEXP ref, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (Rf_isNull(ref)) {
    svgd->clip_cache.clear();
    return;
  }

  int key = INTEGER(ref)[0];

  if (key < 0) {
    return;
  }

  auto it = svgd->clip_cache.find(key);
  // Check if path exists
  if (it != svgd->clip_cache.end()) {
    svgd->clip_cache.erase(it);
  }

  return;
}

SEXP svg_set_mask(SEXP path, SEXP ref, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  int key;
  if (Rf_isNull(path)) {
    svgd->current_mask = -1;
    return Rf_ScalarInteger(-1);
  }
  if (Rf_isNull(ref)) {
    key = svgd->mask_cache_next_id;
    svgd->mask_cache_next_id++;
  } else {
    key = INTEGER(ref)[0];
    if (key < 0) {
      svgd->current_mask = -1;
      return Rf_ScalarInteger(key);
    }
  }

  SvgStreamPtr stream = svgd->stream;

  auto mask_cache_iter = svgd->mask_cache.find(key);
  // Check if path exists
  if (mask_cache_iter == svgd->mask_cache.end()) {

    // Cache current clipping and break out of clipping group
    bool was_clipping = svgd->is_clipping;
    std::string old_clipid = svgd->clipid;
    double clipx0 = svgd->clipx0;
    double clipx1 = svgd->clipx1;
    double clipy0 = svgd->clipy0;
    double clipy1 = svgd->clipy1;
    if (was_clipping) {
      (*stream) << "</g>\n";
    }
    svgd->set_clipping(false);

    (*stream) << "<defs>\n";
    (*stream) << "  <mask id='mask-" << key << "' style='mask-type:alpha'>\n";

    SEXP R_fcall = PROTECT(Rf_lang1(path));
    Rf_eval(R_fcall, R_GlobalEnv);
    UNPROTECT(1);

    // Clipping may have happened above. End it before terminating mask
    if (svgd->is_clipping) {
      (*stream) << "</g>\n";
    }
    svgd->set_clipping(false);
    (*stream) << "  </mask>\n";
    (*stream) << "</defs>\n";

    // Resume old clipping if it was happening
    if (was_clipping) {
      (*stream) << "<g";
      svgd->clipid = old_clipid;
      svgd->clipx0 = clipx0;
      svgd->clipx1 = clipx1;
      svgd->clipy0 = clipy0;
      svgd->clipy1 = clipy1;
      write_attr_clip(stream, svgd->clipid);
      (*stream) << ">\n";
      svgd->set_clipping(true);
    }

    svgd->mask_cache.insert(key);
  }

  svgd->current_mask = key;

  return Rf_ScalarInteger(key);
}

void svg_release_mask(SEXP ref, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  if (Rf_isNull(ref)) {
    svgd->mask_cache.clear();
    return;
  }

  unsigned int key = INTEGER(ref)[0];

  auto it = svgd->mask_cache.find(key);
  // Check if path exists
  if (it != svgd->mask_cache.end()) {
    svgd->mask_cache.erase(it);
  }

  return;
}

pDevDesc svg_driver_new(SvgStreamPtr stream, int bg, double width,
                        double height, double pointsize,
                        bool standalone, cpp11::list& aliases,
                        const std::string& webfonts,
                        const std::string& file, cpp11::strings id,
                        bool fix_text_size, double scaling, bool always_valid) {

  pDevDesc dd = (DevDesc*) calloc(1, sizeof(DevDesc));
  if (dd == NULL)
    return dd;

  dd->startfill = bg;
  dd->startcol = R_RGB(0, 0, 0);
  dd->startps = pointsize;
  dd->startlty = 0;
  dd->startfont = 1;
  dd->startgamma = 1;

  // Callbacks
  dd->activate = NULL;
  dd->deactivate = NULL;
  dd->close = svg_close;
  dd->clip = svg_clip;
  dd->size = svg_size;
  dd->newPage = svg_new_page;
  dd->line = svg_line;
  dd->text = svg_text;
  dd->strWidth = svg_strwidth;
  dd->rect = svg_rect;
  dd->circle = svg_circle;
  dd->polygon = svg_polygon;
  dd->polyline = svg_polyline;
  dd->path = svg_path;
  dd->mode = NULL;
  dd->metricInfo = svg_metric_info;
  dd->cap = NULL;
  dd->raster = svg_raster;
#if R_GE_version >= 13
  dd->setPattern      = svg_set_pattern;
  dd->releasePattern  = svg_release_pattern;
  dd->setClipPath     = svg_set_clip_path;
  dd->releaseClipPath = svg_release_clip_path;
  dd->setMask         = svg_set_mask;
  dd->releaseMask     = svg_release_mask;
#endif

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8 = (Rboolean) 1;
  dd->textUTF8 = svg_text;
  dd->strWidthUTF8 = svg_strwidth;

  // Screen Dimensions in pts
  dd->left = 0;
  dd->top = 0;
  dd->right = width * 72;
  dd->bottom = height * 72;

  // Magic constants copied from other graphics devices
  // nominal character sizes in pts
  dd->cra[0] = 0.9 * pointsize * scaling;
  dd->cra[1] = 1.2 * pointsize * scaling;
  // character alignment offsets
  dd->xCharOffset = 0.4900;
  dd->yCharOffset = 0.3333;
  dd->yLineBias = 0.2;
  // inches per pt
  dd->ipr[0] = 1.0 / (72.0 * scaling);
  dd->ipr[1] = 1.0 / (72.0 * scaling);

  // Capabilities
  dd->canClip = TRUE;
  dd->canHAdj = 1;
  dd->canChangeGamma = FALSE;
  dd->displayListOn = FALSE;
  dd->haveTransparency = 2;
  dd->haveTransparentBg = 2;

#if R_GE_version >= 13
  dd->deviceVersion = R_GE_definitions;
#endif

  dd->deviceSpecific = new SVGDesc(stream, standalone, aliases, webfonts, file,
                                   id, fix_text_size, scaling, always_valid);
  return dd;
}

void makeDevice(SvgStreamPtr stream, std::string bg_, double width, double height,
                double pointsize, bool standalone, cpp11::list& aliases,
                const std::string& webfonts, const std::string& file,
                cpp11::strings id, bool fix_text_size, double scaling,
                bool always_valid) {

  int bg = R_GE_str2col(bg_.c_str());

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = svg_driver_new(stream, bg, width, height, pointsize,
                                  standalone, aliases, webfonts, file, id,
                                  fix_text_size, scaling, always_valid);
    if (dev == NULL)
      cpp11::stop("Failed to start SVG device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "devSVG");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;
}

[[cpp11::register]]
bool svglite_(std::string file, std::string bg, double width, double height,
              double pointsize, bool standalone, cpp11::list aliases,
              std::string webfonts, cpp11::strings id, bool fix_text_size,
              double scaling, bool always_valid) {

  SvgStreamPtr stream(new SvgStreamFile(file, 1, always_valid));
  makeDevice(stream, bg, width, height, pointsize, standalone, aliases, webfonts,
             file, id, fix_text_size, scaling, always_valid);

  return true;
}

[[cpp11::register]]
cpp11::external_pointer<std::stringstream> svgstring_(cpp11::environment env, std::string bg,
                                         double width, double height, double pointsize,
                                         bool standalone, cpp11::list aliases,
                                         std::string webfonts, cpp11::strings id,
                                         bool fix_text_size, double scaling) {

  SvgStreamPtr stream(new SvgStreamString(env));
  makeDevice(stream, bg, width, height, pointsize, standalone, aliases, webfonts,
             "", id, fix_text_size, scaling, true);

  SvgStreamString* strstream = static_cast<SvgStreamString*>(stream.get());

  return {strstream->string_src(), false};
}

[[cpp11::register]]
std::string get_svg_content(cpp11::external_pointer<std::stringstream> p) {
  p->flush();
  std::string svgstr = p->str();
  // If the current SVG is empty, we also make the string empty
  // Otherwise append "</svg>" to make it a valid SVG
  if(!svgstr.empty()) {
    svgstr.append("</g>\n</svg>");
  }
  return svgstr;
}

