#ifndef OSCINTERP_H
#define OSCINTERP_H
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_spline.h>

#define POINTS_PER_PARAB 10
#define INTERP_POINTS 5
#define MIN_TRIGGER 100
#define GEN_INT_POINTS 100

template<typename T>
int find_roots(T* data, int size, T* roots)
{
  int root_count = 0;
  int previous_sign, current_sign;
  int root_condition = 0;
  previous_sign = current_sign = GSL_SIGN(data[0]);
  roots[0] = 0;
  for (int i = 0; i < size; i++)
  {
    current_sign = GSL_SIGN(data[i]);
    if (fabs(data[i]) > MIN_TRIGGER && root_condition == 0)
    {
      root_count++;
      root_condition = 1;
    }
    if (previous_sign != current_sign)
    {
      roots[root_count] = (fabs(data[i]) < fabs(data[i-1])) ? i : i-1;
      if (roots[root_count] != roots[root_count-1] && root_condition == 1)
      {
        root_condition = 0;
        if (root_count == INTERP_POINTS)
          return root_count;
      }
    }
    previous_sign = current_sign;
  }
  roots[root_count++] = size;
  return root_count;
}

template<typename T>
int interp_points(T* data, T* roots, int size, T* x, T* y, int i)
{
  int total_points = 0;
  int current_point = roots[i];
  int step = (roots[i+1] - roots[i]) / POINTS_PER_PARAB ;
  step = step == 1 ? 2 : step;
  while (current_point <= roots[i+1] && step > 0)
  {
    x[total_points] = current_point;
    y[total_points++] = data[current_point];
    current_point += step;
  }
  x[total_points-1] = roots[i+1];
  y[total_points-1] = data[(int)roots[i+1]];
  return total_points;
}

template<typename T>
int interpolate(WFM4type(BCM.parab) y4){
  int points_cnt = POINTS_PER_PARAB * INTERP_POINTS;
  T rootsx[INTERP_POINTS];
  T x[points_cnt];
  T y[points_cnt];
  int Nroots = find_roots(data, size, rootsx);
  gsl_interp_accel *acc;
  const gsl_interp_type *t = gsl_interp_cspline;
  gsl_spline *spline;
  T xi, yi;
  int currentx;
  for (int i = 0; i < Nroots - 1; i++)
  {
    acc = gsl_interp_accel_alloc();
    int N = interp_points(data, rootsx, Nroots, x, y, i);
    if (N == 0)
      continue;
    currentx += N;
    spline = gsl_spline_alloc (t, N);
    gsl_spline_init(spline, x, y, N);
    for (int i = 0; i <= GEN_INT_POINTS; i++)
    {
       = (1 - i / 100.0) * x[0] + (i / 100.0) * x[N-1];
      yi = gsl_spline_eval (spline, xi, acc);
      printf ("%g %g\n", xi, yi);
    }
    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);
  }
  return 0;
}

#endif
