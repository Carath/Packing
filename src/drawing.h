#ifndef DRAWING_H
#define DRAWING_H

#include "SDLA.h"
#include "polygons.h"
#include "geom_tools.h"

Point projection(Point p, Point offset, double scale);
void computeProjection(Solution sol, Point *offset, double *scale);
void drawPoint(const Point *point, const SDL_Color *color);
void drawSegment(const Segment *segment, const SDL_Color *color);
void drawPolygonalChain(const Point *points, int length, bool closed);
void drawPolygon(const Polygon *polygon);
void animation(Solution sol);

#endif
