#ifndef DRAWING_H
#define DRAWING_H

#include "SDLA.h"
#include "polygons.h"
#include "geom_tools.h"

Point pointFromCoord(double xOffset, double yOffset, double x, double y);
void drawPoint(const Point *point, const SDL_Color *color);
void drawSegment(const Segment *segment, const SDL_Color *color);
void drawPolygon(const Polygon *polygon);
void animation(Solution sol);

#endif
