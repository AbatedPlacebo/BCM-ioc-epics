#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_spline.h>

#define POINTS_PER_PARAB 5 
#define START 400 
#define END 2000
#define SIZE END - START

int find_roots(double* data, int size, double* rootsx){
  int cnt = 1;
  int mountcnt = 0;
  int prevsign, cursign;
  prevsign = cursign = GSL_SIGN(data[START]);
  rootsx[0] = START;
  for (int i = START; i < END; i++){
    cursign = GSL_SIGN(data[i]);
    if (prevsign != cursign){
      rootsx[cnt] = (data[i] < data[i-1]) ? i : i-1;
        if (cnt > 0)
        {
          if (rootsx[cnt] != rootsx[cnt-1]){
          cnt++;
          }
        }
      }
    prevsign = cursign;
  }
  rootsx[cnt++] = END;
  return cnt;
}

int interp_points(double* data, double* rootsx, int size, double* x, double* y, int i){
  int cnt = size / 2 * POINTS_PER_PARAB;
  int j = 0;
  int curx = rootsx[i];
  int step = (rootsx[i+1] - rootsx[i]) / POINTS_PER_PARAB ;
  step = step == 1 ? 2 : step;
  while (curx <= rootsx[i+1] && step > 0){
    x[j] = curx;
    y[j++] = data[curx];
    curx += step;
  }
    x[j-1] = rootsx[i+1];
    y[j-1] = data[(int)rootsx[i+1]];
  return j;
}

int main (void)
{
  FILE* fp;
  fp = fopen("BCM.signal.dat", "r");
  char p[100];
  double rootsx[SIZE];
  double x[SIZE];
  double y[SIZE];
  int size;
  fscanf(fp, "%s %d",p, &size); 
  gsl_vector* arr = gsl_vector_alloc(size);
  for (int i = 0; i < size; i++){
    int val;
    fscanf(fp, "%d", &val);
    gsl_vector_set(arr, i, val);
  }
  int Nroots = find_roots(arr->data, arr->size, rootsx);
  /* Note: y[0] == y[3] for periodic data */

//  for (int i = 0; i < Nroots; i++)
//  {
//    printf ("%d %g\n", i, rootsx[i]);
//  }



  gsl_interp_accel *acc;
  const gsl_interp_type *t = gsl_interp_cspline;
  gsl_spline *spline;
  int currentx = START;
  double xi, yi;
  printf ("#m=0,S=3\n");
  for (int i = currentx; i < END; i++) {
    printf ("%d %g\n", i, arr->data[i]);
  }
  printf ("#m=1,S=0\n");
  for (int i = 0; i < Nroots - 1; i++){
    acc = gsl_interp_accel_alloc ();
    int N = interp_points(arr->data, rootsx, Nroots, x, y, i);
    if (N == 0)
      continue;
//    for (int i = 0; i < N; i++)
//    {
//      printf ("%d %g\n", i, x[i]);
//    }
    currentx += N;
    spline = gsl_spline_alloc (t, N);

    gsl_spline_init (spline, x, y, N);

    for (int i = 0; i <= 100; i++)
    {
      xi = (1 - i / 100.0) * x[0] + (i / 100.0) * x[N-1];
      yi = gsl_spline_eval (spline, xi, acc);
      printf ("%g %g\n", xi, yi);
    }
    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);
  }


  fclose(fp);
  return 0;
}
