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
#include "K500.h"

#include <algorithm>

#include "chk_dt.h"
#include "chk.h"
extern int debug_level;

#define INTERP_POINTS 5

template <typename T>
class SignalProcessing {
  public:
    SignalProcessing() = delete;
    SignalProcessing(T* BCM){
      this->BCM = BCM;
    }
    double calcQ();
    double timeQ();
    int interp_points(int* roots, double* x, double* y, int i);
    int find_roots(int* roots);
    int interpolate();
    void doAll();

  private:
    T* BCM;
};

template <typename T>
double SignalProcessing<T>::calcQ(){
  if (BCM->wndBeg > BCM->wndEnd){
    std::swap(BCM->wndBeg, BCM->wndEnd); }
  double integral = 0.0;
  int i;
  int beg = round(BCM->wndBeg / WAVEFORM_LENGTH_TIME);
  int end = round(BCM->wndEnd / WAVEFORM_LENGTH_TIME);
  for (i = beg; i < end; i += 1){
    integral += fabs(BCM->arr[i]) * WAVEFORM_LENGTH_TIME;
  }
  BCM->Q = BCM->QK * pow(10.0, -BCM->k_gain * BCM->k_gain * 2 / 20.0) * integral;
  return BCM->Q;
}

template <typename T>
double SignalProcessing<T>::timeQ(){
  BCM->timeQ = 0;
  BCM->timeQY = 0;
  for (int i = 0; i < 4; i++) {
    if (BCM->minmax == 0){
      double* cur = WFM4(BCM->parab)[i].min();
      if (*cur < BCM->timeQY){
        int idx = WFM4(BCM->parab)[i].distance(cur);
        BCM->timeQ = WFM4(BCM->parabx)[i][idx];
        BCM->timeQY = WFM4(BCM->parab)[i][idx];
      }
    }
    else {
      double* cur = WFM4(BCM->parab)[i].max();
      if (*cur > BCM->timeQY){
        int idx = WFM4(BCM->parab)[i].distance(cur);
        BCM->timeQ = WFM4(BCM->parabx)[i][idx];
        BCM->timeQY = WFM4(BCM->parab)[i][idx];
      }
    }
  }
  return BCM->timeQ;
}

template <typename T>
int SignalProcessing<T>::find_roots(int* roots){
  int root_count = 0;
  int previous_sign, current_sign;
  int root_condition = 0;
  WFMtype(BCM->arr) data = WFM(BCM->arr);
  previous_sign = current_sign = GSL_SIGN(data[0]);
  int beg = round(BCM->wndBeg / WAVEFORM_LENGTH_TIME);
  int end = round(BCM->wndEnd / WAVEFORM_LENGTH_TIME);
  roots[0] = beg;
  for (int i = beg; i < end; i++){
    current_sign = GSL_SIGN(data[i]);
    if (fabs(data[i]) > (BCM->interpolation_trigger_value * fabs(BCM->current_coef)) && root_condition == 0){
      root_count++;
      root_condition = 1;
    }
    if (previous_sign != current_sign){
      roots[root_count] = (fabs(data[i]) < fabs(data[i-1])) ? i : i-1;
      if (roots[root_count] > roots[root_count-1] && root_condition == 1){
        root_condition = 0;
        if (root_count == INTERP_POINTS)
          return root_count;
      }
    }
    previous_sign = current_sign;
  }
  roots[root_count++] = end;
  return root_count;
}


template <typename T>
int SignalProcessing<T>::interp_points(int* roots, double* x, double* y, int i){
  int total_points = 0;
  int current_point = roots[i];
  int end_point = roots[i+1];
  auto it = WFM(BCM->arr).minmax(current_point, end_point);
  auto absmax = std::abs(*it.first) > std::abs(*it.second) ? it.first : it.second;
  int idx = WFM(BCM->arr).distance(absmax) - BCM->parab_offset;
  idx = (idx < 0) ? 0 : idx;
  for (int i = 0; (i < (BCM->parab_offset * 2)) && (i < OSCSIZE); i++) {
    x[total_points] = BCM->arrXt[idx + i];
    y[total_points++] = BCM->arr[idx + i];
  }
  return total_points;
}

template <typename T>
int SignalProcessing<T>::interpolate(){

  int points_cnt = BCM->points_per_parab * INTERP_POINTS;
  int rootsx[INTERP_POINTS];
  double x[points_cnt];
  double y[points_cnt];

  int Nroots = find_roots(rootsx);

  for (int i = 0; i < 4; i++) {
    WFM4(BCM->parabx)[i].linspace(0,0);
    WFM4(BCM->parab)[i].linspace(0,0);
  }

  if (Nroots < 2){
    return -1;
  }

  gsl_interp_accel *acc;
  const gsl_interp_type *t = gsl_interp_cspline;
  gsl_spline *spline;
  double xi, yi;
  for (int i = 0; i < Nroots - 1; i++){
    acc = gsl_interp_accel_alloc();
    int N = interp_points(rootsx, x, y, i);
    if (N == 0)
      continue;
    spline = gsl_spline_alloc (t, N);
    gsl_spline_init(spline, x, y, N);
    int size = WFM4(BCM->parabx)[i].size();
    for (int j = 0; j < size; j++)
    {
      xi = (1.0 - j / (double)(size)) * x[0] + (j / (double)(size)) * x[N-1];
      WFM4(BCM->parabx)[i][j] = xi;
      WFM4(BCM->parab)[i][j] = gsl_spline_eval (spline, xi, acc);
    }
    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);
  }
  return 0;
}
template <typename T>
void SignalProcessing<T>::doAll(){
  if (interpolate() == 0)
  {
    timeQ();
    calcQ();
  }
}
#endif
