#ifndef POLYGONS_H
#define POLYGONS_H

#include <stdbool.h>
#include "settings.h"
#include "geom_tools.h"
#include "rng32.h"

// typedef enum {POS = 1, NEG = -1} Direction;

typedef struct
{
	double xmin, xmax, ymin, ymax;
} Box;

typedef struct
{
	Polygon *polArray;
	const int n_polygons;
	double bigSquareSide;
	double error;
} Solution;

void initConstExpr(void);
double getRadius(void);
Polygon createPolygon(double xCenter, double yCenter);
void printPolygon(const Polygon *s);
void translation(Polygon *s, double xDelta, double yDelta);
void rotation(Polygon *s, double angle);
void mutation(rng32 *rng, Polygon *s);
Box findBoundary(const Polygon *polArray, int n_polygons);
double findBigPolygonSize(const Polygon *polArray, int n_polygons);
void findErrorRatio(const Polygon *polArray, int n_polygons, double *side, double *error);
double relative_error(double ref, double x);
bool checkConfiguration(const Polygon *polArray, int n_polygons);
bool intersects(const Polygon *pol1, const Polygon *pol2);

#endif
