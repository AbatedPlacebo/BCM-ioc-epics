#ifndef BCM_MATH_H
#define BCM_MATH_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_spline.h>
#include "BCM.h"
#include "waveFormMap.h"

#define POINTS_PER_PARAB 5
#define INTERP_POINTS 5
#define MIN_TRIGGER 10

extern int debug_level;

double calcQ(TBCM* BCM);

double timeQ(TBCM* BCM);

int find_roots(TBCM* BCM, int* roots);

int interp_points(TBCM* BCM, int* roots, double* x, double* y, int i);

int interpolate(TBCM* BCM);

#endif
