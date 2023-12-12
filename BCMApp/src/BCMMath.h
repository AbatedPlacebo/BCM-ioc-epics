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

#define POINTS_PER_PARAB 10
#define INTERP_POINTS 5
#define MIN_TRIGGER 10 
#define GEN_INT_POINTS 100


double calcQ(TBCM* BCM){
  if (BCM->wnd1 > BCM->wnd2){
    double tmp = BCM->wnd1;
    BCM->wnd1 = BCM->wnd2;
    BCM->wnd2 = tmp;
  }
  double integral = 0.0;
  int i;
  double dt = WAVEFORM_LENGTH_TIME;
  int beg = BCM->wnd1 / dt;
  int end = BCM->wnd2 / dt;
  for (i = beg; i < end; i += 1){
    integral += fabs(BCM->arr[i]) * dt;
  }
  BCM->Q = BCM->QK * pow(10.0, -BCM->gain * BCM->gainK / 20.0) * integral;
  return BCM->Q;
}

double timeQ(TBCM* BCM){
  if (BCM->wnd1 > BCM->wnd2){
    double tmp = BCM->wnd1;
    BCM->wnd1 = BCM->wnd2;
    BCM->wnd2 = tmp;
  }
  double dt = WAVEFORM_LENGTH_TIME;
  int beg = BCM->wnd1 / dt;
  int end = BCM->wnd2 / dt;
  // j - num of maxs
  int i;
  double extT = 0;
  BCM->timeQY = 0;
  for (i = beg; (i < end - 1); i++){
    int diff = BCM->arr[i+1] - BCM->arr[i];
    if (BCM->minmax == 0)
    {
      if (diff > 0 && BCM->arr[i] < BCM->timeQY){
        extT = i;
        BCM->timeQY = BCM->arr[i];
      }
    }
    else if (diff < 0 && BCM->arr[i] > BCM->timeQY){
      extT = i;
      BCM->timeQY = BCM->arr[i];
    }
  }
  BCM->timeQ = extT * WAVEFORM_LENGTH_TIME;
  return extT * WAVEFORM_LENGTH_TIME;
}

template<typename T>
int find_roots(TBCM* BCM, T* roots)
{
  int root_count = 0;
  int previous_sign, current_sign;
  int root_condition = 0;
  double* data = BCM->arr;
  int size = BCM->arr_ne;
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
int interpolate(TBCM* BCM){
  int points_cnt = POINTS_PER_PARAB * INTERP_POINTS;
  T rootsx[INTERP_POINTS];
  T x[points_cnt];
  T y[points_cnt];
  int Nroots = find_roots(BCM, rootsx);
  gsl_interp_accel *acc;
  const gsl_interp_type *t = gsl_interp_cspline;
  gsl_spline *spline;
  T xi, yi;
  int currentx;
  for (int i = 0; i < Nroots - 1; i++)
  {
    acc = gsl_interp_accel_alloc();
    int N = interp_points(BCM->arr, rootsx, Nroots, x, y, i);
    if (N == 0)
      continue;
    currentx += N;
    spline = gsl_spline_alloc (t, N);
    gsl_spline_init(spline, x, y, N);
    WFM4(BCM->parabx)[i].resize(GEN_INT_POINTS + 1);
    WFM4(BCM->parab)[i].resize(GEN_INT_POINTS + 1);
    for (int j = 0; j <= GEN_INT_POINTS; j++)
    {
      xi = (1 - j / 100.0) * x[0] + (j / 100.0) * x[N-1];
      WFM4(BCM->parabx)[i][j] = xi;
      WFM4(BCM->parab)[i][j] = gsl_spline_eval (spline, xi, acc);
    }
    WFM4(BCM->parabx)[i] *= WAVEFORM_LENGTH_TIME;
    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);
  }
  return 0;
}


#endif
