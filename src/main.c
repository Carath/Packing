#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "polygons.h"
#include "drawing.h"
#include "search.h"

void testIntersection(void);
void testPolygonCreation(rng_type *rng);

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int main(int argc, char const *argv[])
{
	initConstExpr();

	// const uint64_t seed = time(NULL);
	const uint64_t seed = 123456;
	rng_type rng = {0};
	rng_init(&rng, seed, 0);

	// testIntersection();
	// testPolygonCreation(&rng);

	const int n_polygons = 5;
	// const int iterationNumber = 1000;
	const int iterationNumber = 1000000;
	// const int iterationNumber = 1000000 / NEIGHBOURHOOD;

	Solution sol = init(n_polygons, &rng);
	printf("Init error ratio: %.4f\n", sol.error);

	optimize(&sol, &rng, iterationNumber);
	// optimize_2(&sol, &rng, iterationNumber);

	printf("Best error ratio: %f\n", sol.error);
	printf("Best big square side: %f\n\n", sol.bigSquareSide);

	animation(sol);

	free(sol.polArray);
	return 0;
}

void testIntersection(void)
{
	Point p = {0};
	const Point A = {-1, 0}, B = {1, 0};
	const Point C = {0, -1}, D = {0, 1};
	const Segment AB = {&A, &B}, CD = {&C, &D};
	const bool res = segmentsIntersection(&AB, &CD, true, &p);
	printf("Intersection: %d\n", res);
	if (res)
		printPoint(&p);
	exit(0);
}

void testPolygonCreation(rng_type *rng)
{
	Polygon pol = createPolygon(rng_real(rng), rng_real(rng));
	double err_max = 0.;
	for (int i = 0; i < N_SIDES; ++i) {
		const double length = distance(pol.points + i, pol.points + (i+1) % N_SIDES);
		const double err = relative_error(1., length);
		err_max = fmax(err_max, err);
	}
	printf("Length max relative error: %g\n\n", err_max);
	exit(0);
}
