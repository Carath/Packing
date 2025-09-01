#include <stdlib.h>
#include <stdbool.h>
#include "drawing.h"
#include "settings.h"

extern SDL_Window *window;
extern SDL_Renderer *renderer;
static SDL_Event event = {0};
SDL_Color Lime = {0, 255, 0, 255};
SDL_Color Yellow = {255, 255, 0, 255};
// SDL colors cannot be const due to an SDLA limitation.

Point pointFromCoord(double xOffset, double yOffset, double x, double y)
{
	return (Point) { DRAW_SCALE * (x + xOffset), DRAW_SCALE * (y + yOffset)};
}

void drawPoint(const Point *point, const SDL_Color *color)
{
	if (!point) {
		printf("Cannot draw a NULL Point.\n");
		return;
	}
	SDLA_SetDrawColor(color->r, color->g, color->b);
	SDL_Rect rect = {point->x - POINT_SIZE / 2, point->y - POINT_SIZE / 2, POINT_SIZE, POINT_SIZE};
	SDL_RenderFillRect(renderer, &rect);
}

void drawSegment(const Segment *segment, const SDL_Color *color)
{
	if (!segment)
		return;
	else if (!(segment->start) && !(segment->end)) {
		printf("Cannot draw: Segment's start and end are NULL.\n");
		return;
	}
	else {
		SDLA_SetDrawColor(color->r, color->g, color->b);
		SDL_RenderDrawLine(renderer, (segment->start)->x, (segment->start)->y,
			(segment->end)->x, (segment->end)->y);
	}
}

void animation(Solution sol)
{
	SDLA_Init(&window, &renderer, "Squares", WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDLA_BLENDED);
	TTF_Font *font = TTF_OpenFont(FONT_NAME, FONT_SIZE);
	while (1) {
		SDLA_ClearWindow(NULL);

		// 'sol' could be updated here...
		const Box box = findBoundary(sol.sqArray, sol.n_squares);
		const double xOffset = (WINDOW_WIDTH  / DRAW_SCALE - (box.xmax - box.xmin)) / 2. - box.xmin;
		const double yOffset = (WINDOW_HEIGHT / DRAW_SCALE - (box.ymax - box.ymin)) / 2. - box.ymin;
		for (int i = 0; i < sol.n_squares; ++i) {
			Point A = pointFromCoord(xOffset, yOffset, sol.sqArray[i].xA, sol.sqArray[i].yA);
			Point B = pointFromCoord(xOffset, yOffset, sol.sqArray[i].xB, sol.sqArray[i].yB);
			Point C = pointFromCoord(xOffset, yOffset, sol.sqArray[i].xC, sol.sqArray[i].yC);
			Point D = pointFromCoord(xOffset, yOffset, sol.sqArray[i].xD, sol.sqArray[i].yD);
			drawSegment(&(Segment) {&A, &B}, &Yellow);
			drawSegment(&(Segment) {&B, &C}, &Yellow);
			drawSegment(&(Segment) {&C, &D}, &Yellow);
			drawSegment(&(Segment) {&D, &A}, &Yellow);
			drawPoint(&A, &Lime);
			drawPoint(&B, &Lime);
			drawPoint(&C, &Lime);
			drawPoint(&D, &Lime);
		}
		if (font) {
			char bufferStr[100] = {0};
			sprintf(bufferStr, "Error ratio: %.4f", sol.error);
			SDLA_SlowDrawText(font, &Yellow, 50, 50, bufferStr);
			sprintf(bufferStr, "Big square size: %.4f", sol.bigSquareSide);
			SDLA_SlowDrawText(font, &Yellow, 50, 100, bufferStr);
		}

		SDL_RenderPresent(renderer);
		SDL_WaitEvent(&event);
		if (event.type == SDL_QUIT ||
			(event.type == SDL_KEYDOWN && event.key.keysym.sym == QUIT_KEY_1) ||
			(event.type == SDL_KEYDOWN && event.key.keysym.sym == QUIT_KEY_2))
			break;
	}
	TTF_CloseFont(font);
	SDLA_Quit();
}
