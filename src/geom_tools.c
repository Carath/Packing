#include <stdio.h>
#include <stdlib.h>
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
    return x + EPSILON < y;
}

/////////////////////////////////////////////
// Points:
/////////////////////////////////////////////

// This can be freed with a regular 'free()'.
Point* createPoint(double x, double y)
{
	Point *point = (Point*) calloc(1, sizeof(Point));
	point->X = x;
	point->Y = y;
	return point;
}

void printPoint(const Point *point)
{
	if (!point)
		printf("NULL Point.\n");
	else
		printf("Point: (%.2f, %.2f)\n", point->X, point->Y);
}

// Checks if the two given points are equal:
int pointEquality(const Point *A, const Point *B)
{
	if (!A || !B) {
		printf("Cannot test the equality of NULL points.\n");
		return 0;
	}
	return epsilonEquality(A->X, B->X) && epsilonEquality(A->Y, B->Y);
}

// Euclidean distance between the Points A and B:
double distance(const Point *A, const Point *B)
{
	if (!A || !B) {
		printf("Cannot find a distance between NULL points.\n");
		return 0;
	}

	const double delta_x = A->X - B->X, delta_y = A->Y - B->Y;
	return sqrt(delta_x * delta_x + delta_y * delta_y);
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
	segment->Start = start;
	segment->End = end;
	return segment;
}

void printSegment(const Segment *segment)
{
	if (!segment)
		printf("NULL Segment.\n");
	else if (!(segment->Start) || !(segment->End))
		printf("Segment's start or end is NULL.\n");
	else
		printf("Segment: start = (%.2f, %.2f), end = (%.2f, %.2f)\n",
			(segment->Start)->X, (segment->Start)->Y, (segment->End)->X, (segment->End)->Y);
}

// Careful, this frees the segment tips too!
void freeSegmentCompletely(Segment *segment)
{
	free(segment->Start);
	free(segment->End);
	free(segment);
}

double length(const Segment *segment)
{
	if (!segment) {
		printf("NULL Segment.\n");
		return 0;
	}

	return distance(segment->Start, segment->End);
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
	if (epsilonEquality(b, 0)) { // Vertical line.
		if (epsilonEquality(a, 0)) {
			printf("Invalid line equation.\n");
			return NULL;
		}

		start = createPoint(- c / a, 0);
		end = createPoint(- c / a, WINDOW_HEIGHT);
	}
	else {
		start = createPoint(0, - c / b);
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
	if (epsilonEquality(a, 0) && epsilonEquality(b, 0)) {
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
		printf("Cannot find a line going through NULL points.\n");
		return NULL;
	}

	const double a = B->Y - A->Y;
	const double b = A->X - B->X;
	const double c = B->X * A->Y - A->X * B->Y;
	return createLine(a, b, c);
}

/////////////////////////////////////////////
// More 'advanced' geometric utilities:
/////////////////////////////////////////////

// Returns the intersection of the two lines if it exists, NULL otherwise:
Point* linesIntersection(const Line *line1, const Line *line2)
{
	const double a1 = line1->a, b1 = line1->b, c1 = line1->c;
	const double a2 = line2->a, b2 = line2->b, c2 = line2->c;
	const double det = a1 * b2 - b1 * a2;

	if (epsilonEquality(det, 0)) {
		printf("Lines don't intersect, or are equal.\n");
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

	const int sign = line->a * point_ref->X + line->b * point_ref->Y + line->c >= 0 ? 1 : -1;
	return sign * (line->a * point_test->X + line->b * point_test->Y + line->c) >= 0;
}

// Checks the position of a points versus a segment.
// Returns -2 if the point isn't on the segment's line, -1 if it is before the start,
// 1 if it is after the end, and 0 if the point is on the segment.
int posPointSegment(const Point *A, const Segment *segment)
{
	const Point *start = segment->Start;
	const Point *end = segment->End;
	const double dirTo_End_X = end->X - start->X;
	const double dirTo_End_Y = end->Y - start->Y;
	const double dirTo_A_X = A->X - start->X;
	const double dirTo_A_Y = A->Y - start->Y;

	if (epsilonEquality(dirTo_End_X, 0) && epsilonEquality(dirTo_End_Y, 0)) // start equals end.
		return (epsilonEquality(dirTo_A_X, 0) && epsilonEquality(dirTo_A_Y, 0));

	const double ratioX = dirTo_A_X / dirTo_End_X;
	const double ratioY = dirTo_A_Y / dirTo_End_Y;

	if (!epsilonEquality(ratioX, ratioY)) // A isn't anywhere on the line.
		return -2;
	if (epsilonStrInequality(ratioX, 0)) // A is before the start of the segment.
		return -1;
	if (epsilonStrInequality(1, ratioX)) // A is after the end of the segment.
		return 1;
	else
		return 0; // A is on the segment.
}
