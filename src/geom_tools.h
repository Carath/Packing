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
	Point *start, *end;
} Segment;

// Line of equation: ax + by + c = 0:
typedef struct
{
	double a, b, c;
} Line;

// TODO: inline short functions

/////////////////////////////////////////////
// Utilities:
/////////////////////////////////////////////

// Checks if x = y, short of EPSILON:
int epsilonEquality(double x, double y);


// Checks if x < y, short of EPSILON:
int epsilonStrInequality(double x, double y);

/////////////////////////////////////////////
// Points:
/////////////////////////////////////////////

void printPoint(const Point *point);

// Checks if the two given points are equal:
int pointEquality(const Point *A, const Point *B);

double determinant(double x1, double y1, double x2, double y2);

double scalarProduct(double x1, double y1, double x2, double y2);

// Euclidean distance squared.
double distance2(double x1, double y1, double x2, double y2);

// Euclidean distance between the Points A and B:
double distance(const Point *A, const Point *B);

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

// Returns true if the two lines intersect, and if so fills the point.
bool linesIntersection(const Line *line1, const Line *line2, Point *p);

// Checks if the given point is in the half plane defined by a line and another point:
int isPointInHalfPlanePoint(const Point *point_test, const Point *point_ref, const Line *line);

// Checks the position of a points versus a segment.
// Returns -2 if the point isn't on the segment's line, -1 if it is before the start,
// 1 if it is after the end, and 0 if the point is inside the segment.
int pointInsideSegment(const Point *A, const Segment *segment);

// Returns true if there is a non trivial intersection.
bool segmentsIntersection(const Segment *segment_1, const Segment *segment_2);

#endif
