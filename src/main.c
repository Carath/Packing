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
	// Must be called here:
	SDLA_Init(&window, &renderer, "Squares", WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDLA_BLENDED);

	// const uint64_t seed = time(NULL);
	const uint64_t seed = 123456;
	rng32 rng = {0};
	rng32_init(&rng, seed, 0);

	const int n_squares = 4;
	const int iterationNumber = 100000;

	Solution sol = init(n_squares, &rng);
	printf("Init error ratio: %.3f\n", sol.error);
	optimize(&sol, &rng, iterationNumber);
	printf("Best error: %.3f\n", sol.error);

	printf("----> %d\n", checkConfiguration(sol.sqArray, n_squares));

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
	const double error = findErrorRatio(sqArray, n_squares);
	return (Solution) {sqArray, n_squares, error};
}

void optimize(Solution *sol, rng32 *rng, int iterationNumber)
{
	const int n_squares = sol->n_squares;
	double best_error = sol->error;
	Square *sqArray = sol->sqArray;
	Square *best_sqArray = (Square*) calloc(n_squares, sizeof(Square));
	memcpy(best_sqArray, sqArray, n_squares * sizeof(Square));
	for (int i = 0; i < iterationNumber; ++i) {
		for (int j = 0; j < n_squares; ++j) {
			const int idx = j;
			// const int idx = rng32_nextInt(rng) % n_squares;
			const Square copy = sqArray[idx]; // copy
			mutation(rng, sqArray + idx);
			if (checkConfiguration(sqArray, n_squares)) {
				const double err = findErrorRatio(sqArray, n_squares);
				if (err < best_error) {
					best_error = err;
					memcpy(best_sqArray, sqArray, n_squares * sizeof(Square));

					printf("Improvement at iteration %d: %.3f\n", i, best_error);
				}
			}
			else
				sqArray[idx] = copy; // backtracking
			// break;
		}
	}
	memcpy(sqArray, best_sqArray, n_squares * sizeof(Square));
	sol->error = best_error;
	free(best_sqArray);
}

// checkConfiguration bugged.
