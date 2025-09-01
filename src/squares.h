#ifndef SQUARES_H
#define SQUARES_H

#include <stdbool.h>
#include "settings.h"
#include "geom_tools.h"
#include "rng32.h"

typedef enum {POS = 1, NEG = -1} Direction;

typedef struct
{
	double xmin, xmax, ymin, ymax;
} Box;

typedef struct
{
	Square *sqArray;
	const int n_squares;
	double bigSquareSide;
	double error;
} Solution;

Square createSquare(double xA, double yA, double xB, double yB, Direction d);
void printSquare(const Square *s);
void translation(Square *s, double xDelta, double yDelta);
void rotation(Square *s, double angle);
void mutation(rng32 *rng, Square *s);
Box findBoundary(const Square *sqArray, int n_squares);
double findBigSquareSize(const Square *sqArray, int n_squares);
void findErrorRatio(const Square *sqArray, int n_squares, double *side, double *error);
bool intersects(const Square *s1, const Square *s2);
bool checkConfiguration(const Square *sqArray, int n_squares);

#endif
