#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include "squares.h"
#include "drawing.h"

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

	const int n_squares = 17;
	const int n_side = (int) sqrtf(n_squares);
	Square *sqArray = (Square*) calloc(n_squares, sizeof(Square));
	for (int i = 0; i < n_squares; ++i) {
		// const double x = rng32_nextFloat(&rng), y = rng32_nextFloat(&rng), theta = rng32_nextFloat(&rng);
		const double x = i / n_side, y = i % n_side, theta = 0.;
		sqArray[i] = createSquare(x, y, x + cos(theta), y + sin(theta), POS);
		printSquare(sqArray + i);
	}
	const double error_ratio = findErrorRatio(sqArray, n_squares);
	printf("Error ratio: %.3f\n", error_ratio);

	animation(sqArray, n_squares);
	free(sqArray);
	return 0;
}
