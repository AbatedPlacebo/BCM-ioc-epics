#include "BCMMath.h"

double calcQ(TBCM* BCM){
  if (BCM->wnd1 > BCM->wnd2){
    double tmp = BCM->wnd1;
    BCM->wnd1 = BCM->wnd2;
    BCM->wnd2 = tmp;
  }
  double integral = 0.0;
  int i;
  int beg = round(BCM->wnd1 / WAVEFORM_LENGTH_TIME);
  int end = round(BCM->wnd2 / WAVEFORM_LENGTH_TIME);
  for (i = beg; i < end; i += 1){
    integral += fabs(BCM->arr[i]) * WAVEFORM_LENGTH_TIME;
  }
  BCM->Q = BCM->QK * pow(10.0, -BCM->gain * BCM->gainK / 20.0) * integral;
  return BCM->Q;
}


double timeQ(TBCM* BCM){
  BCM->timeQ = 0;
  BCM->timeQY = 0;
  for (int i = 0; i < 4; i++) {
    size_t idx = 0;
    if (BCM->minmax == 0){
      double cur = WFM4(BCM->parab)[i].min(idx);
      if (cur < BCM->timeQY){
        BCM->timeQ = WFM4(BCM->parabx)[i][idx];
        BCM->timeQY = WFM4(BCM->parab)[i][idx];
      }
    }
    else {
      double cur = WFM4(BCM->parab)[i].max(idx);
      if (cur > BCM->timeQY){
        BCM->timeQ = WFM4(BCM->parabx)[i][idx];
        BCM->timeQY = WFM4(BCM->parab)[i][idx];
      }
    }
  }
  return BCM->timeQ;
}


int find_roots(TBCM* BCM, int* roots){
  int root_count = 0;
  int previous_sign, current_sign;
  int root_condition = 0;
  double* data = BCM->arr;
  int size = BCM->arr_ne;
  previous_sign = current_sign = GSL_SIGN(data[0]);
  int beg = round(BCM->wnd1 / WAVEFORM_LENGTH_TIME);
  int end = round(BCM->wnd2 / WAVEFORM_LENGTH_TIME);
  roots[0] = beg;
  for (int i = beg; i < end; i++){
    current_sign = GSL_SIGN(data[i]);
    if (fabs(data[i]) > MIN_TRIGGER && root_condition == 0){
      root_count++;
      root_condition = 1;
    }
    if (previous_sign != current_sign){
      roots[root_count] = (fabs(data[i]) < fabs(data[i-1])) ? i : i-1;
      if (roots[root_count] != roots[root_count-1] && root_condition == 1){
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

int interp_points(TBCM* BCM, int* roots, double* x, double* y, int i){
  int total_points = 0;
  int current_point = roots[i];
  int step = (roots[i+1] - roots[i]) / POINTS_PER_PARAB ;
  step = step == 1 ? 2 : step;
  while (current_point <= roots[i+1] && step > 0){
    x[total_points] = BCM->arrXt[current_point];
    y[total_points++] = BCM->arr[current_point];
    current_point += step;
  }
  if (total_points > 0){
    x[total_points-1] = BCM->arrXt[roots[i+1]];
    y[total_points-1] = BCM->arr[roots[i+1]];
  }
  return total_points;
}

int interpolate(TBCM* BCM){
  int points_cnt = POINTS_PER_PARAB * INTERP_POINTS;
  int rootsx[INTERP_POINTS];
  double x[points_cnt];
  double y[points_cnt];
  int Nroots = find_roots(BCM, rootsx);
  if (Nroots < 2)
    return -1;
  gsl_interp_accel *acc;
  const gsl_interp_type *t = gsl_interp_cspline;
  gsl_spline *spline;
  double xi, yi;
  for (int i = 0; i < 4; i++){
    WFM4(BCM->parabx)[i].linspace(0,0);
    WFM4(BCM->parab)[i].linspace(0,0);
  }
  for (int i = 0; i < Nroots - 1; i++){
    acc = gsl_interp_accel_alloc();
    int N = interp_points(BCM, rootsx, x, y, i);
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

