#ifndef DRAWING_H
#define DRAWING_H

#include "SDLA.h"
#include "squares.h"

// Graphic settings:
#define WINDOW_WIDTH (1200)
#define WINDOW_HEIGHT (800)
#define POINT_SIZE (12)
#define DRAW_OFFSET (400.)
#define DRAW_SCALE  (100.)
#define QUIT_KEY_1 (SDLK_ESCAPE)
#define QUIT_KEY_2 (SDLK_q)

typedef struct
{
	double X;
	double Y;
} Point;

typedef struct
{
	Point *Start;
	Point *End;
} Segment;

// Line of equation: ax + by + c = 0:
typedef struct
{
	double a;
	double b;
	double c;
} Line;

Point pointFromCoord(double x, double y);
void drawPoint(const Point *point, const SDL_Color *color);
void drawSegment(const Segment *segment, const SDL_Color *color);
void animation(const Square *sqArray, int length);

#endif
