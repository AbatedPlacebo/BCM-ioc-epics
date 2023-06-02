#ifndef BCM_MATH_H
#define BCM_MATH_H 

#include <math.h>
#include "BCMVariables.h"

double calcQ(int* arr, int size, int wnd1, int wnd2, double QK, int gain, double gainK);

double timeQ(int* arr, int size, int wnd1, int wnd2, int minmax);


#endif
