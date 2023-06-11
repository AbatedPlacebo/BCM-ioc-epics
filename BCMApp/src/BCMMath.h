#ifndef BCM_MATH_H
#define BCM_MATH_H 

#include "BCMVariables.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_bspline.h>
#include <gsl/gsl_vector.h>
#include <math.h>

double calcQ(int* arr, int size, double wnd1, double wnd2, double QK, int gain, double gainK);

double timeQ(int* arr, int* extY, int size, double wnd1, double wnd2, int minmax);


#endif
