#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "squares.h"

// static const double twoPi = 6.28318530718;

inline double distance2(double x1, double y1, double x2, double y2);
inline void replace4Rot(double *x, double *y, double xCenter, double yCenter, double co, double si);

Square createSquare(double xA, double yA, double xB, double yB, bool direc)
{
	assert(fabs(distance2(xA, yA, xB, yB) - 1.) < EPSILON);
	const int sign = 2*direc-1; // in {-1, 1}
	Square s = {0};
	s.xA = xA, s.yA = yA;
	s.xB = xB, s.yB = yB;
	s.xC = xB - sign * (yB-yA);
	s.yC = yB + sign * (xB-xA);
	s.xD = xA + s.xC - xB;
	s.yD = yA + s.yC - yB;
	s.xCenter = (s.xA + s.xC) / 2.;
	s.yCenter = (s.yA + s.yC) / 2.;
	return s;
}

inline double distance2(double x1, double y1, double x2, double y2)
{
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
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
}

// Useful to update two variables while preventing side effects!
inline void replace4Rot(double *x, double *y, double xCenter, double yCenter, double co, double si)
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
}

// Question: is it faster to apply the mutation on the AB segment,
// and then regenerate the square with createSquare? This would require
// disabling the assert if said function, and save 'direc' in the struct.
void mutation(rng32 *rng, Square *s)
{
	// Rotations are rarely done. Optimization: only apply the
	// rotation when the picked random value is small enough.
	const float angle = rng32_nextFloat(rng);
	if (EPSILON < angle && angle < ROTATION_PROBA)
		rotation(s, angle);
	translation(s, STEP_SIZE * rng32_nextFloat(rng), STEP_SIZE * rng32_nextFloat(rng));
}

double findBoundingSize(const Square *sqArray, int n_squares)
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
	printf( "xmin: %.3f, xmax: %.3f\n"
			"ymin: %.3f, ymax: %.3f\n",
			xmin, xmax, ymin, ymax);
	return fmax(xmax - xmin, ymax - ymin);
}

// Only returns true on non-trivial intersections.
bool intersects(const Square *s1, const Square *s2)
{
	// Optimizations to not consider far away squares. Min distance is 8c².
	if (distance2(s1->xCenter, s1->yCenter, s2->xCenter, s2->yCenter) >= 8.)
		return false;

	// TODO: 16 pairwise segments intersections.
	return false;
}
