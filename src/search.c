#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "search.h"

// Solution init(int n_polygons, rng_type *rng)
// {
// 	Polygon *polArray = (Polygon*) calloc(n_polygons, sizeof(Polygon));
// 	for (int k = 0; k < n_polygons; ++k) {
// 		const double x = 5. * rng_real(rng);
// 		const double y = 5. * rng_real(rng);
// 		polArray[k] = createPolygon(x, y);
// 	}
// 	double side = 0, error = 0;
// 	findErrorRatio(polArray, n_polygons, &side, &error);
// 	return (Solution) {polArray, n_polygons, side, error};
// }

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

bool optimize_area(Solution *sol, rng_type *rng, int iterationNumber)
{
	const int n_polygons = sol->n_polygons;
	Polygon *polArray = sol->polArray;
	Polygon *best_polArray = (Polygon*) calloc(n_polygons, sizeof(Polygon));
	memcpy(best_polArray, polArray, n_polygons * sizeof(Polygon));

	// double weight = 0.1;
	double weight = 0.5;
	// double weight = 5.;

	double best_score = INFINITY;
	for (int i = 0; i < iterationNumber; ++i) {
		for (int j = 0; j < n_polygons; ++j) {
			const int idx = j; // trying to move every polygon before evaluating.
			// const int idx = rng_int(rng) % n_polygons;
			mutation(rng, polArray + idx);
		}

		const double area = configurationQuality(polArray, n_polygons);
		double side = 0, error = 0;
		findErrorRatio(polArray, n_polygons, &side, &error);

		if (i % 1024 == 0)
			weight *= 4.;

		// const double score = weight * area * area + side; // using 'side' for 'error' can be negative now.
		const double score = weight * area + side; // using 'side' for 'error' can be negative now.
		// const double score = (1. + area) + side; // using 'side' for 'error' can be negative now.
		// const double score = (1. + area) * (1. + area) + side; // using 'side' for 'error' can be negative now.

		// const double score = (area < 0.05 ? area : INFINITY) + side;
		// const double score = (area < 0.05 ? 0. : INFINITY) + side;

		// Maybe bound the allowed non-zero area instead?

		if (score < best_score) { // greedy
			best_score = score;
			sol->bigSquareSide = side;
			sol->error = error;
			memcpy(best_polArray, polArray, n_polygons * sizeof(Polygon));
			printf("Improvement at iteration %d: %.4f\n", i, best_score);
		}
		else // backtracking
			memcpy(polArray, best_polArray, n_polygons * sizeof(Polygon));
	}
	free(best_polArray);
	return checkConfiguration(polArray, n_polygons);
}

// Simulated annealing test
void optimize_sa(Solution *sol, rng_type *rng, int iterationNumber)
{
	const int n_polygons = sol->n_polygons;
	Polygon *polArray = sol->polArray;
	Polygon *best_polArray = (Polygon*) calloc(n_polygons, sizeof(Polygon));
	memcpy(best_polArray, polArray, n_polygons * sizeof(Polygon));

	double lambda = 0.01;

	double best_score = INFINITY;
	for (int i = 0; i < iterationNumber; ++i) {
		for (int j = 0; j < n_polygons; ++j) {
			const int idx = j; // trying to move every polygon before evaluating.
			// const int idx = rng_int(rng) % n_polygons;
			mutation(rng, polArray + idx);
		}
		if (checkConfiguration(polArray, n_polygons)) {
			double side = 0, error = 0;
			findErrorRatio(polArray, n_polygons, &side, &error);

			const double score = side; // to be minimized
			// const double score = error; // to be minimized

			if (score < best_score) {
				// best_score = score;
				// sol->bigSquareSide = side;
				// sol->error = error;
				// memcpy(best_polArray, polArray, n_polygons * sizeof(Polygon));
				// printf("Improvement at iteration %d: %.4f\n", i, side);
			}

			// const double temperature = 1. - i / (double) iterationNumber;
			// // const double threshold = fmin(exp((best_score - score) / temperature), 1.);
			// const double threshold = fmin(exp(-(best_score - score) / temperature), 1.);

			const double threshold = exp(-lambda * score); // assumes score >= 0.
			if (i % 1024 == 0) {
				lambda *= 2.;
				// lambda *= 0.5;
			}

			if (rng_real(rng) < threshold) {
				; // keeping

				best_score = score;
				sol->bigSquareSide = side;
				sol->error = error;
				memcpy(best_polArray, polArray, n_polygons * sizeof(Polygon));
				printf("Improvement at iteration %d: %.4f\n", i, side);

			}
			else // backtracking
				memcpy(polArray, best_polArray, n_polygons * sizeof(Polygon));
		}
		else // backtracking
			memcpy(polArray, best_polArray, n_polygons * sizeof(Polygon));
	}
	free(best_polArray);
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
			// const int idx = rng_int(rng) % n_polygons;
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
				// const int idx = rng_int(rng) % n_polygons;
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
