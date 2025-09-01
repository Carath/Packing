#ifndef SEARCH_H
#define SEARCH_H

#include "settings.h"
#include "polygons.h"
#include "rng32.h"

Solution init(int n_polygons, rng32 *rng);
void optimize(Solution *sol, rng32 *rng, int iterationNumber);
void optimize_2(Solution *sol, rng32 *rng, int iterationNumber);

#endif
