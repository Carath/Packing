#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "geom_tools.h"

/////////////////////////////////////////////
// Utilities:
/////////////////////////////////////////////

// Checks if x = y, short of EPSILON:
inline int epsilonEquality(double x, double y)
{
	return fabs(x - y) < EPSILON;
}

// Checks if x < y, short of EPSILON:
inline int epsilonStrInequality(double x, double y)
{
	return y - x > EPSILON;
}

// Checks if x <= y:
inline int epsilonInequality(double x, double y)
{
	return x <= y;
}

/////////////////////////////////////////////
// Points:
/////////////////////////////////////////////

void printPoint(const Point *point)
{
	// assert(point);
	printf("Point: (%.2f, %.2f)\n", point->x, point->y);
}

// Checks if the two given points are equal:
int pointEquality(const Point *A, const Point *B)
{
	// assert(A && B);
	return epsilonEquality(A->x, B->x) && epsilonEquality(A->y, B->y);
}

inline double determinant(double x1, double y1, double x2, double y2)
{
	return x1 * y2 - x2 * y1;
}

inline double scalarProduct(double x1, double y1, double x2, double y2)
{
	return x1 * x2 + y1 * y2;
}

// Euclidean distance squared.
inline double distance2(double x1, double y1, double x2, double y2)
{
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	// return scalarProduct(x2 - x1, y2 - y1, x2 - x1, y2 - y1);
}

// Euclidean distance between the Points A and B:
double distance(const Point *A, const Point *B)
{
	// assert(A && B);
	const double delta_x = A->x - B->x, delta_y = A->y - B->y;
	return sqrt(delta_x * delta_x + delta_y * delta_y);
	// return hypot(delta_x, delta_y);
}

// inline double detFromPoints(const Point *A, const Point *B)
// {
// 	return A->x * B->y - A->y * B->x;
// 	// return determinant(A->x, A->y, B->x, B->y);
// }

/////////////////////////////////////////////
// Segments:
/////////////////////////////////////////////

void printSegment(const Segment *segment)
{
	// assert(segment && segment->start && segment->end);
	printf("Segment: start = (%.2f, %.2f), end = (%.2f, %.2f)\n",
		(segment->start)->x, (segment->start)->y, (segment->end)->x, (segment->end)->y);
}

inline double length(const Segment *segment)
{
	// assert(segment);
	return distance(segment->start, segment->end);
}

/////////////////////////////////////////////
// Lines:
/////////////////////////////////////////////

void printLine(const Line *line)
{
	// assert(line);
	printf("a = %.2f, b = %.2f, c = %.2f\n", line->a, line->b, line->c);
}

Line lineFromPoints(const Point *A, const Point *B)
{
	// assert(A && B);
	return (Line) {B->y - A->y, A->x - B->x, determinant(B->x, B->y, A->x, A->y)};
}

/////////////////////////////////////////////
// More 'advanced' geometric utilities:
/////////////////////////////////////////////

// Returns the intersection of the two lines if it exists, NULL otherwise:
bool linesIntersection(const Line *line1, const Line *line2, Point *p)
{
	// assert(line1 && line2);
	const double a1 = line1->a, b1 = line1->b, c1 = line1->c;
	const double a2 = line2->a, b2 = line2->b, c2 = line2->c;
	const double det = determinant(a1, b1, a2, b2);

	if (epsilonEquality(det, 0.)) {
		// printf("Lines don't intersect, or are equal.\n");
		return false;
	}

	p->x = (b1 * c2 - b2 * c1) / det;
	p->y = (a2 * c1 - a1 * c2) / det;
	return true;
}

// Checks if the given point is in the half plane defined by a line and another point:
int isPointInHalfPlanePoint(const Point *point_test, const Point *point_ref, const Line *line)
{
	// assert(point_test && point_ref && line);
	const int sign = line->a * point_ref->x + line->b * point_ref->y + line->c >= 0. ? 1 : -1;
	return sign * (line->a * point_test->x + line->b * point_test->y + line->c) >= 0.;
}

// Checks the position of a points versus a segment.
// Returns -2 if the point isn't on the segment's line, -1 if it is before the start,
// 1 if it is after the end, and 0 if the point is inside the segment.
int pointInsideSegment(const Point *A, const Segment *segment)
{
	const Point *start = segment->start, *end = segment->end;
	const double dirTo_end_X = end->x - start->x;
	const double dirTo_end_Y = end->y - start->y;
	const double dirTo_A_X = A->x - start->x;
	const double dirTo_A_Y = A->y - start->y;

	// if (epsilonEquality(dirTo_end_X, 0.) && epsilonEquality(dirTo_end_Y, 0.)) // start equals end.
	// 	return (epsilonEquality(dirTo_A_X, 0.) && epsilonEquality(dirTo_A_Y, 0.));

	// const double ratioX = dirTo_A_X / dirTo_end_X;
	// const double ratioY = dirTo_A_Y / dirTo_end_Y;

	// if (!epsilonEquality(ratioX, ratioY)) { // A isn't anywhere on the line.
	// 	// printf("here! %.6f vs %.6f, %d\n", ratioX, ratioY, dirTo_end_X == 0. || dirTo_end_Y == 0.);
	// 	// printPoint(A);
	// 	// printSegment(segment);
	// 	return -2;
	// }
	// if (epsilonInequality(ratioX, 0.)) // A is before the start of the segment.
	// 	return -1;
	// if (epsilonInequality(1., ratioX)) // A is after the end of the segment.
	// 	return 1;
	// else
	// 	return 0; // A is inside the segment.
	// // This breaks with perfectly horizontal or vertical segments...


	if (epsilonEquality(dirTo_end_X, 0.) && epsilonEquality(dirTo_end_Y, 0.)) // start equals end.
		return (epsilonEquality(dirTo_A_X, 0.) && epsilonEquality(dirTo_A_Y, 0.));

	// Checking if A isn't anywhere on the line. Test is det(A-S, E-S) != 0
	if (!epsilonEquality(dirTo_A_X * dirTo_end_Y - dirTo_A_Y * dirTo_end_X, 0.))
		return -2;

	if ((epsilonEquality(dirTo_end_X, 0.) || dirTo_A_X / dirTo_end_X <= 0.) &&
		(epsilonEquality(dirTo_end_Y, 0.) || dirTo_A_Y / dirTo_end_Y <= 0.)) // A is before the start of the segment.
		return -1;
	if ((epsilonEquality(dirTo_end_X, 0.) || dirTo_A_X / dirTo_end_X >= 1.) &&
		(epsilonEquality(dirTo_end_Y, 0.) || dirTo_A_Y / dirTo_end_Y >= 1.)) // A is after the start of the segment.
		return 1;
	else
		return 0; // A is inside the segment.
}

/////////////////////////////////////////////

// Returns true if there is a non trivial intersection.
bool segmentsIntersection(const Segment *segment_1, const Segment *segment_2)
{
	Point p = {0};
	const Line l1 = lineFromPoints(segment_1->start, segment_1->end);
	const Line l2 = lineFromPoints(segment_2->start, segment_2->end);
	const bool res = linesIntersection(&l1, &l2, &p);

	// printPoint(p);
	// Point *A = segment_1->start, *B = segment_1->end;
	// Point *C = segment_2->start, *D = segment_2->end;
	// Point BmA = {B->x - A->x, B->y - A->y}, DmC = {D->x - C->x, D->y - D->x};
	// printf("det: %.4f\n", detFromPoints(&BmA, &DmC));
	// const double xx = (detFromPoints(C, D) - detFromPoints(A, &DmC)) / detFromPoints(&BmA, &DmC);
	// printf("%.4f\n", xx);

	return res && pointInsideSegment(&p, segment_1) == 0 && pointInsideSegment(&p, segment_2) == 0;
}
// TODO: optimize this function?
