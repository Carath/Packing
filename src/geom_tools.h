#ifndef GEOM_TOOLS_H
#define GEOM_TOOLS_H

#include <stdbool.h>
#include "settings.h"

typedef struct
{
	double x, y;
} Point;

typedef struct
{
	const Point *start, *end;
} Segment;

typedef struct
{
	double a, b, c;
} Line;
// Line of equation: ax + by + c = 0.
// N.B: a Line could also be represented by a reference Point, and a vector
// encoded by another Point. Maybe this yields to less computations...

typedef struct
{
	Point points[N_SIDES];
	Point center; // put this as the array last spot?
} Polygon; // polygon area = 1.

// TODO: inline short functions

/////////////////////////////////////////////
// Utilities:
/////////////////////////////////////////////

// Checks if x = y, short of EPSILON:
bool epsilonEquality(double x, double y);


// Checks if x < y, short of EPSILON:
bool epsilonStrInequality(double x, double y);

/////////////////////////////////////////////
// Points:
/////////////////////////////////////////////

void printPoint(const Point *point);

// Checks if the two given points are equal:
bool pointEquality(const Point *A, const Point *B);

// Returns the AB vector, stored as a Point.
Point vector(const Point *A, const Point *B);

double determinant(double x1, double y1, double x2, double y2);

double scalarProduct(double x1, double y1, double x2, double y2);

// Euclidean distance squared.
double distance2(const Point *A, const Point *B);

// Euclidean distance between the Points A and B:
double distance(const Point *A, const Point *B);

double detFromPoints(const Point *A, const Point *B);

// 'co' and 'si' are the precomputed cosinus and sinus of the desired rotation's angle.
Point rotatePoint(const Point center, Point A, double co, double si);

/////////////////////////////////////////////
// Segments:
/////////////////////////////////////////////

void printSegment(const Segment *segment);

double length(const Segment *segment);

/////////////////////////////////////////////
// Lines:
/////////////////////////////////////////////

void printLine(const Line *line);

Line lineFromPoints(const Point *A, const Point *B);

/////////////////////////////////////////////
// More 'advanced' geometric utilities:
/////////////////////////////////////////////

// Returns true if the two lines intersect in a single point, and if so fills it.
bool linesIntersection(const Line *line1, const Line *line2, Point *p);

// Checks if the given point is in the half plane defined by a line and a reference
// point inside said half plane. If strict is true, the point must not be on the line.
bool isPointInHalfPlane(const Point *point_test, const Point *point_ref, const Line *line, bool strict);

// Checks if the given point is in the segment. If strict
// is true, the point must not be on the segment bounds.
bool isPointInSegment(const Point *A, const Segment *segment, bool strict);

/////////////////////////////////////////////

// Returns true if the segments intersect in a point, and if so fills it.
// If strict is true, the intersection must not be on any segment bounds.
bool segmentsIntersection(const Segment *segment1, const Segment *segment2, bool strict, Point *p);

/////////////////////////////////////////////

// Returns the squared area of the triangle ABC, using Heron's formula:
double area2(const Point *A, const Point *B, const Point *C);

bool isInPolygon(const Point *p, const Point polygonPoints[N_SIDES], const Line polygonlines[N_SIDES]);

bool isPointInArray(const Point *p, const Point *array, int length);

double intersectionArea(const Polygon *pol1, const Polygon *pol2);

#endif
