#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
// #include <limits.h>

#define EPSILON ((double) 1e-6)
#define ROTATION_PROBA ((float) 0.05f)
#define STEP_SIZE      ((float) 0.05f)

typedef struct
{
	double xA, yA, xB, yB, xC, yC, xD, yD;
	double xCenter, yCenter;
} Square; // square length = 1.

Square createSquare(double xA, double yA, double xB, double yB, bool direc);
inline double distance2(double x1, double y1, double x2, double y2);
void printSquare(const Square *s);
void translation(Square *s, double xDelta, double yDelta);
inline void replace4Rot(double *x, double *y, double xCenter, double yCenter, double co, double si);
void rotation(Square *s, double angle);
inline float randomFloat(void);
void mutation(Square *s);
bool intersects(const Square *s1, const Square *s2);
// bool checkSquare(const Square *s);

// static const double twoPi = 6.28318530718;

int main(int argc, char const *argv[])
{
	srand(123456);
	const Square s = createSquare(0., 0., cos(0.42), sin(0.42), 1);
	printSquare(&s);
	return 0;
}

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

// Values between 0. and 1.
inline float randomFloat(void) // TODO: add proper, thread-safe, fast RNG.
{
	return (float) rand() / RAND_MAX;
}

// Question: is it faster to apply the mutation on the AB segment,
// and then regenerate the square with createSquare? This would require
// disabling the assert if said function, and save 'direc' in the struct.
void mutation(Square *s)
{
	// Rotations are rarely done. Optimization: only apply the
	// rotation when the picked random value is small enough.
	const float angle = randomFloat();
	if (EPSILON < angle && angle < ROTATION_PROBA)
		rotation(s, angle);
	translation(s, STEP_SIZE * randomFloat(), STEP_SIZE * randomFloat());
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
