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
#include <Rcpp.h>
#include <gdtools.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <R_ext/GraphicsEngine.h>

#include "SvgStream.h"
#include "utils.h"

typedef boost::shared_ptr<SvgStream> SvgStreamPtr;

// SVG device metadata
class SVGDesc {
public:
  SvgStreamPtr stream;

  int pageno;
  std::string clipid;  // ID for the clip path
  double clipx0, clipx1, clipy0, clipy1;  // Save the previous clip path to avoid duplication
  bool standalone;
  Rcpp::List system_aliases;
  Rcpp::List user_aliases;
  XPtrCairoContext cc;

  SVGDesc(SvgStreamPtr stream_, bool standalone_, Rcpp::List aliases_):
      stream(stream_),
      pageno(0),
      clipx0(0), clipx1(0), clipy0(0), clipy1(0),
      standalone(standalone_),
      system_aliases(Rcpp::wrap(aliases_["system"])),
      user_aliases(Rcpp::wrap(aliases_["user"])),
      cc(gdtools::context_create()) {
  }
};

inline bool is_black(int col) {
  return (R_RED(col) == 0) && (R_GREEN(col) == 0) && (R_BLUE(col) == 0) &&
    (R_ALPHA(col) == 255);
}

inline bool is_filled(int col) {
  return R_ALPHA(col) != 0;
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

inline std::string find_alias_field(std::string& family, Rcpp::List& alias,
                                    const char* face, const char* field) {
  if (alias.containsElementNamed(face)) {
    Rcpp::List font = alias[face];
    if (font.containsElementNamed(field))
      return font[field];
  }
  return std::string();
}

inline std::string find_user_alias(std::string& family,
                                   Rcpp::List const& aliases,
                                   int face, const char* field) {
  std::string out;
  if (aliases.containsElementNamed(family.c_str())) {
    Rcpp::List alias = aliases[family];
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

inline std::string find_system_alias(std::string& family,
                                     Rcpp::List const& aliases) {
  std::string out;
  if (aliases.containsElementNamed(family.c_str())) {
    SEXP alias = aliases[family];
    if (TYPEOF(alias) == STRSXP && Rf_length(alias) == 1)
      out = Rcpp::as<std::string>(alias);
  }
  return out;
}

inline std::string fontname(const char* family_, int face,
                            Rcpp::List const& system_aliases,
                            Rcpp::List const& user_aliases) {
  std::string family(family_);
  if (face == 5)
    family = "symbol";
  else if (family == "")
    family = "sans";

  std::string alias = find_system_alias(family, system_aliases);
  if (!alias.size())
    alias = find_user_alias(family, user_aliases, face, "name");

  if (alias.size())
    return alias;
  else
    return family;
}

inline std::string fontfile(const char* family_, int face,
                            Rcpp::List user_aliases) {
  std::string family(family_);
  if (face == 5)
    family = "symbol";
  else if (family == "")
    family = "sans";

  return find_user_alias(family, user_aliases, face, "file");
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

// Beginning of writing style attributes
inline void write_style_begin(SvgStreamPtr stream) {
  (*stream) << " style='";
}

// End of writing style attributes
inline void write_style_end(SvgStreamPtr stream) {
  (*stream) << "'";
}

// Writing style attributes related to colors
inline void write_style_col(SvgStreamPtr stream, const char* attr, int col, bool first = false) {
  int alpha = R_ALPHA(col);

  if(!first)  (*stream) << ' ';

  if (alpha == 0) {
    (*stream) << attr << ": none;";
    return;
  } else {
    (*stream) << tfm::format("%s: #%02X%02X%02X;", attr, R_RED(col), R_GREEN(col), R_BLUE(col));
    if (alpha != 255)
      (*stream) << ' ' << attr << "-opacity: " << alpha / 255.0 << ';';
  }
}

// Writing style attributes whose values are double type
inline void write_style_dbl(SvgStreamPtr stream, const char* attr, double value, bool first = false) {
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
inline void write_style_linetype(SvgStreamPtr stream, const pGEcontext gc, bool first = false) {
  int lty = gc->lty;

  // 1 lwd = 1/96", but units in rest of document are 1/72"
  write_style_dbl(stream, "stroke-width", gc->lwd / 96.0 * 72, first);

  // Default is "stroke: #000000;" as declared in <style>
  if (!is_black(gc->col))
    write_style_col(stream, "stroke", gc->col);

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
    (*stream) << scale_lty(lty, gc->lwd);
    lty = lty >> 4;
    // Remaining numbers
    for(int i = 1 ; i < 8 && lty & 15; i++) {
      (*stream) << ',' << scale_lty(lty, gc->lwd);
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


// Callback functions for graphics device --------------------------------------

void svg_metric_info(int c, const pGEcontext gc, double* ascent,
                     double* descent, double* width, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  bool is_unicode = mbcslocale;
  if (c < 0) {
    is_unicode = true;
    c = -c;
  }

  // Convert to string - negative implies unicode code point
  char str[16];
  if (is_unicode) {
    Rf_ucstoutf8(str, (unsigned int) c);
  } else {
    str[0] = (char) c;
    str[1] = '\0';
  }

  std::string file = fontfile(gc->fontfamily, gc->fontface, svgd->user_aliases);
  std::string name = fontname(gc->fontfamily, gc->fontface, svgd->system_aliases, svgd->user_aliases);
  gdtools::context_set_font(svgd->cc, name, gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface), file);
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));

  *ascent = fm.ascent;
  *descent = fm.descent;
  *width = fm.width;
}

void svg_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  SvgStreamPtr stream = svgd->stream;

  // Avoid duplication
  if (std::abs(x0 - svgd->clipx0) < 0.01 &&
      std::abs(x1 - svgd->clipx1) < 0.01 &&
      std::abs(y0 - svgd->clipy0) < 0.01 &&
      std::abs(y1 - svgd->clipy1) < 0.01)
    return;

  std::ostringstream s;
  s << std::fixed << std::setprecision(2);
  s << dbl_format(x0) << "|" << dbl_format(x1) << "|" <<
       dbl_format(y0) << "|" << dbl_format(y1);
  std::string clipid = gdtools::base64_string_encode(s.str());

  svgd->clipid = clipid;
  svgd->clipx0 = x0;
  svgd->clipx1 = x1;
  svgd->clipy0 = y0;
  svgd->clipy1 = y1;

  (*stream) << "<defs>\n";
  (*stream) << "  <clipPath id='cp" << svgd->clipid << "'>\n";
  (*stream) << "    <rect x='" << std::min(x0, x1) << "' y='" << std::min(y0, y1) <<
    "' width='" << std::abs(x1 - x0) << "' height='" << std::abs(y1 - y0) << "' />\n";
  (*stream) << "  </clipPath>\n";
  (*stream) << "</defs>\n";
  stream->flush();
}

void svg_new_page(const pGEcontext gc, pDevDesc dd) {
BEGIN_RCPP

  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  SvgStreamPtr stream = svgd->stream;

  if (svgd->pageno > 0) {
    Rcpp::stop("svglite only supports one page");
  }

  if (svgd->standalone)
    (*stream) << "<?xml version='1.0' encoding='UTF-8' ?>\n";

  (*stream) << "<svg";
  if (svgd->standalone){
    (*stream) << " xmlns='http://www.w3.org/2000/svg'";
    //http://www.w3.org/wiki/SVG_Links
    (*stream) << " xmlns:xlink='http://www.w3.org/1999/xlink'";
  }

  (*stream) << " viewBox='0 0 " << dd->right << ' ' << dd->bottom << "'>\n";

  // Initialise clipping the same way R does
  svgd->clipx0 = 0;
  svgd->clipy0 = dd->bottom;
  svgd->clipx1 = dd->right;
  svgd->clipy1 = 0;

  // Setting default styles
  (*stream) << "<defs>\n";
  (*stream) << "  <style type='text/css'><![CDATA[\n";
  (*stream) << "    line, polyline, polygon, path, rect, circle {\n";
  (*stream) << "      fill: none;\n";
  (*stream) << "      stroke: #000000;\n";
  (*stream) << "      stroke-linecap: round;\n";
  (*stream) << "      stroke-linejoin: round;\n";
  (*stream) << "      stroke-miterlimit: 10.00;\n";
  (*stream) << "    }\n";
  (*stream) << "  ]]></style>\n";
  (*stream) << "</defs>\n";

  (*stream) << "<rect width='100%' height='100%'";
  write_style_begin(stream);
  write_style_str(stream, "stroke", "none", true);
  if (is_filled(gc->fill))
    write_style_col(stream, "fill", gc->fill);
  else
    write_style_col(stream, "fill", dd->startfill);
  write_style_end(stream);
  (*stream) << "/>\n";

  svgd->stream->flush();
  svgd->pageno++;

VOID_END_RCPP
}

void svg_close(pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  svgd->stream->finish();
  delete(svgd);
}

void svg_line(double x1, double y1, double x2, double y2,
              const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  SvgStreamPtr stream = svgd->stream;

  (*stream) << "<line x1='" << x1 << "' y1='" << y1 << "' x2='" <<
    x2 << "' y2='" << y2 << '\'';

  write_style_begin(stream);
  write_style_linetype(stream, gc, true);
  write_style_end(stream);

  write_attr_clip(stream, svgd->clipid);

  (*stream) << " />\n";
  stream->flush();
}

void svg_poly(int n, double *x, double *y, int filled, const pGEcontext gc,
              pDevDesc dd, const char* node_name) {

  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  SvgStreamPtr stream = svgd->stream;

  (*stream) << "<" << node_name << " points='";

  for (int i = 0; i < n; i++) {
    (*stream) << x[i] << ',' << y[i] << ' ';
  }
  stream->put('\'');

  write_style_begin(stream);
  write_style_linetype(stream, gc, true);
  if (filled)
    write_style_col(stream, "fill", gc->fill);
  write_style_end(stream);

  write_attr_clip(stream, svgd->clipid);

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
  SvgStreamPtr stream = svgd->stream;

  // Create path data
  (*stream) << "<path d='";
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
  // Finish path data
  stream->put('\'');

  write_style_begin(stream);
  // Specify fill rule
  write_style_str(stream, "fill-rule", winding ? "nonzero" : "evenodd", true);
  if (is_filled(gc->fill))
    write_style_col(stream, "fill", gc->fill);
  write_style_linetype(stream, gc);
  write_style_end(stream);

  write_attr_clip(stream, svgd->clipid);

  (*stream) << " />\n";
  stream->flush();
}

double svg_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;

  std::string file = fontfile(gc->fontfamily, gc->fontface, svgd->user_aliases);
  std::string name = fontname(gc->fontfamily, gc->fontface, svgd->system_aliases, svgd->user_aliases);
  gdtools::context_set_font(svgd->cc, name, gc->cex * gc->ps, is_bold(gc->fontface), is_italic(gc->fontface), file);
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));

  return fm.width;
}

void svg_rect(double x0, double y0, double x1, double y1,
              const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  SvgStreamPtr stream = svgd->stream;

  // x and y give top-left position
  (*stream) << "<rect x='" << fmin(x0, x1) << "' y='" << fmin(y0, y1) <<
    "' width='" << fabs(x1 - x0) << "' height='" << fabs(y1 - y0) << '\'';

  write_style_begin(stream);
  write_style_linetype(stream, gc, true);
  if (is_filled(gc->fill))
    write_style_col(stream, "fill", gc->fill);
  write_style_end(stream);

  write_attr_clip(stream, svgd->clipid);

  (*stream) << " />\n";
  stream->flush();
}

void svg_circle(double x, double y, double r, const pGEcontext gc,
                       pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  SvgStreamPtr stream = svgd->stream;

  (*stream) << "<circle cx='" << x << "' cy='" << y << "' r='" << r << "'";

  write_style_begin(stream);
  write_style_linetype(stream, gc, true);
  if (is_filled(gc->fill))
    write_style_col(stream, "fill", gc->fill);
  write_style_end(stream);

  write_attr_clip(stream, svgd->clipid);

  (*stream) << " />\n";
  stream->flush();
}

void svg_text(double x, double y, const char *str, double rot,
              double hadj, const pGEcontext gc, pDevDesc dd) {
  SVGDesc *svgd = (SVGDesc*) dd->deviceSpecific;
  SvgStreamPtr stream = svgd->stream;

  // If we specify the clip path inside <text>, the "transform" also
  // affects the clip path, so we need to specify clip path at an outer level
  if (svgd->clipid.size()) {
    (*stream) << "<g";
    write_attr_clip(stream, svgd->clipid);
    stream->put('>');
  }

  (*stream) << "<text";

  if (rot == 0) {
    write_attr_dbl(stream, "x", x);
    write_attr_dbl(stream, "y", y);
  } else {
    (*stream) << tfm::format(" transform='translate(%0.2f,%0.2f) rotate(%0.0f)'",
      x, y, -1.0 * rot);
  }

  double fontsize = gc->cex * gc->ps;

  write_style_begin(stream);
  write_style_fontsize(stream, fontsize, true);
  if (is_bold(gc->fontface))
    write_style_str(stream, "font-weight", "bold");
  if (is_italic(gc->fontface))
    write_style_str(stream, "font-style", "italic");
  if (!is_black(gc->col))
    write_style_col(stream, "fill", gc->col);

  std::string font = fontname(gc->fontfamily, gc->fontface, svgd->system_aliases, svgd->user_aliases);
  write_style_str(stream, "font-family", font.c_str());
  write_style_end(stream);

  std::string file = fontfile(gc->fontfamily, gc->fontface, svgd->user_aliases);
  gdtools::context_set_font(svgd->cc, font, fontsize, is_bold(gc->fontface), is_italic(gc->fontface), file);
  FontMetric fm = gdtools::context_extents(svgd->cc, std::string(str));
  (*stream) << " textLength='" << fm.width << "px'";
  (*stream) << " lengthAdjust='spacingAndGlyphs'";
  stream->put('>');

  write_escaped(stream, str);

  (*stream) << "</text>";

  if (svgd->clipid.size())
    (*stream) << "</g>";

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
  SvgStreamPtr stream = svgd->stream;

  if (height < 0)
    height = -height;

  std::vector<unsigned int> raster_(w*h);
  for (std::vector<unsigned int>::size_type i = 0 ; i < raster_.size(); ++i) {
    raster_[i] = raster[i] ;
  }

  std::string base64_str = gdtools::raster_to_str(raster_, w, h, width, height,
    (Rboolean) interpolate);

  // If we specify the clip path inside <image>, the "transform" also
  // affects the clip path, so we need to specify clip path at an outer level
  if (svgd->clipid.size()) {
    (*stream) << "<g";
    write_attr_clip(stream, svgd->clipid);
    stream->put('>');
  }

  (*stream) << "<image";
  write_attr_dbl(stream, "width", width);
  write_attr_dbl(stream, "height", height);
  write_attr_dbl(stream, "x", x);
  write_attr_dbl(stream, "y", y - height);

  if( rot != 0 ){
    (*stream) << tfm::format(" transform='rotate(%0.0f,%.2f,%.2f)'", -1.0 * rot, x, y);
  }

  (*stream) << " xlink:href='data:image/png;base64," << base64_str << '\'';
  (*stream) << "/>";

  if (svgd->clipid.size())
    (*stream) << "</g>";

  stream->put('\n');
  stream->flush();
}


pDevDesc svg_driver_new(SvgStreamPtr stream, int bg, double width,
                        double height, double pointsize,
                        bool standalone, Rcpp::List& aliases) {

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
  dd->cra[0] = 0.9 * pointsize;
  dd->cra[1] = 1.2 * pointsize;
  // character alignment offsets
  dd->xCharOffset = 0.4900;
  dd->yCharOffset = 0.3333;
  dd->yLineBias = 0.2;
  // inches per pt
  dd->ipr[0] = 1.0 / 72.0;
  dd->ipr[1] = 1.0 / 72.0;

  // Capabilities
  dd->canClip = TRUE;
  dd->canHAdj = 0;
  dd->canChangeGamma = FALSE;
  dd->displayListOn = FALSE;
  dd->haveTransparency = 2;
  dd->haveTransparentBg = 2;

  dd->deviceSpecific = new SVGDesc(stream, standalone, aliases);
  return dd;
}

void makeDevice(SvgStreamPtr stream, std::string bg_, double width, double height,
                double pointsize, bool standalone, Rcpp::List& aliases) {

  int bg = R_GE_str2col(bg_.c_str());

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = svg_driver_new(stream, bg, width, height, pointsize,
                                  standalone, aliases);
    if (dev == NULL)
      Rcpp::stop("Failed to start SVG device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "devSVG");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;
}

// [[Rcpp::export]]
bool svglite_(std::string file, std::string bg, double width, double height,
              double pointsize, bool standalone, Rcpp::List aliases) {

  SvgStreamPtr stream(new SvgStreamFile(file));
  makeDevice(stream, bg, width, height, pointsize, standalone, aliases);

  return true;
}

// [[Rcpp::export]]
Rcpp::XPtr<std::stringstream> svgstring_(Rcpp::Environment env, std::string bg,
                                         double width, double height, double pointsize,
                                         bool standalone, Rcpp::List aliases) {

  SvgStreamPtr stream(new SvgStreamString(env));
  makeDevice(stream, bg, width, height, pointsize, standalone, aliases);

  SvgStreamString* strstream = static_cast<SvgStreamString*>(stream.get());

  return strstream->string_src();
}

// [[Rcpp::export]]
std::string get_svg_content(Rcpp::XPtr<std::stringstream> p) {
  p->flush();
  std::string svgstr = p->str();
  // If the current SVG is empty, we also make the string empty
  // Otherwise append "</svg>" to make it a valid SVG
  if(!svgstr.empty()) {
    svgstr.append("</svg>");
  }
  return svgstr;
}
