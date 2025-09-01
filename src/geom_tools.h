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

typedef struct
{
	double xA, yA, xB, yB, xC, yC, xD, yD;
	double xCenter, yCenter;
} Square; // square length = 1.

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
bool isPointInHalfPlanePoint(const Point *point_test, const Point *point_ref, const Line *line);

// Checks the position of a points versus a segment.
// Returns -2 if the point isn't on the segment's line, -1 if it is before the start,
// 1 if it is after the end, and 0 if the point is inside the segment.
bool pointInsideSegment(const Point *A, const Segment *segment);

/////////////////////////////////////////////

// Returns true if there is a non trivial intersection.
bool segmentsNonTrivialIntersection(const Segment *segment_1, const Segment *segment_2);

/////////////////////////////////////////////

// Returns the squared area of the triangle ABC, using Heron's formula:
double area2(const Point *A, const Point *B, const Point *C);

bool isInSquare(const Point *p, const Point squarePoints[4], const Line squarelines[4]);

bool isPointInArray(const Point *p, const Point *array, int length);

double intersectionArea2(const Square *s1, const Square *s2);

#endif
