#ifndef SETTINGS_H
#define SETTINGS_H

#define N_SIDES (4)

// Squares functions settings:
#define EPSILON        ((double) 1e-6)
#define INIT_MARGIN    ((double) 0.50)

#define NEIGHBOURHOOD  (10)
#define ROTATION_RANGE ((float) 0.05)

// Those work well with n_squares = 5
#define ROTATION_PROBA ((float) 0.15f)
#define STEP_SIZE      ((float) 0.05)

// // Those work well with n_squares = 7
// #define ROTATION_PROBA ((float) 0.25f)
// #define STEP_SIZE      ((float) 0.1)

// Graphic settings:
#define WINDOW_WIDTH (1200)
#define WINDOW_HEIGHT (800)
#define POINT_SIZE      (8)
#define DRAW_SCALE   (100.)
#define FONT_SIZE      (25)
#define QUIT_KEY_1 (SDLK_ESCAPE)
#define QUIT_KEY_2 (SDLK_q)

#define FONT_NAME ("/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf")

#endif
