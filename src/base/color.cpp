//---------------------------------------------------------------------------
// VREng (Virtual Reality Engine)	http://vreng.enst.fr/
//
// Copyright (C) 1997-2008 Philippe Dax
// Telecom-ParisTech (Ecole Nationale Superieure des Telecommunications)
//
// VREng is a free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public Licence as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VREng is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//---------------------------------------------------------------------------
#include "vreng.hpp"
#include "color.hpp"


struct sRgb {
  char name[24];
  float color[3];
};

struct sRgb rgb[] = {
  {"black", {0.00,0.00,0.00}},
  {"white", {1.00,1.00,1.00}},
  {"gray", {0.75,0.75,0.75}},
  {"grey", {0.75,0.75,0.75}},
  {"snow", {1.00,0.98,0.98}},
  {"GhostWhite", {0.97,0.97,1.00}},
  {"WhiteSmoke", {0.96,0.96,0.96}},
  {"gainsboro", {0.86,0.86,0.86}},
  {"FloralWhite", {1.00,0.98,0.94}},
  {"OldLace", {0.99,0.96,0.90}},
  {"linen", {0.98,0.94,0.90}},
  {"AntiqueWhite", {0.98,0.92,0.84}},
  {"PapayaWhip", {1.00,0.94,0.84}},
  {"BlanchedAlmond", {1.00,0.92,0.80}},
  {"bisque", {1.00,0.89,0.77}},
  {"PeachPuff", {1.00,0.85,0.73}},
  {"NavajoWhite", {1.00,0.87,0.68}},
  {"moccasin", {1.00,0.89,0.71}},
  {"cornsilk", {1.00,0.97,0.86}},
  {"ivory", {1.00,1.00,0.94}},
  {"LemonChiffon", {1.00,0.98,0.80}},
  {"seashell", {1.00,0.96,0.93}},
  {"honeydew", {0.94,1.00,0.94}},
  {"MintCream", {0.96,1.00,0.98}},
  {"azure", {0.94,1.00,1.00}},
  {"AliceBlue", {0.94,0.97,1.00}},
  {"lavender", {0.90,0.90,0.98}},
  {"LavenderBlush", {1.00,0.94,0.96}},
  {"MistyRose", {1.00,0.89,0.88}},
  {"DarkSlateGrey", {0.18,0.31,0.31}},
  {"DimGrey", {0.41,0.41,0.41}},
  {"SlateGrey", {0.44,0.50,0.56}},
  {"LightSlateGrey", {0.47,0.53,0.60}},
  {"LightGrey", {0.83,0.83,0.83}},
  {"MidnightBlue", {0.10,0.10,0.44}},
  {"navy", {0.00,0.00,0.50}},
  {"NavyBlue", {0.00,0.00,0.50}},
  {"CornflowerBlue", {0.39,0.58,0.93}},
  {"DarkSlateBlue", {0.28,0.24,0.55}},
  {"SlateBlue", {0.42,0.35,0.80}},
  {"LightSlateBlue", {0.52,0.44,1.00}},
  {"RoyalBlue", {0.25,0.41,0.88}},
  {"blue", {0.00,0.00,1.00}},
  {"DodgerBlue", {0.12,0.56,1.00}},
  {"DeepSkyBlue", {0.00,0.75,1.00}},
  {"SkyBlue", {0.53,0.81,0.92}},
  {"LightSkyBlue", {0.53,0.81,0.98}},
  {"SteelBlue", {0.27,0.51,0.71}},
  {"LightSteelBlue", {0.69,0.77,0.87}},
  {"LightBlue", {0.68,0.85,0.90}},
  {"PowderBlue", {0.69,0.88,0.90}},
  {"PaleTurquoise", {0.69,0.93,0.93}},
  {"DarkTurquoise", {0.00,0.81,0.82}},
  {"turquoise", {0.25,0.88,0.82}},
  {"cyan", {0.00,1.00,1.00}},
  {"LightCyan", {0.88,1.00,1.00}},
  {"CadetBlue", {0.37,0.62,0.63}},
  {"aquamarine", {0.50,1.00,0.83}},
  {"DarkGreen", {0.00,0.39,0.00}},
  {"DarkOliveGreen", {0.33,0.42,0.18}},
  {"DarkSeaGreen", {0.56,0.74,0.56}},
  {"SeaGreen", {0.18,0.55,0.34}},
  {"LightSeaGreen", {0.13,0.70,0.67}},
  {"PaleGreen", {0.60,0.98,0.60}},
  {"SpringGreen", {0.00,1.00,0.50}},
  {"LawnGreen", {0.49,0.99,0.00}},
  {"green", {0.00,1.00,0.00}},
  {"chartreuse", {0.50,1.00,0.00}},
  {"GreenYellow", {0.68,1.00,0.18}},
  {"LimeGreen", {0.20,0.80,0.20}},
  {"YellowGreen", {0.60,0.80,0.20}},
  {"ForestGreen", {0.13,0.55,0.13}},
  {"OliveDrab", {0.42,0.56,0.14}},
  {"DarkKhaki", {0.74,0.72,0.42}},
  {"khaki", {0.94,0.90,0.55}},
  {"PaleGoldenrod", {0.93,0.91,0.67}},
  {"LightYellow", {1.00,1.00,0.88}},
  {"yellow", {1.00,1.00,0.00}},
  {"gold", {1.00,0.84,0.00}},
  {"LightGoldenrod", {0.93,0.87,0.51}},
  {"goldenrod", {0.85,0.65,0.13}},
  {"DarkGoldenrod", {0.72,0.53,0.04}},
  {"RosyBrown", {0.74,0.56,0.56}},
  {"IndianRed", {0.80,0.36,0.36}},
  {"SaddleBrown", {0.55,0.27,0.07}},
  {"sienna", {0.63,0.32,0.18}},
  {"peru", {0.80,0.52,0.25}},
  {"burlywood", {0.87,0.72,0.53}},
  {"beige", {0.96,0.96,0.86}},
  {"wheat", {0.96,0.87,0.70}},
  {"SandyBrown", {0.96,0.64,0.38}},
  {"tan", {0.82,0.71,0.55}},
  {"chocolate", {0.82,0.41,0.12}},
  {"firebrick", {0.70,0.13,0.13}},
  {"brown", {0.65,0.16,0.16}},
  {"DarkSalmon", {0.91,0.59,0.48}},
  {"salmon", {0.98,0.50,0.45}},
  {"LightSalmon", {1.00,0.63,0.48}},
  {"orange", {1.00,0.65,0.00}},
  {"DarkOrange", {1.00,0.55,0.00}},
  {"coral", {1.00,0.50,0.31}},
  {"LightCoral", {0.94,0.50,0.50}},
  {"tomato", {1.00,0.39,0.28}},
  {"OrangeRed", {1.00,0.27,0.00}},
  {"red", {1.00,0.00,0.00}},
  {"HotPink", {1.00,0.41,0.71}},
  {"DeepPink", {1.00,0.08,0.58}},
  {"pink", {1.00,0.75,0.80}},
  {"LightPink", {1.00,0.71,0.76}},
  {"PaleVioletRed", {0.86,0.44,0.58}},
  {"maroon", {0.69,0.19,0.38}},
  {"VioletRed", {0.82,0.13,0.56}},
  {"magenta", {1.00,0.00,1.00}},
  {"violet", {0.93,0.51,0.93}},
  {"plum", {0.87,0.63,0.87}},
  {"orchid", {0.85,0.44,0.84}},
  {"DarkOrchid", {0.60,0.20,0.80}},
  {"DarkViolet", {0.58,0.00,0.83}},
  {"BlueViolet", {0.54,0.17,0.89}},
  {"purple", {0.63,0.13,0.94}},
  {"thistle", {0.85,0.75,0.85}},
  {"grey10", {0.10,0.10,0.10}},
  {"grey20", {0.20,0.20,0.20}},
  {"grey30", {0.30,0.30,0.30}},
  {"grey40", {0.40,0.40,0.40}},
  {"grey50", {0.50,0.50,0.50}},
  {"grey60", {0.60,0.60,0.60}},
  {"grey70", {0.70,0.70,0.70}},
  {"grey80", {0.80,0.80,0.80}},
  {"grey90", {0.90,0.90,0.90}},
  {"grey99", {0.99,0.99,0.99}},
  {"DarkGrey", {0.66,0.66,0.66}},
  {"DarkBlue", {0.00,0.00,0.55}},
  {"DarkCyan", {0.00,0.55,0.55}},
  {"DarkMagenta", {0.55,0.00,0.55}},
  {"DarkRed", {0.55,0.00,0.00}},
  {"LightGreen", {0.56,0.93,0.56}},
  {"none", {0,0,0}}
};


const Color Color::black(0, 0, 0, 1);
const Color Color::white(1, 1, 1, 1);
const Color Color::red(1, 0, 0, 1);
const Color Color::green(0, 1, 0, 1);
const Color Color::blue(0, 0, 1, 1);
const Color Color::yellow(1, 1, 0, 1);
const Color Color::cyan(0, 1, 1, 1);
const Color Color::magenta(1, 0, 1, 1);
const Color Color::grey(.5, .5, .5, 1);
const Color Color::orange(1, .65, 0, 1);
const Color Color::pink(1, .75, 0, 1);


Color::Color(const float* _color)
{
  for (int i = 0; i < 4; ++i)
    color[i] = _color[i];
}

Color::Color(float r, float g, float b, float a)
{
  color[0] = r; color[1] = g; color[2] = b; color[3] = a;
}

bool Color::operator==(const Color& c) const
{
  return ((color[0] == c.color[0])
	  && (color[1] == c.color[1])
	  && (color[2] == c.color[2])
	  && (color[3] == c.color[3]));
}

/* static */
void Color::getRGB(const char *name, float *v)
{
  const struct sRgb *p = rgb;

  if (name) {
    char *q;
    if ((q = const_cast<char *>(strchr(name, '"'))))
      *q = 0;
    for ( ; p ; p++) {
      if (! strcmp(p->name, "none"))
        break;
      if (! strcasecmp(p->name, name)) {
        v[0] = p->color[0];
        v[1] = p->color[1];
        v[2] = p->color[2];
        return;
      }
    }
    v[0] = v[1] = v[2] = 0;	//not found = black
  }
}
