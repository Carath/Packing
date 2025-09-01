#ifndef SETTINGS_H
#define SETTINGS_H

#define N_SIDES (4)

// Polygons functions settings:
#define EPSILON        (1.e-6)
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
#define POINT_SIZE      (8)
#define DRAW_SCALE   (100.) // TODO: improve on this.
#define FONT_SIZE      (25)
#define QUIT_KEY_1 (SDLK_ESCAPE)
#define QUIT_KEY_2 (SDLK_q)

#define FONT_NAME ("/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf")

#endif
