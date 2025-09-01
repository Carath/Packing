#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
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

// Checks if x <= y, short of EPSILON.
// Note that this fuzzy inequality does not describe an actual order.
inline bool epsilonInequality(double x, double y)
{
	return y - x >= -EPSILON;
}

/////////////////////////////////////////////
// Points:
/////////////////////////////////////////////

void printPoint(const Point *point)
{
	// assert(point);
	printf("Point: (%g, %g)\n", point->x, point->y);
}

// Checks if the two given points are equal:
inline bool pointEquality(const Point *A, const Point *B)
{
	// assert(A && B);
	return epsilonEquality(A->x, B->x) && epsilonEquality(A->y, B->y);
}

// Returns the AB vector, stored as a Point.
inline Point vector(const Point *A, const Point *B)
{
	return (Point) {B->x - A->x, B->y - A->y};
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
inline double distance2(const Point *A, const Point *B)
{
	// assert(A && B);
	const Point AB = vector(A, B);
	return AB.x * AB.x + AB.y * AB.y;
}

// Euclidean distance between the Points A and B:
inline double distance(const Point *A, const Point *B)
{
	return sqrt(distance2(A, B));
	// return hypot(B->x - A->x, B->y - A->y); // a bit slower but more precise.
}

inline double detFromPoints(const Point *A, const Point *B)
{
	return A->x * B->y - A->y * B->x;
}

// 'co' and 'si' are the precomputed cosinus and sinus of the desired rotation's angle.
Point rotatePoint(const Point center, Point A, double co, double si)
{
	const Point CA = vector(&center, &A);
	A.x = co * CA.x - si * CA.y + center.x;
	A.y = si * CA.x + co * CA.y + center.y;
	return A;
}

inline void swapPoints(Point *A, Point *B)
{
	const Point temp = *A;
	*A = *B; *B = temp;
}

Point getCenter(const Point *points, int length)
{
	Point center = {0};
	for (int i = 0; i < length; ++i) {
		center.x += points[i].x;
		center.y += points[i].y;
	}
	center.x /= length;
	center.y /= length;
	return center;
}

bool isPointInArray(const Point *p, const Point *array, int length)
{
	for (int i = 0; i < length; ++i) {
		if (pointEquality(p, array + i))
			return true;
	}
	return false;
}

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
	return (Line) {B->y - A->y, A->x - B->x, detFromPoints(B, A)};
	// const Point AB = vector(A, B);
	// return (Line) {-AB.y, AB.x, detFromPoints(A, B)};
}

/////////////////////////////////////////////
// More 'advanced' geometric utilities:
/////////////////////////////////////////////

// Returns true if the two lines intersect in a single point, and if so fills it.
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

// Checks if the given point is in the half plane defined by a line and a reference
// point inside said half plane. If strict is true, the point must not be on the line.
bool isPointInHalfPlane(const Point *point_test, const Point *point_ref, const Line *line, bool strict)
{
	// assert(point_test && point_ref && line);
	const int sign = line->a * point_ref->x + line->b * point_ref->y + line->c >= 0. ? 1 : -1;
	// return sign * (line->a * point_test->x + line->b * point_test->y + line->c) >= (strict ? EPSILON : 0.); // unstable
	return sign * (line->a * point_test->x + line->b * point_test->y + line->c) >= (strict ? EPSILON : -EPSILON); // stable
}

// Checks if the given point is in the segment. If strict
// is true, the point must not be on the segment bounds.
bool isPointInSegment(const Point *A, const Segment *segment, bool strict)
{
	const Point *start = segment->start, *end = segment->end;
	const Point vectSA = vector(start, A);
	const Point vectSE = vector(start, end);

	if (pointEquality(start, end))
		return !strict && pointEquality(start, A);

	// Checking if A isn't anywhere on the line. Test is det(A-S, E-S) != 0
	if (!epsilonEquality(detFromPoints(&vectSA, &vectSE), 0.))
		return 0;

	const double ratioX = vectSA.x / vectSE.x;
	const double ratioY = vectSA.y / vectSE.y;
	const double tmin = strict ?    EPSILON : 0.;
	const double tmax = strict ? 1.-EPSILON : 1.;
	return (epsilonEquality(vectSE.x, 0.) || (tmin <= ratioX && ratioX <= tmax))
		&& (epsilonEquality(vectSE.y, 0.) || (tmin <= ratioY && ratioY <= tmax));
}

/////////////////////////////////////////////

// Returns true if the segments intersect in a single point, and if so fills it.
// If strict is true, the intersection must not be on any segment bounds.
bool segmentsIntersection(const Segment *segment1, const Segment *segment2, bool strict, Point *p)
{
	const Line l1 = lineFromPoints(segment1->start, segment1->end);
	const Line l2 = lineFromPoints(segment2->start, segment2->end);
	const bool res = linesIntersection(&l1, &l2, p);
	return res && isPointInSegment(p, segment1, strict) && isPointInSegment(p, segment2, strict);

	// TODO: optimize more this function?
	// printPoint(p);
	// Point *A = segment1->start, *B = segment1->end;
	// Point *C = segment2->start, *D = segment2->end;
	// Point BmA = {B->x - A->x, B->y - A->y}, DmC = {D->x - C->x, D->y - D->x};
	// printf("det: %.4f\n", detFromPoints(&BmA, &DmC));
	// const double xx = (detFromPoints(C, D) - detFromPoints(A, &DmC)) / detFromPoints(&BmA, &DmC);
	// printf("%.4f\n", xx);
}

bool isPointInPolygon(const Point *p, const Point polygonPoints[N_SIDES], const Line polygonlines[N_SIDES])
{
	for (int i = 0; i < N_SIDES; ++i) {
		if (!isPointInHalfPlane(p, polygonPoints + (i+2) % N_SIDES, polygonlines + i, false))
			return false;
	}
	return true;
}

/////////////////////////////////////////////

// Idea: compute non-zero intersections area!
// To realize that, compute every intersection point of sides, keep them along with polygons
// corners inside the intersection. Said intersection is the convex hull of those points,
// to compute it just divide the area in triangles.
// Note: use the fact that the intersection will be a convex polygon.

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

// Origin utilities for findIntersection():

inline Origin newOrigin(int i, int j)
{
	return (Origin) {i, j};
}

inline bool originEquality(Origin o1, Origin o2)
{
	return o1.i == o2.i && o1.j == o2.j;
}

// next() and prev() are inverse of each other when k != NO:

// This should not be called on NO.
inline int next(int k)
{
	// assert(0 <= k && k < N_SIDES); // detecting NO
	return (k+1) % N_SIDES;
}

// This should not be called on NO.
inline int prev(int k)
{
	// assert(0 <= k && k < N_SIDES); // detecting NO
	return (k-1+N_SIDES) % N_SIDES;
}

// Note: all versions originsLinked() are bugged as of now.
// Indeed, prev() and next() can be called on NO, which should never happen.

// Natural version:
bool originsLinked(Origin o1, Origin o2)
{
	if (o1.i == NO)
		return originEquality(o2, newOrigin(NO, prev(o1.j)))
			|| originEquality(o2, newOrigin(NO, next(o1.j)))
			|| (o2.i != NO && o1.j == o2.j) || (o2.i != NO && prev(o1.j) == o2.j);

	if (o1.j == NO)
		return originEquality(o2, newOrigin(prev(o1.i), NO))
			|| originEquality(o2, newOrigin(next(o1.i), NO))
			|| (o1.i == o2.i && o2.j != NO) || (prev(o1.i) == o2.i && o2.j != NO);

	return originEquality(o2, newOrigin(o1.i, NO)) || originEquality(o2, newOrigin(next(o1.i), NO))
		|| originEquality(o2, newOrigin(NO, o1.j)) || originEquality(o2, newOrigin(NO, next(o1.j)))
		|| (o1.i == o2.i && o2.j != NO) || (o2.i != NO && o1.j == o2.j);
}

// // Factorized version, sadly isn't faster:
// bool originsLinked(Origin o1, Origin o2)
// {
// 	const bool testi = o2.i == NO ? o2.j == next(o1.j) : o2.j == o1.j;
// 	const bool testj = o2.j == NO ? o2.i == next(o1.i) : o2.i == o1.i;
// 	if (o1.i == NO)
// 		return o2.j == prev(o1.j) || testi;
// 	if (o1.j == NO)
// 		return o2.i == prev(o1.i) || testj;
// 	return o2.j == o1.j || o2.i == o1.i || testi || testj;
// }

// // WRONG checksum.
// bool originsLinked(Origin o1, Origin o2)
// {
// 	const bool testi = o2.i == NO ? o2.j == next(o1.j) : o2.j == o1.j;
// 	const bool testj = o2.j == NO ? o2.i == next(o1.i) : o2.i == o1.i;
// 	if (o1.i == NO)
// 		return o1.j == next(o2.j) || testi;
// 	if (o1.j == NO)
// 		return o1.i == next(o2.i) || testj;
// 	return o2.j == o1.j || o2.i == o1.i || testi || testj;
// }

// // // WRONG checksum.
// bool originsLinked(Origin o1, Origin o2)
// {
// 	const int a = o2.i == NO;
// 	const int A = o2.j == NO;
// 	const int b = o2.i == o1.i;
// 	const int B = o2.j == o1.j;
// 	const int c = o2.i == next(o1.i);
// 	const int C = o2.j == next(o1.j);

// 	// const int d = o1.i == next(o2.i);
// 	// const int D = o1.j == next(o2.j);
// 	const int d = o2.i == prev(o1.i);
// 	const int D = o2.j == prev(o1.j);

// 	return ((a & (C | D)) | (~a & A & (c | d)) | (~a & ~A & (b | B))) & 1;
// }

int findIntersection(const Polygon *pol1, const Polygon *pol2, Point allIntersections[2*N_SIDES])
{
	Line lines1[N_SIDES] = {0};
	Line lines2[N_SIDES] = {0};
	for (int i = 0; i < N_SIDES; ++i) {
		lines1[i] = lineFromPoints(pol1->points + i, pol1->points + (i+1) % N_SIDES);
		lines2[i] = lineFromPoints(pol2->points + i, pol2->points + (i+1) % N_SIDES);
	}

	Segment segment1[N_SIDES] = {0};
	Segment segment2[N_SIDES] = {0};
	for (int i = 0; i < N_SIDES; ++i) {
		segment1[i] = (Segment) {pol1->points + i, pol1->points + (i+1) % N_SIDES};
		segment2[i] = (Segment) {pol2->points + i, pol2->points + (i+1) % N_SIDES};
	}

	////////////////

	// N.B: isPointInPolygon does not check if it is inside the interior. But that is actually ok.

	// 2*N_SIDES should be the max number of points of any intersection:
	Origin origins[2*N_SIDES] = {0};
	int idx = 0;

	// Checking for corners in the intersection. First polygon first:
	for (int i = 0; i < N_SIDES; ++i) {
		if (isPointInPolygon(pol1->points + i, pol2->points, lines2)) { // corners are accepted
			allIntersections[idx] = pol1->points[i];
			origins[idx] = (Origin) {i, NO};
			++idx;
		}
	}

	// Checking for corners in the intersection. Second polygon:
	for (int i = 0; i < N_SIDES; ++i) {
		// if (isPointInPolygon(pol2->points + i, pol1->points, lines1)) { // corners are accepted. Check redondancies here?
		if (isPointInPolygon(pol2->points + i, pol1->points, lines1) && !isPointInArray(pol2->points + i, allIntersections, idx)) {
			allIntersections[idx] = pol2->points[i];
			origins[idx] = (Origin) {NO, i};
			++idx;
		}
	}

	// Checking for segments intersections:
	for (int i = 0; i < N_SIDES; ++i) {
		for (int j = 0; j < N_SIDES; ++j) {
			Point p = {0};
			// if (segmentsIntersection(segment1 + i, segment2 + j, false, &p) && !isPointInArray(&p, allIntersections, idx))
			if (segmentsIntersection(segment1 + i, segment2 + j, true, &p)) {
				allIntersections[idx] = p; // adding the intersection which is not a corner.
				origins[idx] = (Origin) {i, j}; // i->(i+1)%N, idem for j.
				++idx;
			}
		}
	}

	////////////////

	if (idx > 3) {
		for (int i = 0; i < idx-1; ++i) {
			int j = i+1;
			while (j < idx && !originsLinked(origins[i], origins[j])) {
				++j;
			}
			if (!(j < idx)) {
				printf("ERROR!\n");
				printf("idx: %d\n", idx);
				printf("i=%d, j=%d\n", i, j);
				for (int k = 0; k < idx; ++k)
					printf("origins[%d]: (%d, %d)\n", k, origins[k].i, origins[k].j);
				exit(1);
			}
			// printf("Swapping %d and %d\n", i+1, j);
			swapPoints(allIntersections + i+1, allIntersections + j);
			const Origin temp = origins[i+1];
			origins[i+1] = origins[j];
			origins[j] = temp;
		}
	}

	return idx;
}

#include "drawing.h" // for debugging

double intersectionArea(const Polygon *pol1, const Polygon *pol2)
{
	Point allIntersections[2*N_SIDES] = {0};
	const int idx = findIntersection(pol1, pol2, allIntersections);

	////////////////
	// Method 1:
	const Point center = getCenter(allIntersections, idx);
	double totalArea1 = 0.;
	for (int i = 0; i < idx; ++i)
		totalArea1 += sqrt(area2(&center, allIntersections + i, allIntersections + (i+1) % idx));
	// return totalArea;

	////////////////
	// Method 2:
	double totalArea2 = 0.;
	for (int i = 1; i < idx-1; ++i) // idx-2 triangles
		totalArea2 += sqrt(area2(allIntersections, allIntersections + i, allIntersections + (i+1)));
	// return totalArea;

	////////////////
	// Sum the squared areas instead for speed?

	// if (!res || !epsilonEquality(totalArea1, totalArea2)) {
	if (!epsilonEquality(totalArea1, totalArea2)) {
		printf("\nIncompatible areas: %g vs %g\n", totalArea1, totalArea2);
		printf("idx: %d\n", idx);
		printf("intersection points:\n");
		for (int i = 0; i < idx; ++i) {
			printPoint(allIntersections + i);
		}

		// printf("\npol1:\n");
		// for (int i = 0; i < N_SIDES; ++i) {
		// 	printPoint(pol1->points + i);
		// }

		// printf("\npol2:\n");
		// for (int i = 0; i < N_SIDES; ++i) {
		// 	printPoint(pol2->points + i);
		// }

		// printf("%d\n", allIntersections[0].x == allIntersections[2].x);
		// printf("%d\n", allIntersections[0].y == allIntersections[2].y);
		// printf("%d\n", pointEquality(allIntersections, allIntersections + 2));
		// printf("%g, %g\n", allIntersections[0].x - allIntersections[2].x, allIntersections[0].y - allIntersections[2].y);

		// Polygon polArray[1];
		// memcpy(polArray[0].points, allIntersections, idx * sizeof(Point));
		// const Solution sol = {polArray, 1, -1., -1.};

		// // Polygon polArray[2] = {*pol1, *pol2};
		// // const Solution sol = {polArray, 2, -1., -1.};

		// animation(sol);

		exit(1);
	}
	return totalArea1;
}
