#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#define EPSILON ((double) 1e-6)

typedef struct
{
	double xA, yA, xB, yB, xC, yC, xD, yD;
	double xCenter, yCenter;
} Square; // square length = 1.

Square createSquare(double xA, double yA, double xB, double yB, bool direc);
double distance2(double x1, double y1, double x2, double y2);
void printSquare(const Square *s);
// bool checkSquare(const Square *s);

int main(int argc, char const *argv[])
{
	const Square s = createSquare(0., 0., cos(0.42), sin(0.42), 1);
	printSquare(&s);
	return 0;
}

Square createSquare(double xA, double yA, double xB, double yB, bool direc)
{
	assert(fabs(distance2(xA, yA, xB, yB) - 1.) < EPSILON);
	const int sign = 2*direc-1; // in {-1, 1}
	Square s = {0};
	s.xA = xA; s.yA = yA;
	s.xB = xB; s.yB = yB;
	s.xC = xB - sign * (yB-yA);
	s.yC = yB + sign * (xB-xA);
	s.xD = xA + s.xC - xB;
	s.yD = yA + s.yC - yB;
	s.xCenter = (s.xA + s.xC) / 2.;
	s.yCenter = (s.yA + s.yC) / 2.;
	return s;
}

double distance2(double x1, double y1, double x2, double y2)
{
	return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

void printSquare(const Square *s)
{
	printf( "xA = %.3f, yA = %.3f\nxB = %.3f, yB = %.3f\n"
			"xC = %.3f, yC = %.3f\nxD = %.3f, yD = %.3f\n",
			s->xA, s->yA, s->xB, s->yB, s->xC, s->yC, s->xD, s->yD);
}
