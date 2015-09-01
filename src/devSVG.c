//  (C) 2012 Matthieu Decorde: UTF-8 support, XML reserved characters and XML header
//  (C) 2008 Tony Plate: Line type support from RSVGTipsDevice package
//  (C) 2002 T Jake Luciani: SVG device, based on PicTex device
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
#include <string.h>

#include "R.h"
#include "Rinternals.h"
#include "R_ext/GraphicsEngine.h"

// device-specific information per SVG device //

typedef struct {
  FILE *texfp;
  char filename[1024];
  int pageno;
  double width;
  double height;
  double clipleft, clipright, cliptop, clipbottom;

  Rboolean xmlHeader;
  Rboolean useNS;

} SVGDesc;

static double charwidth[4][128] = {
  { 0.5416690, 0.8333360, 0.7777810,
    0.6111145, 0.6666690, 0.7083380, 0.7222240, 0.7777810, 0.7222240,
    0.7777810, 0.7222240, 0.5833360, 0.5361130, 0.5361130, 0.8138910,
    0.8138910, 0.2388900, 0.2666680, 0.5000020, 0.5000020, 0.5000020,
    0.5000020, 0.5000020, 0.6666700, 0.4444460, 0.4805580, 0.7222240,
    0.7777810, 0.5000020, 0.8611145, 0.9722260, 0.7777810, 0.2388900,
    0.3194460, 0.5000020, 0.8333360, 0.5000020, 0.8333360, 0.7583360,
    0.2777790, 0.3888900, 0.3888900, 0.5000020, 0.7777810, 0.2777790,
    0.3333340, 0.2777790, 0.5000020, 0.5000020, 0.5000020, 0.5000020,
    0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020,
    0.5000020, 0.2777790, 0.2777790, 0.3194460, 0.7777810, 0.4722240,
    0.4722240, 0.6666690, 0.6666700, 0.6666700, 0.6388910, 0.7222260,
    0.5972240, 0.5694475, 0.6666690, 0.7083380, 0.2777810, 0.4722240,
    0.6944480, 0.5416690, 0.8750050, 0.7083380, 0.7361130, 0.6388910,
    0.7361130, 0.6458360, 0.5555570, 0.6805570, 0.6875050, 0.6666700,
    0.9444480, 0.6666700, 0.6666700, 0.6111130, 0.2888900, 0.5000020,
    0.2888900, 0.5000020, 0.2777790, 0.2777790, 0.4805570, 0.5166680,
    0.4444460, 0.5166680, 0.4444460, 0.3055570, 0.5000020, 0.5166680,
    0.2388900, 0.2666680, 0.4888920, 0.2388900, 0.7944470, 0.5166680,
    0.5000020, 0.5166680, 0.5166680, 0.3416690, 0.3833340, 0.3611120,
    0.5166680, 0.4611130, 0.6833360, 0.4611130, 0.4611130, 0.4347230,
    0.5000020, 1.0000030, 0.5000020, 0.5000020, 0.5000020 },
  { 0.5805590,
        0.9166720, 0.8555600, 0.6722260, 0.7333370, 0.7944490, 0.7944490,
        0.8555600, 0.7944490, 0.8555600, 0.7944490, 0.6416700, 0.5861150,
        0.5861150, 0.8916720, 0.8916720, 0.2555570, 0.2861130, 0.5500030,
        0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.7333370, 0.4888920,
        0.5652800, 0.7944490, 0.8555600, 0.5500030, 0.9472275, 1.0694500,
        0.8555600, 0.2555570, 0.3666690, 0.5583360, 0.9166720, 0.5500030,
        1.0291190, 0.8305610, 0.3055570, 0.4277800, 0.4277800, 0.5500030,
        0.8555600, 0.3055570, 0.3666690, 0.3055570, 0.5500030, 0.5500030,
        0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.5500030,
        0.5500030, 0.5500030, 0.5500030, 0.3055570, 0.3055570, 0.3666690,
        0.8555600, 0.5194470, 0.5194470, 0.7333370, 0.7333370, 0.7333370,
        0.7027820, 0.7944490, 0.6416700, 0.6111145, 0.7333370, 0.7944490,
        0.3305570, 0.5194470, 0.7638930, 0.5805590, 0.9777830, 0.7944490,
        0.7944490, 0.7027820, 0.7944490, 0.7027820, 0.6111145, 0.7333370,
        0.7638930, 0.7333370, 1.0388950, 0.7333370, 0.7333370, 0.6722260,
        0.3430580, 0.5583360, 0.3430580, 0.5500030, 0.3055570, 0.3055570,
        0.5250030, 0.5611140, 0.4888920, 0.5611140, 0.5111140, 0.3361130,
        0.5500030, 0.5611140, 0.2555570, 0.2861130, 0.5305590, 0.2555570,
        0.8666720, 0.5611140, 0.5500030, 0.5611140, 0.5611140, 0.3722250,
        0.4216690, 0.4041690, 0.5611140, 0.5000030, 0.7444490, 0.5000030,
        0.5000030, 0.4763920, 0.5500030, 1.1000060, 0.5500030, 0.5500030,
        0.550003 }, { 0.5416690, 0.8333360, 0.7777810, 0.6111145, 0.6666690,
            0.7083380, 0.7222240, 0.7777810, 0.7222240, 0.7777810, 0.7222240,
            0.5833360, 0.5361130, 0.5361130, 0.8138910, 0.8138910, 0.2388900,
            0.2666680, 0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020,
            0.7375210, 0.4444460, 0.4805580, 0.7222240, 0.7777810, 0.5000020,
            0.8611145, 0.9722260, 0.7777810, 0.2388900, 0.3194460, 0.5000020,
            0.8333360, 0.5000020, 0.8333360, 0.7583360, 0.2777790, 0.3888900,
            0.3888900, 0.5000020, 0.7777810, 0.2777790, 0.3333340, 0.2777790,
            0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020,
            0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.5000020, 0.2777790,
            0.2777790, 0.3194460, 0.7777810, 0.4722240, 0.4722240, 0.6666690,
            0.6666700, 0.6666700, 0.6388910, 0.7222260, 0.5972240, 0.5694475,
            0.6666690, 0.7083380, 0.2777810, 0.4722240, 0.6944480, 0.5416690,
            0.8750050, 0.7083380, 0.7361130, 0.6388910, 0.7361130, 0.6458360,
            0.5555570, 0.6805570, 0.6875050, 0.6666700, 0.9444480, 0.6666700,
            0.6666700, 0.6111130, 0.2888900, 0.5000020, 0.2888900, 0.5000020,
            0.2777790, 0.2777790, 0.4805570, 0.5166680, 0.4444460, 0.5166680,
            0.4444460, 0.3055570, 0.5000020, 0.5166680, 0.2388900, 0.2666680,
            0.4888920, 0.2388900, 0.7944470, 0.5166680, 0.5000020, 0.5166680,
            0.5166680, 0.3416690, 0.3833340, 0.3611120, 0.5166680, 0.4611130,
            0.6833360, 0.4611130, 0.4611130, 0.4347230, 0.5000020, 1.0000030,
            0.5000020, 0.5000020, 0.5000020 }, { 0.5805590, 0.9166720, 0.8555600,
                0.6722260, 0.7333370, 0.7944490, 0.7944490, 0.8555600, 0.7944490,
                0.8555600, 0.7944490, 0.6416700, 0.5861150, 0.5861150, 0.8916720,
                0.8916720, 0.2555570, 0.2861130, 0.5500030, 0.5500030, 0.5500030,
                0.5500030, 0.5500030, 0.8002530, 0.4888920, 0.5652800, 0.7944490,
                0.8555600, 0.5500030, 0.9472275, 1.0694500, 0.8555600, 0.2555570,
                0.3666690, 0.5583360, 0.9166720, 0.5500030, 1.0291190, 0.8305610,
                0.3055570, 0.4277800, 0.4277800, 0.5500030, 0.8555600, 0.3055570,
                0.3666690, 0.3055570, 0.5500030, 0.5500030, 0.5500030, 0.5500030,
                0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.5500030, 0.5500030,
                0.5500030, 0.3055570, 0.3055570, 0.3666690, 0.8555600, 0.5194470,
                0.5194470, 0.7333370, 0.7333370, 0.7333370, 0.7027820, 0.7944490,
                0.6416700, 0.6111145, 0.7333370, 0.7944490, 0.3305570, 0.5194470,
                0.7638930, 0.5805590, 0.9777830, 0.7944490, 0.7944490, 0.7027820,
                0.7944490, 0.7027820, 0.6111145, 0.7333370, 0.7638930, 0.7333370,
                1.0388950, 0.7333370, 0.7333370, 0.6722260, 0.3430580, 0.5583360,
                0.3430580, 0.5500030, 0.3055570, 0.3055570, 0.5250030, 0.5611140,
                0.4888920, 0.5611140, 0.5111140, 0.3361130, 0.5500030, 0.5611140,
                0.2555570, 0.2861130, 0.5305590, 0.2555570, 0.8666720, 0.5611140,
                0.5500030, 0.5611140, 0.5611140, 0.3722250, 0.4216690, 0.4041690,
                0.5611140, 0.5000030, 0.7444490, 0.5000030, 0.5000030, 0.4763920,
                0.5500030, 1.1000060, 0.5500030, 0.5500030, 0.550003 } };

void write_escaped(FILE* f, const char* text) {
  for(const char* cur = text; *cur != '\0'; ++cur) {
    switch(*cur) {
    case '&': fprintf(f, "&amp;"); break;
    case '<': fprintf(f, "&lt;"); break;
    case '>': fprintf(f, "&gt;"); break;
    default: fputc(*cur, f);
    }
  }
}

void write_colour(FILE* f, unsigned int col) {
  int alpha = R_ALPHA(col);

  if (col == NA_INTEGER || alpha == 0) {
    fprintf(f, "none");
    return;
  } else if (alpha == 255) {
    fprintf(f, "#%02X%02X%02X", R_RED(col), R_GREEN(col), R_BLUE(col));
  } else {
    fprintf(f, "rgba(%i, %i, %i, %0.2f)", R_RED(col), R_GREEN(col), R_BLUE(col),
      alpha / 255.0);
  }
}

static void write_fill(FILE* f, int fgcol) {
  fprintf(f, " fill='");
  write_colour(f, fgcol);
  fprintf(f, "'");
}

static void write_linetype(FILE* f, int lty, double lwd, int col) {
  fprintf(f, " stroke='");
  write_colour(f, col);
  fprintf(f, "'");

  fprintf(f, " stroke-width='%.3f'", lwd);

  // Set line pattern type
	switch (lty) {
	case LTY_BLANK:
		break;
	case LTY_SOLID:
		break;
	case LTY_DOTTED:
		fputs(" stroke-dasharray=\"1,5\"", f);
		break;
	case LTY_DASHED:
		fputs(" stroke-dasharray=\"5,5\"", f);
		break;
	case LTY_LONGDASH:
		fputs(" stroke-dasharray=\"10,5\"", f);
		break;
	case LTY_DOTDASH:
		fputs(" stroke-dasharray=\"1,5,5,5\"", f);
		break;
	case LTY_TWODASH:
		fputs(" stroke-dasharray=\"10,5,5,5\"", f);
		break;
	default: {
		  int newlty = lty;
    	double newlwd = lwd;
  		fputs(" stroke-dasharray=\"", f);
  		for(int i = 0 ; i < 8 && newlty & 15; i++) {
  			int lwd = (int) newlwd * newlty;
  			lwd = lwd & 15;
  			if(i > 0)
  			  fputc(',', f);
  			fprintf(f, "%i", lwd);
  			newlty = newlty >> 4;
  		}
  		fputs("\"", f);
	  	break;
		}
	}
}

static void write_font(FILE* f, int face, int size, unsigned int col) {
  fprintf(f, " font-size='%d'", size);

  fprintf(f, " fill='");
  write_colour(f, col);
  fprintf(f, "'");
}

// Callback functions for graphics device --------------------------------------

static void SVG_MetricInfo(int c, const pGEcontext gc, double* ascent,
    double* descent, double* width, pDevDesc dd) {

  //  Rboolean Unicode = mbcslocale && (gc->fontface != 5);
    //     if (c < 0) { Unicode = TRUE; c = -c; }
    //     if(Unicode) UniCharMetric(c, ...); else CharMetric(c, ...);
  // metric information not available => return 0,0,0 */
  *ascent = 0.0;
  *descent = 0.0;
  *width = 0.0;
}

static void SVG_Clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  SVGDesc *ptd = dd->deviceSpecific;

  ptd->clipleft = x0;
  ptd->clipright = x1;
  ptd->clipbottom = y0;
  ptd->cliptop = y1;
}

static void SVG_NewPage(const pGEcontext gc, pDevDesc dd) {
  SVGDesc *ptd = dd->deviceSpecific;

  if (ptd->pageno > 0) {
    Rf_error("RSvgDevice only supports one page");
  }

  if (ptd->xmlHeader)
    fprintf(ptd->texfp, "<?xml version='1.0' encoding='UTF-8'?>\n");

  fprintf(ptd->texfp, "<svg ");
  if (ptd->useNS)
    fprintf(ptd->texfp, "xmlns='http://www.w3.org/2000/svg' ");

  fprintf(ptd->texfp, "viewBox='0,0,%.2f,%.2f'>\n", ptd->width * 72.0,
      ptd->height * 72.0);

  fprintf(ptd->texfp, "<rect width='100%%' height='100%%' fill='");
  write_colour(ptd->texfp, gc->fill);
  fprintf(ptd->texfp, "' />\n");

  ptd->pageno++;
}

static void SVG_Close(pDevDesc dd) {
  SVGDesc *ptd = dd->deviceSpecific;

  fprintf(ptd->texfp, "</svg>\n");

  fclose(ptd->texfp);

  free(ptd);
}

static void SVG_Line(double x1, double y1, double x2, double y2,
    const pGEcontext gc, pDevDesc dd) {
  SVGDesc *ptd = dd->deviceSpecific;

  fprintf(ptd->texfp, "<line x1='%.2f' y1='%.2f' x2='%.2f' y2='%.2f'",
    x1, y1, x2, y2);

  write_linetype(ptd->texfp, gc->lty, gc->lwd, gc->col);
  fprintf(ptd->texfp, " />\n");
}

static void SVG_Polyline(int n, double *x, double *y, const pGEcontext gc,
      pDevDesc dd) {
  int i;
  SVGDesc *ptd = dd->deviceSpecific;
  fprintf(ptd->texfp, "<polyline points='");

  for (i = 0; i < n; i++) {
    fprintf(ptd->texfp, "%.2f,%.2f ", x[i], y[i]);
  }
  fprintf(ptd->texfp, "'");

  write_fill(ptd->texfp, NA_INTEGER);
  write_linetype(ptd->texfp, gc->lty, gc->lwd, gc->col);

  fprintf(ptd->texfp, " />\n");
}

static double SVG_StrWidth(const char *str, const pGEcontext gc, pDevDesc dd) {
  int fontface = gc->fontface == NA_INTEGER ? 0 : gc->fontface;

  double sum = 0;
  for (const char *p = str; *p; p++)
    sum += charwidth[fontface][(int) *p];

  int size = gc->cex * gc->ps + 0.5;
  return sum * size;
}

static void SVG_Rect(double x0, double y0, double x1, double y1,
    const pGEcontext gc, pDevDesc dd) {
  double tmp;
  SVGDesc *ptd = dd->deviceSpecific;

  // Make sure width and height are positive
  if (x0 >= x1) {
    tmp = x0;
    x0 = x1;
    x1 = tmp;
  }

  if (y0 >= y1) {
    tmp = y0;
    y0 = y1;
    y1 = tmp;
  }

  fprintf(ptd->texfp,
      "<rect x='%.2f' y='%.2f' width='%.2f' height='%.2f'",
      x0, y0, x1 - x0, y1 - y0);

  write_fill(ptd->texfp, gc->fill);
  write_linetype(ptd->texfp, gc->lty, gc->lwd, gc->col);
  fprintf(ptd->texfp, " />\n");
}

static void SVG_Circle(double x, double y, double r, const pGEcontext gc,
                       pDevDesc dd) {
  SVGDesc *ptd = dd->deviceSpecific;

  fprintf(ptd->texfp, "<circle cx='%.2f' cy='%.2f' r='%.2f'", x, y, r * 1.5);
  write_fill(ptd->texfp, gc->fill);
  write_linetype(ptd->texfp, gc->lty, gc->lwd, gc->col);
  fprintf(ptd->texfp, " />\n");
}

static void SVG_Polygon(int n, double *x, double *y, const pGEcontext gc,
                        pDevDesc dd) {
  int i;

  SVGDesc *ptd = dd->deviceSpecific;

  fprintf(ptd->texfp, "<polygon points='");
  for (i = 0; i < n; i++) {
    fprintf(ptd->texfp, "%.2f,%.2f ", x[i], y[i]);
  }
  fprintf(ptd->texfp, "'");

  write_fill(ptd->texfp, gc->fill);
  write_linetype(ptd->texfp, gc->lty, gc->lwd, gc->col);

  fprintf(ptd->texfp, " />\n");
}

static void SVG_Text(double x, double y, const char *str, double rot,
                     double hadj, const pGEcontext gc, pDevDesc dd) {

  SVGDesc *ptd = dd->deviceSpecific;

  fprintf(ptd->texfp, "<text transform='translate(%.2f,%.2f)", x, y);
  if (rot != 0)
    fprintf(ptd->texfp, " rotate(%0.0f)", -1.0 * rot);
  fprintf(ptd->texfp, "' ");
  int size = gc->cex * gc->ps + 0.5;
  write_font(ptd->texfp, gc->fontface, size, gc->col);
  fprintf(ptd->texfp, ">");

  write_escaped(ptd->texfp, str);

  fprintf(ptd->texfp, "</text>\n");
}

static void SVG_Size(double *left, double *right, double *bottom, double *top,
                     pDevDesc dd) {
  *left = dd->left;
  *right = dd->right;
  *bottom = dd->bottom;
  *top = dd->top;
}

Rboolean SVGDeviceDriver(pDevDesc dd, const char *filename, int bg,
    double width, double height, int pointsize, Rboolean xmlHeader, Rboolean useNS) {

  dd->startfill = bg;
  dd->startcol = R_RGB(0, 0, 0);
  dd->startps = pointsize;
  dd->startlty = 0;
  dd->startfont = 1;
  dd->startgamma = 1;

  dd->activate = NULL;
  dd->deactivate = NULL;
  dd->close = SVG_Close;
  dd->clip = SVG_Clip;
  dd->size = SVG_Size;
  dd->newPage = SVG_NewPage;
  dd->line = SVG_Line;
  dd->text = SVG_Text;
  dd->textUTF8 = SVG_Text; // UTF-8 support
  dd->strWidth = SVG_StrWidth;
  dd->strWidthUTF8 = SVG_StrWidth; // UTF-8 support
  dd->rect = SVG_Rect;
  dd->circle = SVG_Circle;
  dd->polygon = SVG_Polygon;
  dd->polyline = SVG_Polyline;
  dd->mode = NULL;
  dd->metricInfo = SVG_MetricInfo;
  dd->cap = NULL;
  dd->raster = NULL; // not implemented

  // UTF-8 support
  dd->wantSymbolUTF8 = 1;
  dd->hasTextUTF8 = 1;

  // Screen Dimensions in Pixels
  dd->left = 0;
  dd->top = 0;
  dd->right = width * 72;
  dd->bottom = height * 72;

  // Base Pointsize: Nominal Character Sizes in Pixels
  // I'm not sure where these constants come from, but they're used in
  // the majority of the base grahpics devices
  dd->cra[0] = 0.9 * pointsize;
  dd->cra[1] = 1.2 * pointsize;

  // Character Addressing Offsets: These offsets should center a single,
  // plotting character over the plotting point.
  dd->xCharOffset = 0; // 0.4900;
  dd->yCharOffset = 0; // 0.3333;
  dd->yLineBias = 0; // 0.1;

  // Inches per Raster Unit
  dd->ipr[0] = 1.0 / 72.0;
  dd->ipr[1] = 1.0 / 72.0;

  dd->canClip = FALSE;
  dd->canHAdj = 0;
  dd->canChangeGamma = FALSE;
  dd->displayListOn = FALSE;

  // Device specific setup
  SVGDesc *ptd = malloc(sizeof(SVGDesc));
  if (ptd == NULL)
    return FALSE;

  strncpy(ptd->filename, filename, 1024);
  ptd->texfp = fopen(R_ExpandFileName(ptd->filename), "w");
  if (ptd->texfp == NULL) {
    free(ptd);
    return FALSE;
  }

  ptd->width = width;
  ptd->height = height;
  ptd->xmlHeader = xmlHeader;
  ptd->useNS = useNS;
  ptd->pageno = 0;

  dd->deviceSpecific = ptd;
  return TRUE;
}


SEXP devSVG_(SEXP file_, SEXP bg_, SEXP width_, SEXP height_,
            SEXP pointsize_, SEXP xmlHeader_, SEXP useNS_) {

  const char* file = CHAR(asChar(file_));
  int bg = R_GE_str2col(CHAR(asChar(bg_)));
  int width = asInteger(width_), height = asInteger(height_);
  int pointsize = asInteger(pointsize_);
  int xmlHeader = asLogical(xmlHeader_), useNS = asLogical(useNS_);

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = calloc(1, sizeof(DevDesc));
    if (dev == NULL)
      error("unable to allocate memory for DevDesc");

    if (!SVGDeviceDriver(dev, file, bg, width, height, pointsize, xmlHeader, useNS)) {
      free(dev);
      error("unable to start device SVG");
    }

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "devSVG");
    GEinitDisplayList(dd);

  }END_SUSPEND_INTERRUPTS;

  return ScalarLogical(1);
}

