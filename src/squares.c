#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "squares.h"

// static const double twoPi = 6.28318530718;

Square createSquare(double xA, double yA, double xB, double yB, Direction d)
{
	assert(fabs(distance2(xA, yA, xB, yB) - 1.) < EPSILON);
	Square s = {0};
	s.xA = xA, s.yA = yA;
	s.xB = xB, s.yB = yB;
	s.xC = xB - d * (yB-yA);
	s.yC = yB + d * (xB-xA);
	s.xD = xA + s.xC - xB;
	s.yD = yA + s.yC - yB;
	s.xCenter = (s.xA + s.xC) / 2.;
	s.yCenter = (s.yA + s.yC) / 2.;
	return s;
}

void printSquare(const Square *s)
{
	printf( "xA = %.3f, yA = %.3f\nxB = %.3f, yB = %.3f\n"
			"xC = %.3f, yC = %.3f\nxD = %.3f, yD = %.3f\n",
			s->xA, s->yA, s->xB, s->yB, s->xC, s->yC, s->xD, s->yD);
}

void translation(Square *s, double xDelta, double yDelta)
{
	s->xA += xDelta;
	s->xB += xDelta;
	s->xC += xDelta;
	s->xD += xDelta;
	s->yA += yDelta;
	s->yB += yDelta;
	s->yC += yDelta;
	s->yD += yDelta;
	s->xCenter += xDelta;
	s->yCenter += yDelta;
}

// Useful to update two variables while preventing side effects!
static inline void replace4Rot(double *x, double *y, double xCenter,
	double yCenter, double co, double si)
{
	const double xDelta = *x - xCenter, yDelta = *y - yCenter;
	*x = co * xDelta - si * yDelta + xCenter;
	*y = si * xDelta + co * yDelta + yCenter;
}

// Rotation center is square center.
void rotation(Square *s, double angle)
{
	const double co = cos(angle), si = sin(angle);
	replace4Rot(&(s->xA), &(s->yA), s->xCenter, s->yCenter, co, si);
	replace4Rot(&(s->xB), &(s->yB), s->xCenter, s->yCenter, co, si);
	replace4Rot(&(s->xC), &(s->yC), s->xCenter, s->yCenter, co, si);
	replace4Rot(&(s->xD), &(s->yD), s->xCenter, s->yCenter, co, si);
	// No need to update the center.
}

// Question: is it faster to apply the mutation on the AB segment,
// and then regenerate the square with createSquare? This would require
// disabling the assert if said function, and save 'direc' in the struct.
void mutation(rng32 *rng, Square *s)
{
	const float proba = rng32_nextFloat(rng);

	if (proba < ROTATION_PROBA) {
		const double angle = proba - ROTATION_PROBA/2.;
		rotation(s, angle); // angle between -ROTATION_PROBA/2. and ROTATION_PROBA/2.
	}

	// if (proba < ROTATION_PROBA) {
	// 	const double angle = (rng32_nextFloat(rng) - 0.5f) * ROTATION_RANGE;
	// 	rotation(s, angle);
	// }

	translation(s, STEP_SIZE * rng32_nextFloat(rng), STEP_SIZE * rng32_nextFloat(rng));
}

Box findBoundary(const Square *sqArray, int n_squares)
{
	double xmin = INFINITY, xmax = -INFINITY, ymin = INFINITY, ymax = -INFINITY;
	for (int i = 0; i < n_squares; ++i) {
		xmin = fmin(xmin, sqArray[i].xA);
		xmin = fmin(xmin, sqArray[i].xB);
		xmin = fmin(xmin, sqArray[i].xC);
		xmin = fmin(xmin, sqArray[i].xD);

		ymin = fmin(ymin, sqArray[i].yA);
		ymin = fmin(ymin, sqArray[i].yB);
		ymin = fmin(ymin, sqArray[i].yC);
		ymin = fmin(ymin, sqArray[i].yD);

		xmax = fmax(xmax, sqArray[i].xA);
		xmax = fmax(xmax, sqArray[i].xB);
		xmax = fmax(xmax, sqArray[i].xC);
		xmax = fmax(xmax, sqArray[i].xD);

		ymax = fmax(ymax, sqArray[i].yA);
		ymax = fmax(ymax, sqArray[i].yB);
		ymax = fmax(ymax, sqArray[i].yC);
		ymax = fmax(ymax, sqArray[i].yD);
	}
	return (Box) {xmin, xmax, ymin, ymax};
}

inline double findBigSquareSize(const Square *sqArray, int n_squares)
{
	const Box b = findBoundary(sqArray, n_squares);
	return fmax(b.xmax - b.xmin, b.ymax - b.ymin);
}

// Only pertinent when the squares do not intersect non trivially,
// this should be verified with checkConfiguration() beforehand.
void findErrorRatio(const Square *sqArray, int n_squares, double *side, double *error)
{
	*side = findBigSquareSize(sqArray, n_squares);
	*error = *side * *side / n_squares - 1.;
	// *error = 1. - n_squares / (*side * *side);
}

bool checkConfiguration(const Square *sqArray, int n_squares)
{
	for (int i = 0; i < n_squares; ++i) {
		for (int j = i+1; j < n_squares; ++j) {
			if (intersects(sqArray + i, sqArray + j))
				return false;
		}
	}
	return true;
}

// Only returns true on non-trivial intersections.
bool intersects(const Square *s1, const Square *s2)
{
	// Huge optimization to not consider far away squares. Min squared distance is 2c².
	if (distance2(s1->xCenter, s1->yCenter, s2->xCenter, s2->yCenter) >= 2.)
		return false;

	Point points_1[4] = {
		(Point) {s1->xA, s1->yA},
		(Point) {s1->xB, s1->yB},
		(Point) {s1->xC, s1->yC},
		(Point) {s1->xD, s1->yD}
	};
	Point points_2[4] = {
		(Point) {s2->xA, s2->yA},
		(Point) {s2->xB, s2->yB},
		(Point) {s2->xC, s2->yC},
		(Point) {s2->xD, s2->yD}
	};
	const Segment segments_1[4] = {
		(Segment) {points_1    , points_1 + 1},
		(Segment) {points_1 + 1, points_1 + 2},
		(Segment) {points_1 + 2, points_1 + 3},
		(Segment) {points_1 + 3, points_1    }
	};
	const Segment segments_2[4] = {
		(Segment) {points_2    , points_2 + 1},
		(Segment) {points_2 + 1, points_2 + 2},
		(Segment) {points_2 + 2, points_2 + 3},
		(Segment) {points_2 + 3, points_2    }
	};

	// 16 pairwise segments intersections.
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (segmentsIntersection(segments_1 + i, segments_2 + j))
				return true;
		}
	}
	return false;
}
