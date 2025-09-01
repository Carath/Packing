#ifndef SQUARES_H
#define SQUARES_H

#include <stdbool.h>
#include "settings.h"
#include "rng32.h"

typedef enum {POS = 1, NEG = -1} Direction;

typedef struct
{
	double xA, yA, xB, yB, xC, yC, xD, yD;
	double xCenter, yCenter;
} Square; // square length = 1.

Square createSquare(double xA, double yA, double xB, double yB, Direction d);
void printSquare(const Square *s);
void translation(Square *s, double xDelta, double yDelta);
void rotation(Square *s, double angle);
void mutation(rng32 *rng, Square *s);
double findErrorRatio(const Square *sqArray, int n_squares);
bool intersects(const Square *s1, const Square *s2);
bool checkConfiguration(const Square *sqArray, int n_squares);
// bool checkSquare(const Square *s);

#endif
