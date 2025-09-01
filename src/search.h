#ifndef SEARCH_H
#define SEARCH_H

#include "settings.h"
#include "polygons.h"

Solution init(int n_polygons, rng_type *rng);
void optimize(Solution *sol, rng_type *rng, int iterationNumber);
void optimize_2(Solution *sol, rng_type *rng, int iterationNumber);

#endif
