#ifndef SQUARES_H
#define SQUARES_H

#include <stdbool.h>
#include "rng32.h"

#define EPSILON ((double) 1e-6)
#define ROTATION_PROBA ((float) 0.05f)
#define STEP_SIZE      ((float) 0.05f)

typedef struct
{
	double xA, yA, xB, yB, xC, yC, xD, yD;
	double xCenter, yCenter;
} Square; // square length = 1.

Square createSquare(double xA, double yA, double xB, double yB, bool direc);
void printSquare(const Square *s);
void translation(Square *s, double xDelta, double yDelta);
void rotation(Square *s, double angle);
void mutation(rng32 *rng, Square *s);
double findBoundingSize(const Square *sqArray, int n_squares);
bool intersects(const Square *s1, const Square *s2);
// bool checkSquare(const Square *s);

#endif
