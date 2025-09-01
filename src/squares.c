#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "squares.h"

static const double Pi = 3.14159265359;
static const double N_angle = 2. * Pi / N_SIDES;
static double Diam2 = 0.;
static double Radius = 0.;

// To be called at the program's start.
void initConstExpr(void)
{
	assert(N_SIDES > 2);
	Diam2 = 8. / (N_SIDES * sin(N_angle)); // squared diameter.
	Radius = sqrt(Diam2) / 2.; // chosen so that the area is 1 for all N_SIDES.
}

double getRadius(void)
{
	return Radius;
}

// Generate a square of area equal to 1.
Square createSquare(double xCenter, double yCenter)
{
	Square sq = {0};
	sq.center = (Point) {xCenter, yCenter};
	for (int i = 0; i < N_SIDES; ++i) {
		const double angle = (i + 0.5) * N_angle;
		sq.points[i] = (Point) {xCenter + Radius * cos(angle), yCenter + Radius * sin(angle)};
	}
	return sq;
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
		s->points[i] = rotatePoint(s->center, s->points[i], co, si);
	} // No need to update the center for this one.
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

double relative_error(double ref, double x)
{
	assert(ref != 0.);
	return fabs((x - ref) / ref);
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
	// Huge optimization to not consider far away squares.
	if (distance2(s1->center.x, s1->center.y, s2->center.x, s2->center.y) >= Diam2)
		return false;

	Segment segments_1[N_SIDES] = {0};
	Segment segments_2[N_SIDES] = {0};
	for (int i = 0; i < N_SIDES; ++i) {
		segments_1[i] = (Segment) {s1->points + i, s1->points + (i+1) % N_SIDES};
		segments_2[i] = (Segment) {s2->points + i, s2->points + (i+1) % N_SIDES};
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
