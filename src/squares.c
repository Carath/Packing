#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "squares.h"

// static const double twoPi = 6.28318530718;

Square createSquare(double xA, double yA, double xB, double yB, Direction d) // specific to squares. TODO!
{
	assert(fabs(distance2(xA, yA, xB, yB) - 1.) < EPSILON);
	const double xC = xB - d * (yB-yA), yC = yB + d * (xB-xA);
	Square s = {0};
	s.points[0] = (Point) {xA, yA};
	s.points[1] = (Point) {xB, yB};
	s.points[2] = (Point) {xC, yC};
	s.points[3] = (Point) {xA + xC - xB, yA + yC - yB};
	s.center = (Point) {(xA + xC) / 2., (yA + yC) / 2.};
	return s;
}

void printSquare(const Square *s)
{
	for (int i = 0; i < N_SIDES; ++i)
		printf("Point %d: (%.3f, %.3f)\n", i, s->points[i].x, s->points[i].y);
}

void translation(Square *s, double xDelta, double yDelta)
{
	for (int i = 0; i < N_SIDES; ++i) {
		s->points[i].x += xDelta;
		s->points[i].y += yDelta;
	}
	s->center.x += xDelta;
	s->center.y += yDelta;
	// Last two lines could be part of the loop if 'center' was the array's last spot.
}

// Rotation center is square center.
void rotation(Square *s, double angle)
{
	const double co = cos(angle), si = sin(angle);
	for (int i = 0; i < N_SIDES; ++i) {
		const double xDelta = s->points[i].x - s->center.x;
		const double yDelta = s->points[i].y - s->center.y;
		s->points[i].x = co * xDelta - si * yDelta + s->center.x;
		s->points[i].y = si * xDelta + co * yDelta + s->center.y;
	}
	// No need to update the center for this one.
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

	// TODO: try rotating around the corners too?

	translation(s, STEP_SIZE * rng32_nextFloat(rng), STEP_SIZE * rng32_nextFloat(rng));
}

Box findBoundary(const Square *sqArray, int n_squares)
{
	double xmin = INFINITY, xmax = -INFINITY, ymin = INFINITY, ymax = -INFINITY;
	for (int i = 0; i < n_squares; ++i) {

		for (int j = 0; j < N_SIDES; ++j) {
			xmin = fmin(xmin, sqArray[i].points[j].x);
			ymin = fmin(ymin, sqArray[i].points[j].y);
			xmax = fmax(xmax, sqArray[i].points[j].x);
			ymax = fmax(ymax, sqArray[i].points[j].y);
		}
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
	if (distance2(s1->center.x, s1->center.y, s2->center.x, s2->center.y) >= 2.)
		return false;

	Point *points1 = (Point*) s1->points;
	Point *points2 = (Point*) s2->points;
	Segment segments_1[N_SIDES] = {0};
	Segment segments_2[N_SIDES] = {0};
	for (int i = 0; i < N_SIDES; ++i) {
		segments_1[i] = (Segment) {points1 + i, points1 + (i+1)%N_SIDES};
		segments_2[i] = (Segment) {points2 + i, points2 + (i+1)%N_SIDES};
	}

	// 16 pairwise segments intersections.
	for (int i = 0; i < N_SIDES; ++i) {
		for (int j = 0; j < N_SIDES; ++j) {
			if (segmentsNonTrivialIntersection(segments_1 + i, segments_2 + j))
				return true;
		}
	}
	return false;
}
