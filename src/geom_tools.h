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

// This can be freed with a regular 'free()'.
Point* createPoint(double x, double y);

void printPoint(const Point *point);

// Checks if the two given points are equal:
int pointEquality(const Point *A, const Point *B);

// Euclidean distance between the Points A and B:
double distance(const Point *A, const Point *B);

// Euclidean distance squared.
double distance2(double x1, double y1, double x2, double y2);

double determinant(double x1, double y1, double x2, double y2);

/////////////////////////////////////////////
// Segments:
/////////////////////////////////////////////

// A segment may be freed with 'free()', but references to its tips must be kept!
// Otherwise, memory will leak. Use 'freeSegmentCompletely()' to free the tips along the segment.
Segment* createSegment(Point *start, Point *end);

void printSegment(const Segment *segment);

// Careful, this frees the segment tips too!
void freeSegmentCompletely(Segment *segment);

double length(const Segment *segment);

// Bounding a line:
Segment* SegmentFromLine(const Line *line);

/////////////////////////////////////////////
// Lines:
/////////////////////////////////////////////

// This can be freed with a regular 'free()'.
Line* createLine(double a, double b, double c);

void printLine(const Line *line);

Line* lineFromPoints(const Point *A, const Point *B);

/////////////////////////////////////////////
// More 'advanced' geometric utilities:
/////////////////////////////////////////////

// Returns the intersection of the two lines if it exists, NULL otherwise:
Point* linesIntersection(const Line *line1, const Line *line2);

// Checks if the given point is in the half plane defined by a line and another point:
int isPointInHalfPlanePoint(const Point *point_test, const Point *point_ref, const Line *line);

// Checks the position of a points versus a segment.
// Returns -2 if the point isn't on the segment's line, -1 if it is before the start,
// 1 if it is after the end, and 0 if the point is inside the segment.
int pointInsideSegment(const Point *A, const Segment *segment);

// Returns true if there is a non trivial intersection.
bool segmentsIntersection(const Segment *segments_1, const Segment *segments_2);

#endif
