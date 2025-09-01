#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "squares.h"
#include "drawing.h"

Solution init(int n_squares, rng32 *rng);
void optimize(Solution *sol, rng32 *rng, int iterationNumber);


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int main(int argc, char const *argv[])
{
	// const uint64_t seed = time(NULL);
	const uint64_t seed = 123456;
	rng32 rng = {0};
	rng32_init(&rng, seed, 0);

	const int n_squares = 5;
	// const int iterationNumber = 1000;
	const int iterationNumber = 1000000;

	Solution sol = init(n_squares, &rng);
	printf("Init error ratio: %.4f\n", sol.error);
	optimize(&sol, &rng, iterationNumber);
	printf("Best error ratio: %f\n", sol.error);
	printf("Best big square side: %f\n\n", sol.bigSquareSide);

	animation(sol);

	free(sol.sqArray);
	return 0;
}

Solution init(int n_squares, rng32 *rng)
{
	const int n_side = (int) sqrtf(n_squares);
	Square *sqArray = (Square*) calloc(n_squares, sizeof(Square));
	for (int k = 0; k < n_squares; ++k) {
		// const double x = rng32_nextFloat(rng), y = rng32_nextFloat(rng),
		// 	theta = rng32_nextFloat(rng); // random
		const int i = k / n_side, j = k % n_side;
		const double x = i * (1. + INIT_MARGIN), y = j * (1. + INIT_MARGIN), theta = 0.;
		sqArray[k] = createSquare(x, y, x + cos(theta), y + sin(theta), POS);
		// printSquare(sqArray + k);
	}
	double side = 0, error = 0;
	findErrorRatio(sqArray, n_squares, &side, &error);
	return (Solution) {sqArray, n_squares, side, error};
}

void optimize(Solution *sol, rng32 *rng, int iterationNumber)
{
	const int n_squares = sol->n_squares;
	Square *sqArray = sol->sqArray;
	Square *best_sqArray = (Square*) calloc(n_squares, sizeof(Square));
	memcpy(best_sqArray, sqArray, n_squares * sizeof(Square));
	for (int i = 0; i < iterationNumber; ++i) {
		for (int j = 0; j < n_squares; ++j) {
			const int idx = j; // trying to move every square before evaluating.
			// const int idx = rng32_nextInt(rng) % n_squares;
			mutation(rng, sqArray + idx);
		}
		if (checkConfiguration(sqArray, n_squares)) {
			double side = 0, error = 0;
			findErrorRatio(sqArray, n_squares, &side, &error);
			if (error < sol->error) { // greedy
				sol->bigSquareSide = side;
				sol->error = error;
				memcpy(best_sqArray, sqArray, n_squares * sizeof(Square));
				printf("Improvement at iteration %d: %.4f\n", i, sol->error);
			}
		}
		else // backtracking
			memcpy(sqArray, best_sqArray, n_squares * sizeof(Square));
	}
	free(best_sqArray);
}

// TODO:
// - add local exploration of several mutations, and take the best?
// - save good quality configurations.
