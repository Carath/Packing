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

	// return sign * (line->a * point_test->x + line->b * point_test->y + line->c) >= (strict ? EPSILON : 0.); // bugged
	return sign * (line->a * point_test->x + line->b * point_test->y + line->c) >= (strict ? EPSILON : -EPSILON); // stable


	// stability issues?
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

bool isInPolygon(const Point *p, const Point polygonPoints[N_SIDES], const Line polygonlines[N_SIDES])
{
	for (int i = 0; i < N_SIDES; ++i) {
		if (!isPointInHalfPlane(p, polygonPoints + (i+2) % N_SIDES, polygonlines + i, false))
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

inline void swapPoint(Point *A, Point *B)
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






// First point is fixed. Then step by step, the next point will
// be the closest to the current one among the remaining ones.
void rearrange(Point *array, int length)
{
	for (int i = 0; i < length-1; ++i) {
		double d2min = INFINITY;
		int idxmin = 0;
		for (int j = i+1; j < length; ++j) {
			const double d2 = distance2(array + i, array + j);
			// if (d2 < d2min) {
			if (epsilonStrInequality(d2, d2min)) {
				d2min = d2;
				idxmin = j;
			}
		}
		swapPoint(array + i+1, array + idxmin);
	}
}

// ...
void rearrange_2(Point *array, int length)
{
	double d2_min = INFINITY;
	int i_min = 0, j_min = 0;
	for (int i = 0; i < length; ++i) {
		for (int j = i+1; j < length; ++j) {
			const double d2 = distance2(array + i, array + j);
			// if (d2 < d2_min) {
			if (epsilonStrInequality(d2, d2_min)) {
				d2_min = d2;
				i_min = i;
				j_min = j;
			}
		}
	}
	swapPoint(array + i_min, array);
	swapPoint(array + j_min, array+1);

	for (int i = 1; i < length-1; ++i) { // skipping the first pass.
		double d2min = INFINITY;
		int idxmin = 0;
		for (int j = i+1; j < length; ++j) {
			const double d2 = distance2(array + i, array + j);
			// if (d2 < d2min) {
			if (epsilonStrInequality(d2, d2min)) {
				d2min = d2;
				idxmin = j;
			}
		}
		swapPoint(array + i+1, array + idxmin);
	}
}

typedef struct
{
	double proximity;
	int index;
} Node;

// Insertion sort by increasing proximity:
static void sortNodes(Node *nodes, int size)
{
	for (int i = 1; i < size; ++i) {
		const Node ref = nodes[i];
		int j = i;
		// for (; j > 0 && nodes[j-1].proximity > ref.proximity; --j)
		for (; j > 0 && epsilonStrInequality(ref.proximity, nodes[j-1].proximity); --j) // more stable
			nodes[j] = nodes[j-1];
		nodes[j] = ref;
	}
}

// carjacking the proximity, by the angle's sine:
void rearrange_sin(Point *points, int length)
{
	// for (int i = 0; i < length; ++i)
	// 	printPoint(points + i);

	const Point center = getCenter(points, length);

	Node *nodes = (Node*) calloc(length-1, sizeof(Node));
	const Point AO = vector(points, &center);
	const double dAO = distance(points, &center);

	for (int i = 1; i < length; ++i) {
		const Point AB = vector(points, points + i);
		const double dAB = distance(points, points + i);
		const double det = detFromPoints(&AO, &AB);
		const double angleSine = det / (dAO * dAB);
		// assert(-1. <= angleSine && angleSine <= 1.);
		assert(epsilonInequality(-1., angleSine) && epsilonInequality(angleSine, 1.));
		nodes[i-1] = (Node) {angleSine, i};
	}

	sortNodes(nodes, length-1); // increasing or decreasing order.

	Point *copy = (Point*) calloc(length, sizeof(Point));
	memcpy(copy, points, length * sizeof(Point));
	for (int i = 1; i < length; ++i)
		points[i] = copy[nodes[i-1].index];

	// for (int i = 0; i < length-1; ++i)
	// 	printf("Node: (%d, %g)\n", nodes[i].index, nodes[i].proximity);
	// printf("\n");

	free(copy);
	free(nodes);
}
// Note: the sine approach fails for it is extremely sensitive to numerical instabilities.
// It requires to sort the angles sine, yet comparison are up to EPSILON so no proper order
// can be used to categorically compare those values...

bool rearrange_halfPlane(Point *points, int length)
{
	// printf("\nBefore:\n");
	// for (int k = 0; k < length; ++k) {
	// 	// printPoint(points + k);
	// 	printf("(Point) {%a, %a},\n", points[k].x, points[k].y);
	// }

	const Point center = getCenter(points, length);
	// printf("\nCenter: %g, %g\n", center.x, center.y);

	// Point *copy = (Point*) calloc(length, sizeof(Point));
	// memcpy(copy, points, length * sizeof(Point));

	for (int i = 0; i < length; ++i) {
		for (int j = i+1; j < length; ++j) {
			const Line line = lineFromPoints(points + i, points + j);

			bool valid = true;
			int k = 0;
			for (; k < length; ++k) {
				if (!isPointInHalfPlane(points + k, &center, &line, false)) {
					// printf("\ni=%d, j=%d, k=%d\n", i, j, k);
					// printPoint(points + i);
					// printPoint(points + j);
					// printPoint(points + k);
					valid = false;
					break;
				}
			}
			if (valid) {
				if (i+1 != j) {
					// printf("Swapping %d and %d\n", i+1, j);
					swapPoint(points + i+1, points + j);
				}
				break;
			}
			else {
				if (j == length-1) {
					printf("\nERROR for i=%d, j=%d, k=%d\n", i, j, k);

					const bool strict = false;
					const Point *point_test = points + k, *point_ref = &center;
					const double protoValue = line.a * point_ref->x + line.b * point_ref->y + line.c;
					const int sign = protoValue >= 0. ? 1 : -1;
					const double value = sign * (line.a * point_test->x + line.b * point_test->y + line.c);
					printf("protoValue: %g\n", protoValue);
					printf("sign: %d\n", sign);
					printf("value: %g\n", value);
					printf("res: %d\n", value >= (strict ? EPSILON : -EPSILON));

					// for (int l = 0; l < length; ++l)
					// 	printPoint(points + l);
					// exit(1);
					return false;
				}
			}
		}
	}
	// free(copy);
	return true;
}
// Note: Good approach but fails in some cases: due to EPSILON shenanigans,
// an half plane may seem ok but the underlaying segment should not be
// chosen least an invalid path be created. Fixing this would require
// constructing a matrix of valid segments and trying to extract a
// valid path from here, yielding to a cubic algorithm.

bool rearrange_intersection(Point *points, int length)
{
	printf("\nBefore:\n");
	for (int k = 0; k < length; ++k)
		printPoint(points + k);

	// Point *copy = (Point*) calloc(length, sizeof(Point));
	// memcpy(copy, points, length * sizeof(Point));

	for (int i = 0; i < length; ++i) {
		for (int j = i+1; j < length; ++j) {
			const Segment segm_ij = {points + i, points + j};

			bool valid = true;
			int k = 0;
			for (; k < length; ++k) {
				const Segment segm_ik = {points + i, points + k};
				const Segment segm_jk = {points + j, points + k};
				Point p = {0};
				if (segmentsIntersection(&segm_ij, &segm_ik, true, &p) || segmentsIntersection(&segm_ij, &segm_jk, true, &p)) {
					printf("\ni=%d, j=%d, k=%d\n", i, j, k);
					printPoint(points + i);
					printPoint(points + j);
					printPoint(points + k);
					valid = false;
					break;
				}
			}
			if (valid) {
				swapPoint(points + i+1, points + j);
				break;
			}
			else {
				if (j == length-1) {
					printf("\nERROR for i=%d, j=%d, k=%d\n", i, j, k);
					for (int k = 0; k < length; ++k)
						printPoint(points + k);
					// exit(1);
					return false;
				}
			}
		}
	}
	// free(copy);
	return true;
}
// Not working yet...


// ...
void betterArrangement(Point *points, int length)
{
	const int size = length * length;
	Node *nodes = (Node*) calloc(size, sizeof(Node));
	// bool *objetsSet = (bool*) calloc(length, sizeof(bool));
	// bool *objetsPrecSet = (bool*) calloc(length, sizeof(bool));
	// bool *indexSet = (bool*) calloc(length, sizeof(bool));
	int *indexCounts = (int*) calloc(length, sizeof(int));
	Point *copy = (Point*) calloc(length, sizeof(Point));
	memcpy(copy, points, length * sizeof(Point));

	// Point *buffer = (Point*) calloc(2 * length, sizeof(Point));
	int *buffer = (int*) calloc(2 * length, sizeof(int));


	#define EMPTY_VALUE (-1.) // must be < 0.
	for (int k = 0; k < size; ++k)
		nodes[k].proximity = EMPTY_VALUE;

	// Filling the superior part of the matrix with the
	// squared distance between each couple of points:
	for (int i = 0; i < length; ++i) {
		for (int j = i+1; j < length; ++j) {
			const double proximity = distance2(points + i, points + j);
			const int index = i * length + j;
			nodes[index] = (Node) {proximity, index};
		}
	}

	sortNodes(nodes, size);

	// Optimal matching:
	// int nbPointsLeft = length;
	// for (int k = 0; nbPointsLeft > 0; ++k) {
	int nbMatchFound = 0;
	for (int k = 0; nbMatchFound < length; ++k) {

	// for (int k = 0; k < size; ++k) {
		assert(k < size);

		if (nodes[k].proximity == EMPTY_VALUE) // only the superior part matters
			continue;
		const int idx = nodes[k].index;
		const int i = idx / length, j = idx % length;

		assert(i < j);

		// if (!indexSet[i] && !indexSet[j]) {
		if (indexCounts[i] < 2 && indexCounts[j] < 2) {
			++indexCounts[i];
			++indexCounts[j];
			// --nbPointsLeft;
			// ++nbMatchFound;

			// indexSet[i] = true;
			// indexSet[j] = true; //
			// --nbObjRestant; --nbObjPrecRestant;

			// i, j: ok
			// swapPoint(points + i, points + nbMatchFound++); // side effect...
			// swapPoint(points + j, points + nbMatchFound++); // side effect...
			// points[nbMatchFound++] = copy[i];
			// points[nbMatchFound++] = copy[j];
			// does not work, will overflow points!!!!

			// buffer[nbMatchFound++] = copy[i];
			// buffer[nbMatchFound++] = copy[j];
			buffer[nbMatchFound++] = i;
			buffer[nbMatchFound++] = j;
			--nbMatchFound;
		}
	}

	// points[0] = buffer[0]; // points of this
	// points[1] = buffer[1];
	// for (int i = 1; i < length; ++i) {
	// 	for (int j = 0; j < length; ++j) {
	// 		if (buffer[j])
	// 	}
	// 	points[k]   = buffer[2*k];
	// 	points[k+1] = buffer[2*k+1];
	// }


	int idx1 = buffer[0], idx2 = buffer[1];
	points[0] = copy[idx1];
	points[1] = copy[idx2];
	for (int i = 2; i < length; ++i) {
		for (int j = 1; j < length; ++j) { // use swapPoints o optimize?
			if (buffer[2*j] == idx2 && buffer[2*j+1] != idx1) {
				idx1 = idx2; idx2 = 2*j+1;
				points[i] = copy[idx2];
			}
			else if (buffer[2*j+1] == idx2 && buffer[2*j] != idx1) {
				idx1 = idx2; idx2 = 2*j;
				points[i] = copy[idx2];
			}
		}
	}

	free(buffer);
	free(copy);
	free(indexCounts);
	// free(indexSet);
	// free(objetsPrecSet);
	// free(objetsSet);
	free(nodes);
}



#include "drawing.h" // for debugging

// Idea: compute non-zero intersections area!
// To realize that, compute every intersection point of sides, keep them along with polygons
// corners inside the intersection. Said intersection is the convex hull of those points,
// to compute it just divide the area in triangles.
// Note: use the fact that the intersection will be a convex polygon.

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

	// 2*N_SIDES should be the max number of points of any intersection:
	int idx = 0;

	// N.B: isInPolygon does not check if it is inside the interior... But that is actually ok.

	////////////////

	// ...
	for (int i = 0; i < N_SIDES; ++i) {
		// if (isInPolygon(pol1->points + i, pol2->points, lines2)) // corners are accepted
		if (isInPolygon(pol1->points + i, pol2->points, lines2) && !isPointInArray(pol1->points + i, allIntersections, idx))
			allIntersections[idx++] = pol1->points[i];
		// if (isInPolygon(pol2->points + i, pol1->points, lines1)) // corners are accepted. Check redondancies here?
		if (isInPolygon(pol2->points + i, pol1->points, lines1) && !isPointInArray(pol2->points + i, allIntersections, idx))
			allIntersections[idx++] = pol2->points[i];
	}

	// ...
	for (int i = 0; i < N_SIDES; ++i) {
		for (int j = 0; j < N_SIDES; ++j) {
			Point p = {0};
			// if (segmentsIntersection(segment1 + i, segment2 + j, false, &p) && !isPointInArray(&p, allIntersections, idx))
			if (segmentsIntersection(segment1 + i, segment2 + j, true, &p))
				allIntersections[idx++] = p; // adding the intersection which is not a corner.
		}
	}

	// Are redondancies really problematic anyways?
	// Order of the last two blocks could be commuted with some slight adjustments.


	if (idx < 3) // no intersection or zero area.
		return 0.;

	if (!(idx <= 2*N_SIDES)) {
		printf("!(idx <= 2*N_SIDES)\n"); // to be sure.
		exit(1);
	}

	bool res = true;
	if (idx > 3) { // optimization?
		// rearrange(allIntersections, idx);
		// rearrange_2(allIntersections, idx);
		// betterArrangement(allIntersections, idx);
		// rearrange_sin(allIntersections, idx);
		res = rearrange_halfPlane(allIntersections, idx);
		// res = rearrange_intersection(allIntersections, idx);
	}
	printf("res: %d\n", res);

	// Sorting the intersection points, and splitting in triangles to sum the areas:

	return idx;
}


#define NO (-1) // must not be in [0, N_SIDES-1]

typedef struct
{
	int i, j;
} Origin;
// Here, the origin (i, j) represents:
// - point i of first polygon if j = NO
// - point j of second polygon if i = NO
// - else, a single point on the intersection of segments [i, i+1] and [j, j+1] (modulo N_SIDES)

inline int next(int k)
{
	return (k+1) % N_SIDES;
}

inline int prev(int k)
{
	return (k-1+N_SIDES) % N_SIDES;
}

inline bool originEquality(Origin o1, Origin o2)
{
	return o1.i == o2.i && o1.j == o2.j;
}

inline Origin newOrigin(int i, int j)
{
	return (Origin) {i, j};
}

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

int findIntersection_2(const Polygon *pol1, const Polygon *pol2, Point allIntersections[2*N_SIDES])
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

	// 2*N_SIDES should be the max number of points of any intersection:
	int idx = 0;

	// N.B: isInPolygon does not check if it is inside the interior... But that is actually ok.

	////////////////

	Origin origins[2*N_SIDES] = {0};

	// ...

	// First polygon first
	for (int i = 0; i < N_SIDES; ++i) {
		if (isInPolygon(pol1->points + i, pol2->points, lines2)) { // corners are accepted
			allIntersections[idx] = pol1->points[i];
			origins[idx] = (Origin) {i, NO};
			++idx;
		}
	}

	// Then the second:
	for (int i = 0; i < N_SIDES; ++i) {
		// if (isInPolygon(pol2->points + i, pol1->points, lines1)) { // corners are accepted. Check redondancies here?
		if (isInPolygon(pol2->points + i, pol1->points, lines1) && !isPointInArray(pol2->points + i, allIntersections, idx)) {
			allIntersections[idx] = pol2->points[i];
			origins[idx] = (Origin) {NO, i};
			++idx;
		}
	}

	// ...
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
			swapPoint(allIntersections + i+1, allIntersections + j);
			const Origin temp = origins[i+1];
			origins[i+1] = origins[j];
			origins[j] = temp;
		}
	}

	return idx;
}

double intersectionArea(const Polygon *pol1, const Polygon *pol2)
{
	Point allIntersections[2*N_SIDES] = {0};
	// const int idx = findIntersection(pol1, pol2, allIntersections);
	const int idx = findIntersection_2(pol1, pol2, allIntersections);

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
	// Sum the squared areas instead?

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
