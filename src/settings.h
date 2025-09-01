#ifndef SETTINGS_H
#define SETTINGS_H

#define N_SIDES (4)

// Polygons functions settings:
#define EPSILON        (1.e-9)
#define INIT_MARGIN    (0.50)

#define NEIGHBOURHOOD  (10)
#define ROTATION_RANGE (0.05)

// Those work well with n_polygons = 5
#define ROTATION_PROBA (0.15f)
#define STEP_SIZE      (0.05f)

// // Those work well with n_polygons = 7
// #define ROTATION_PROBA ((float) 0.25f)
// #define STEP_SIZE      ((float) 0.1)

// Graphic settings:
#define WINDOW_WIDTH  (1200)
#define WINDOW_HEIGHT (1000)
#define POINT_SIZE       (6)
#define FRAME_MARGIN  (0.25) // should be between 0. and 0.5
#define FONT_SIZE       (30)
#define QUIT_KEY_1 (SDLK_ESCAPE)
#define QUIT_KEY_2 (SDLK_q)

#define FONT_NAME ("/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf")


// Somehow, the rng64 does not help at all. This might be
// due to the search over sensibility to small changes.
#define RNG32

#ifdef RNG32
#include "rng32.h"
#define rng_type rng32
#define rng_init rng32_init
#define rng_int  rng32_nextInt
#define rng_real rng32_nextFloat
#else
#include "rng64.h"
#define rng_type rng64
#define rng_init rng64_init
#define rng_int  rng64_nextInt
#define rng_real rng64_nextDouble
#endif

#endif
