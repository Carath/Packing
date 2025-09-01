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

// This can be freed with a regular 'free()'.
Point* createPoint(double x, double y)
{
	Point *point = (Point*) calloc(1, sizeof(Point));
	point->x = x;
	point->y = y;
	return point;
}

void printPoint(const Point *point)
{
	if (!point)
		printf("NULL Point.\n");
	else
		printf("Point: (%.2f, %.2f)\n", point->x, point->y);
}

// Checks if the two given points are equal:
int pointEquality(const Point *A, const Point *B)
{
	if (!A || !B) {
		printf("Cannot test the equality of NULL points.\n");
		return 0;
	}
	return epsilonEquality(A->x, B->x) && epsilonEquality(A->y, B->y);
}

// Euclidean distance between the Points A and B:
double distance(const Point *A, const Point *B)
{
	if (!A || !B) {
		printf("Cannot find a distance between NULL points.\n");
		return 0;
	}

	const double delta_x = A->x - B->x, delta_y = A->y - B->y;
	return sqrt(delta_x * delta_x + delta_y * delta_y);
}

// Euclidean distance squared.
inline double distance2(double x1, double y1, double x2, double y2)
{
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

inline double determinant(double x1, double y1, double x2, double y2)
{
	return x1 * y2 - x2 * y1;
}

/////////////////////////////////////////////
// Segments:
/////////////////////////////////////////////

// A segment may be freed with 'free()', but references to its tips must be kept!
// Otherwise, memory will leak. Use 'freeSegmentCompletely()' to free the tips along the segment.
Segment* createSegment(Point *start, Point *end)
{
	if (!start || !end) {
		printf("Segment's tips must not be NULL.\n");
		return NULL;
	}

	Segment *segment = (Segment*) calloc(1, sizeof(Segment));
	segment->start = start;
	segment->end = end;
	return segment;
}

void printSegment(const Segment *segment)
{
	if (!segment)
		printf("NULL Segment.\n");
	else if (!(segment->start) || !(segment->end))
		printf("Segment's start or end is NULL.\n");
	else
		printf("Segment: start = (%.2f, %.2f), end = (%.2f, %.2f)\n",
			(segment->start)->x, (segment->start)->y, (segment->end)->x, (segment->end)->y);
}

// Careful, this frees the segment tips too!
void freeSegmentCompletely(Segment *segment)
{
	free(segment->start);
	free(segment->end);
	free(segment);
}

double length(const Segment *segment)
{
	if (!segment) {
		printf("NULL Segment.\n");
		return 0.;
	}

	return distance(segment->start, segment->end);
}

// Bounding a line:
Segment* SegmentFromLine(const Line *line)
{
	if (!line) {
		printf("NULL line.\n");
		return NULL;
	}

	const double a = line->a, b = line->b, c = line->c;
	Point *start, *end;
	if (epsilonEquality(b, 0.)) { // Vertical line.
		if (epsilonEquality(a, 0.)) {
			printf("Invalid line equation.\n");
			return NULL;
		}

		start = createPoint(- c / a, 0.);
		end = createPoint(- c / a, WINDOW_HEIGHT);
	}
	else {
		start = createPoint(0., - c / b);
		end = createPoint(WINDOW_WIDTH, - (a * WINDOW_WIDTH + c) / b);
	}

	// Careful, start and end are not freed!
	return createSegment(start, end);
}

/////////////////////////////////////////////
// Lines:
/////////////////////////////////////////////

// This can be freed with a regular 'free()'.
Line* createLine(double a, double b, double c)
{
	if (epsilonEquality(a, 0.) && epsilonEquality(b, 0.)) {
		printf("Invalid line equation.\n");
		return NULL;
	}

	Line *line = (Line*) calloc(1, sizeof(Line));
	line->a = a;
	line->b = b;
	line->c = c;
	return line;
}

void printLine(const Line *line)
{
	if (!line) {
		printf("NULL line.\n");
		return;
	}
	printf("a = %.2f, b = %.2f, c = %.2f\n", line->a, line->b, line->c);
}

Line* lineFromPoints(const Point *A, const Point *B)
{
	if (!A || !B) {
		// printf("Cannot find a line going through NULL points.\n");
		return NULL;
	}

	const double a = B->y - A->y;
	const double b = A->x - B->x;
	const double c = B->x * A->y - A->x * B->y;
	return createLine(a, b, c);
}

/////////////////////////////////////////////
// More 'advanced' geometric utilities:
/////////////////////////////////////////////

// Returns the intersection of the two lines if it exists, NULL otherwise:
Point* linesIntersection(const Line *line1, const Line *line2)
{
	if (!line1 || !line2)
		return NULL;

	const double a1 = line1->a, b1 = line1->b, c1 = line1->c;
	const double a2 = line2->a, b2 = line2->b, c2 = line2->c;
	const double det = determinant(a1, b1, a2, b2);

	if (epsilonEquality(det, 0.)) {
		// printf("Lines don't intersect, or are equal.\n");
		return NULL;
	}

	const double interX = (b1 * c2 - b2 * c1) / det;
	const double interY = (a2 * c1 - a1 * c2) / det;
	return createPoint(interX, interY);
}

// Checks if the given point is in the half plane defined by a line and another point:
int isPointInHalfPlanePoint(const Point *point_test, const Point *point_ref, const Line *line)
{
	if (!point_test || !point_ref || !line) {
		printf("Cannot do a domain check, invalid arguments.\n");
		return 0;
	}

	const int sign = line->a * point_ref->x + line->b * point_ref->y + line->c >= 0. ? 1 : -1;
	return sign * (line->a * point_test->x + line->b * point_test->y + line->c) >= 0.;
}

// Checks the position of a points versus a segment.
// Returns -2 if the point isn't on the segment's line, -1 if it is before the start,
// 1 if it is after the end, and 0 if the point is inside the segment.
int pointInsideSegment(const Point *A, const Segment *segment)
{
	const Point *start = segment->start;
	const Point *end   = segment->end;
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
bool segmentsIntersection(const Segment *segments_1, const Segment *segments_2)
{
	Line *l1 = lineFromPoints(segments_1->start, segments_1->end);
	Line *l2 = lineFromPoints(segments_2->start, segments_2->end);
	Point *p = linesIntersection(l1, l2);
	const bool result = p && pointInsideSegment(p, segments_1) == 0 && pointInsideSegment(p, segments_2) == 0;
	free(p);
	free(l1);
	free(l2);
	return result;
}
