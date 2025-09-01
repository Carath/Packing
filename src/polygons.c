#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "polygons.h"

static const double Pi = 3.14159265359;
static const double N_angle = 2. * Pi / N_SIDES;
static double Diam2 = 0.;
static double Radius = 0.;

// To be called at the program's start.
void initConstExpr(void)
{
	assert(N_SIDES > 2);
	Diam2 = 8. / (N_SIDES * sin(N_angle)); // squared diameter.
	Radius = sqrt(Diam2) / 2.; // chosen so that the area is 1 for all N_SIDES.
}

double getRadius(void)
{
	return Radius;
}

// Generate a polygon of area equal to 1.
Polygon createPolygon(double xCenter, double yCenter)
{
	Polygon pol = {0};
	pol.center = (Point) {xCenter, yCenter};
	for (int i = 0; i < N_SIDES; ++i) {
		const double angle = (i + 0.5) * N_angle;
		pol.points[i] = (Point) {xCenter + Radius * cos(angle), yCenter + Radius * sin(angle)};
	}
	return pol;
}

void printPolygon(const Polygon *pol)
{
	for (int i = 0; i < N_SIDES; ++i)
		printf("Point %d: (%.3f, %.3f)\n", i, pol->points[i].x, pol->points[i].y);
}

void translation(Polygon *pol, double xDelta, double yDelta)
{
	for (int i = 0; i < N_SIDES; ++i) {
		pol->points[i].x += xDelta;
		pol->points[i].y += yDelta;
	}
	pol->center.x += xDelta;
	pol->center.y += yDelta;
	// Last two lines could be part of the loop if 'center' was the array's last spot.
}

// Rotation center is polygon center.
void rotation(Polygon *pol, double angle)
{
	const double co = cos(angle), si = sin(angle);
	for (int i = 0; i < N_SIDES; ++i) {
		pol->points[i] = rotatePoint(pol->center, pol->points[i], co, si);
	} // No need to update the center for this one.
}

// Question: is it faster to apply the mutation on the AB segment,
// and then regenerate the polygon with createPolygon? This would require
// disabling the assert if said function, and save 'direc' in the struct.
void mutation(rng32 *rng, Polygon *pol)
{
	const float proba = rng32_nextFloat(rng);

	if (proba < ROTATION_PROBA) {
		const double angle = proba - ROTATION_PROBA/2.;
		rotation(pol, angle); // angle between -ROTATION_PROBA/2. and ROTATION_PROBA/2.
	}

	// if (proba < ROTATION_PROBA) {
	// 	const double angle = (rng32_nextFloat(rng) - 0.5f) * ROTATION_RANGE;
	// 	rotation(pol, angle);
	// }

	// TODO: try rotating around the corners too?

	translation(pol, STEP_SIZE * rng32_nextFloat(rng), STEP_SIZE * rng32_nextFloat(rng));
}

Box findBoundary(const Polygon *polArray, int n_polygons)
{
	double xmin = INFINITY, xmax = -INFINITY, ymin = INFINITY, ymax = -INFINITY;
	for (int i = 0; i < n_polygons; ++i) {
		for (int j = 0; j < N_SIDES; ++j) {
			xmin = fmin(xmin, polArray[i].points[j].x);
			ymin = fmin(ymin, polArray[i].points[j].y);
			xmax = fmax(xmax, polArray[i].points[j].x);
			ymax = fmax(ymax, polArray[i].points[j].y);
		}
	}
	return (Box) {xmin, xmax, ymin, ymax};
}

inline double findBigPolygonSize(const Polygon *polArray, int n_polygons)
{
	const Box b = findBoundary(polArray, n_polygons);
	return fmax(b.xmax - b.xmin, b.ymax - b.ymin);
}

// Only pertinent when the polygons do not intersect non trivially,
// this should be verified with checkConfiguration() beforehand.
void findErrorRatio(const Polygon *polArray, int n_polygons, double *side, double *error)
{
	*side = findBigPolygonSize(polArray, n_polygons);
	*error = *side * *side / n_polygons - 1.;
	// *error = 1. - n_polygons / (*side * *side);
}

double relative_error(double ref, double x)
{
	assert(ref != 0.);
	return fabs((x - ref) / ref);
}

bool checkConfiguration(const Polygon *polArray, int n_polygons)
{
	for (int i = 0; i < n_polygons; ++i) {
		for (int j = i+1; j < n_polygons; ++j) {
			if (intersects(polArray + i, polArray + j))
				return false;
		}
	}
	return true;
}

// Only returns true on non-trivial intersections.
bool intersects(const Polygon *pol1, const Polygon *pol2)
{
	// Huge optimization to not consider far away polygons.
	if (distance2(pol1->center.x, pol1->center.y, pol2->center.x, pol2->center.y) >= Diam2)
		return false;

	Segment segments_1[N_SIDES] = {0};
	Segment segments_2[N_SIDES] = {0};
	for (int i = 0; i < N_SIDES; ++i) {
		segments_1[i] = (Segment) {pol1->points + i, pol1->points + (i+1) % N_SIDES};
		segments_2[i] = (Segment) {pol2->points + i, pol2->points + (i+1) % N_SIDES};
	}

	// 16 pairwise segments intersections.
	for (int i = 0; i < N_SIDES; ++i) {
		for (int j = 0; j < N_SIDES; ++j) {
			if (segmentsNonTrivialIntersection(segments_1 + i, segments_2 + j))
				return true;
		}
	}
	return false;
}
