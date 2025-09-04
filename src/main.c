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
void testIntersectionArea(int n_polygons, rng_type *rng);
void testOriginsLinked(rng_type *rng);
void testIsPointInHalfPlane(void);

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int main(int argc, char const *argv[])
{
	initConstExpr();

	const int n_polygons = 5;

	// const uint64_t seed = time(NULL);
	const uint64_t seed = 123456;
	printf("seed: %lu\n", seed);
	rng_type rng = {0};
	rng_init(&rng, seed, 0);

	// testIntersection();
	// testPolygonCreation(&rng);
	// testIntersectionArea(n_polygons, &rng);
	// testOriginsLinked(&rng);
	// testIsPointInHalfPlane();

	// const int iterationNumber = 1000;
	const int iterationNumber = 1000000;
	// const int iterationNumber = 1000000 / NEIGHBOURHOOD;

	Solution sol = init(n_polygons, &rng);
	printf("Init error ratio: %.4f\n", sol.error);

	optimize(&sol, &rng, iterationNumber);
	// optimize_2(&sol, &rng, iterationNumber);
	// printf("OK status: %d\n", optimize_area(&sol, &rng, iterationNumber));
	// optimize_sa(&sol, &rng, iterationNumber);

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

void testIntersectionArea(int n_polygons, rng_type *rng)
{
	const Solution sol = init(n_polygons, rng);
	for (int i = 0; i < n_polygons; ++i) {
		for (int j = i+1; j < n_polygons; ++j) {
			const double area = intersectionArea(sol.polArray + i, sol.polArray + j);
			if (area > EPSILON)
				printf("Non zero-area for (%d, %d): %g\n", i, j, area);
		}
	}
	animation(sol);
	exit(0);
}

void testOriginsLinked(rng_type *rng)
{
	uint64_t sum = 0; // allowing overflows
	for (int i = 0; i < 200000000; ++i) {
		// const int n_points = 4 + rng_int(rng) % 9; // between 4 and 12 included.
		const Origin o1 = {rng_int(rng) % (N_SIDES+1) -1, rng_int(rng) % (N_SIDES+1) -1}; // removing 1 to emulate 'NO'.
		const Origin o2 = {rng_int(rng) % (N_SIDES+1) -1, rng_int(rng) % (N_SIDES+1) -1};
		// Multiplier must always be != 0:
		sum += (i+1) * originsLinked(o1, o2);
		// sum += (i % 17 + 1) * originsLinked(o1, o2); // smaller checksums
	}
	printf("sum: %lu\n", sum);
	exit(0);
}

void testIsPointInHalfPlane(void)
{
	// Very specific set of points:

	const Point points[] = {
		(Point) {0x1.9c94e881b913p+1, 0x1.7b9298f9b3c44p+0},
		(Point) {0x1.a1d65c1d353f4p+1, 0x1.4b5fe3b12c1b8p-1},
		(Point) {0x1.9ae367d64a496p+1, 0x1.4b095b8ce5eecp-1},
		(Point) {0x1.a089a20e162c9p+1, 0x1.7b728d1994f01p+0},
	};

	const Point center = getCenter(points, 4);
	const Line line = lineFromPoints(points, points+1);
	printf("isPointInHalfPlane: %d\n", isPointInHalfPlane(points, &center, &line, false));

	exit(0);

	// Point points[] = {
	// 	(Point) {0x1.5193f551a34d9p+2, 0x1.28926bcf666a4p+2},
	// 	(Point) {0x1.525a904f8f83fp+2, 0x1.28964125ae9fep+2},
	// 	(Point) {0x1.5193fd5a8d541p+2, 0x1.28926bc89afe3p+2},
	// 	(Point) {0x1.51b0fcce8e538p+2, 0x1.4ae65b5d43e2p+2},
	// };

	// for (int i = 0; i < 4; ++i)
	// 	printPoint(points + i);

	// printf("Distance between p0 and p2: %g\n", distance(points, points + 2));
	// printf("%g, %g\n", points[0].x - points[2].x, points[0].y - points[2].y);

	// const Point center = getCenter(points, 4);

	// Line lines[6] = {0};
	// int k = 0;
	// for (int i = 0; i < 4; ++i) {
	// 	for (int j = i+1; j < 4; ++j) {
	// 		lines[k++] = lineFromPoints(points + i, points + j);
	// 	}
	// }

	// k = 0;
	// for (int i = 0; i < 4; ++i) {
	// 	for (int j = i+1; j < 4; ++j) {
	// 		for (int l = 0; l < 4; ++l) {
	// 			printf("Plane test for i=%d, j=%d and point %d: %d\n", i, j, l,
	// 				isPointInHalfPlane(points + l, &center, lines + k, false));
	// 		}
	// 		++k;
	// 		printf("\n");
	// 	}
	// }

	// exit(0);
}
