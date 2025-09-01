#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "geom_tools.h"

/////////////////////////////////////////////
// Utilities:
/////////////////////////////////////////////

// Checks if x = y, short of EPSILON:
inline bool epsilonEquality(double x, double y)
{
	return fabs(x - y) < EPSILON;
}

// Checks if x < y, short of EPSILON:
inline bool epsilonStrInequality(double x, double y)
{
	return y - x > EPSILON;
}

// Checks if x <= y:
inline bool epsilonInequality(double x, double y)
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
bool pointEquality(const Point *A, const Point *B)
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
	return sqrt(distance2(A->x, A->y, B->x, B->y));
	// return hypot(B->x - A->x, B->y - A->y);
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

	p->x = determinant(b1, b2, c1, c2) / det;
	p->y = determinant(c1, c2, a1, a2) / det;
	return true;
}

// Checks if the given point is in the half plane defined by a line and another point:
bool isPointInHalfPlanePoint(const Point *point_test, const Point *point_ref, const Line *line)
{
	// assert(point_test && point_ref && line);
	const int sign = line->a * point_ref->x + line->b * point_ref->y + line->c >= 0. ? 1 : -1;
	return sign * (line->a * point_test->x + line->b * point_test->y + line->c) >= 0.;
	// const int sign = scalarProduct(line->a, line->b, point_ref->x, point_ref->y) + line->c >= 0. ? 1 : -1;
	// return sign * (scalarProduct(line->a, line->b, point_test->x, point_test->y) + line->c) >= 0.;
}

// Checks the position of a points versus a segment.
// Returns -2 if the point isn't on the segment's line, -1 if it is before the start,
// 1 if it is after the end, and 0 if the point is inside the segment.
bool pointInsideSegment(const Point *A, const Segment *segment)
{
	const Point *start = segment->start, *end = segment->end;
	const double dirTo_end_X = end->x - start->x;
	const double dirTo_end_Y = end->y - start->y;
	const double dirTo_A_X = A->x - start->x;
	const double dirTo_A_Y = A->y - start->y;

	if (epsilonEquality(dirTo_end_X, 0.) && epsilonEquality(dirTo_end_Y, 0.)) // start equals end.
		return (epsilonEquality(dirTo_A_X, 0.) && epsilonEquality(dirTo_A_Y, 0.));

	// Checking if A isn't anywhere on the line. Test is det(A-S, E-S) != 0
	if (!epsilonEquality(determinant(dirTo_A_X, dirTo_A_Y, dirTo_end_X, dirTo_end_Y), 0.))
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
bool segmentsNonTrivialIntersection(const Segment *segment_1, const Segment *segment_2)
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

/////////////////////////////////////////////

// Returns the squared area of the triangle ABC, using Heron's formula:
double area2(const Point *A, const Point *B, const Point *C)
{
	// assert(A && B && C);
	const double a = distance(A, B);
	const double b = distance(B, C);
	const double c = distance(C, A);
	const double s = (a + b + c) / 2.; // half perimeter
	return s*(s-a)*(s-b)*(s-c);
}

bool isInSquare(const Point *p, const Point squarePoints[N_SIDES], const Line squarelines[N_SIDES])
{
	// return isPointInHalfPlanePoint(p, squarePoints + 2, squarelines    )
	// 	&& isPointInHalfPlanePoint(p, squarePoints + 3, squarelines + 1)
	// 	&& isPointInHalfPlanePoint(p, squarePoints    , squarelines + 2)
	// 	&& isPointInHalfPlanePoint(p, squarePoints + 1, squarelines + 3);

	for (int i = 0; i < N_SIDES; ++i) {
		if (!isPointInHalfPlanePoint(p, squarePoints + (i+2)%N_SIDES, squarelines + i))
			return false;
	}
	return true;
}

bool isPointInArray(const Point *p, const Point *array, int length)
{
	for (int i = 0; i < length; ++i) {
		if (pointEquality(p, array + i))
			return true;
	}
	return false;
}

// Idea: compute non trivial intersections surface!
// To realize that, compute every intersection poitn of sides, keep them along with
// squares corners inside the intersection. Said intersection is the convex hull of those points,
// to compute it just divide the area in triangles.
double intersectionArea2(const Square *s1, const Square *s2)
{
	Line lines1[N_SIDES] = {0};
	Line lines2[N_SIDES] = {0};
	for (int i = 0; i < N_SIDES; ++i) {
		lines1[i] = lineFromPoints(s1->points + i, s1->points + (i+1)%N_SIDES);
		lines2[i] = lineFromPoints(s2->points + i, s2->points + (i+1)%N_SIDES);
	}

	// 2*N_SIDES should be the max non trivial intersection points:
	Point allIntersections[2*N_SIDES] = {0};
	int idx = 0;
	for (int i = 0; i < N_SIDES; ++i) {
		if (isInSquare(s1->points + i, s2->points, lines2)) // corners are accepted
			allIntersections[idx++] = s1->points[i];
		if (isInSquare(s2->points + i, s1->points, lines1)) // corners are accepted
			allIntersections[idx++] = s2->points[i];
	}
	for (int i = 0; i < N_SIDES; ++i) {
		for (int j = 0; j < N_SIDES; ++j) {
			Point p = {0};
			if (linesIntersection(lines1 + i, lines2 + j, &p) && !isPointInArray(&p, allIntersections, idx))
				allIntersections[idx++] = p; // adding the intersection which is not a corner.
		}
	}
	assert(idx <= 2*N_SIDES); // to be sure.

	// TODO: sort intersection points, split in triangles and sum the squared areas.
	return 0.;
}
