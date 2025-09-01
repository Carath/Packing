#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "search.h"

Solution init(int n_polygons, rng_type *rng)
{
	// const double diameter = 2. * getRadius();
	const int n_side = (int) sqrtf(n_polygons);
	Polygon *polArray = (Polygon*) calloc(n_polygons, sizeof(Polygon));
	for (int k = 0; k < n_polygons; ++k) {
		const int i = k / n_side, j = k % n_side;
		const double x = i * (1. + INIT_MARGIN), y = j * (1. + INIT_MARGIN);
		// #define INIT_MARGIN_2 (0.0606598435597)
		// const double x = i * diameter * (1. + INIT_MARGIN_2), y = j * diameter * (1. + INIT_MARGIN_2);
		polArray[k] = createPolygon(x, y);
		// printPolygon(polArray + k);
	}
	double side = 0, error = 0;
	findErrorRatio(polArray, n_polygons, &side, &error);
	return (Solution) {polArray, n_polygons, side, error};
}

void optimize(Solution *sol, rng_type *rng, int iterationNumber)
{
	const int n_polygons = sol->n_polygons;
	Polygon *polArray = sol->polArray;
	Polygon *best_polArray = (Polygon*) calloc(n_polygons, sizeof(Polygon));
	memcpy(best_polArray, polArray, n_polygons * sizeof(Polygon));
	for (int i = 0; i < iterationNumber; ++i) {
		for (int j = 0; j < n_polygons; ++j) {
			const int idx = j; // trying to move every polygon before evaluating.
			// const int idx = rng_type_nextInt(rng) % n_polygons;
			mutation(rng, polArray + idx);
		}
		if (checkConfiguration(polArray, n_polygons)) {
			double side = 0, error = 0;
			findErrorRatio(polArray, n_polygons, &side, &error);
			if (error < sol->error) { // greedy
				sol->bigSquareSide = side;
				sol->error = error;
				memcpy(best_polArray, polArray, n_polygons * sizeof(Polygon));
				printf("Improvement at iteration %d: %.4f\n", i, sol->error);
			}
		}
		else // backtracking
			memcpy(polArray, best_polArray, n_polygons * sizeof(Polygon));
	}
	free(best_polArray);
}

void optimize_2(Solution *sol, rng_type *rng, int iterationNumber)
{
	const int n_polygons = sol->n_polygons;
	Polygon *polArray = sol->polArray;
	Polygon* buffer[NEIGHBOURHOOD] = {0};
	for (int k = 0; k < NEIGHBOURHOOD; ++k) {
		buffer[k] = (Polygon*) calloc(n_polygons, sizeof(Polygon));
		memcpy(buffer[k], polArray, n_polygons * sizeof(Polygon));
	}

	// bool progress = true; // to init the buffer
	for (int i = 0; i < iterationNumber; ++i) {

		// if (progress) {
		// 	for (int k = 0; k < NEIGHBOURHOOD; ++k)
		// 		memcpy(buffer[k], polArray, n_polygons * sizeof(Polygon));
		// 	progress = false;
		// }

		for (int k = 0; k < NEIGHBOURHOOD; ++k) { // for local exploration
			for (int j = 0; j < n_polygons; ++j) {
				const int idx = j; // trying to move every polygon before evaluating.
				// const int idx = rng_type_nextInt(rng) % n_polygons;
				mutation(rng, buffer[k] + idx);
			}
			if (checkConfiguration(buffer[k], n_polygons)) {
				double side = 0, error = 0;
				findErrorRatio(buffer[k], n_polygons, &side, &error);
				if (error < sol->error) { // greedy
					// progress = true;
					sol->bigSquareSide = side;
					sol->error = error;
					memcpy(polArray, buffer[k], n_polygons * sizeof(Polygon));
					printf("Improvement at iteration %d: %.4f\n", i, sol->error);
				}
			}
			else // backtracking
				memcpy(buffer[k], polArray, n_polygons * sizeof(Polygon)); // may not be up to date
		}

	}
	for (int k = 0; k < NEIGHBOURHOOD; ++k)
		free(buffer[k]);
}
