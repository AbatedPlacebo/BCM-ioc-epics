#include "BCMMath.h"

double calcQ(int* arr, int size, double wnd1, double wnd2, double QK, int gain, double gainK){
	if (wnd1 > wnd2){
		double tmp = wnd1;
		wnd1 = wnd2;
		wnd2 = tmp;
	}
	double integral = 0.0;
	int i;
	double dt = WAVEFORM_LENGTH_TIME; 
	int beg = wnd1 / dt;
	int end = wnd2 / dt;
	for (i = beg; i < end; i += 1){
		integral += fabs(arr[i]) * dt; 
	}
	return QK * pow(10.0, -gain * gainK / 20.0) * integral;
}

double timeQ(int* arr, int* extY, int size, double wnd1, double wnd2, int minmax){
	if (wnd1 > wnd2){
		double tmp = wnd1;
		wnd1 = wnd2;
		wnd2 = tmp;
	}
	double dt = WAVEFORM_LENGTH_TIME; 
	int beg = wnd1 / dt;
	int end = wnd2 / dt;
	// j - num of maxs
	int i;
	double extT = 0;
	*extY = 0;
	for (i = beg; (i < end - 1); i++){
		int diff = arr[i+1] - arr[i];
		if (minmax == 0)
		{
			if (diff > 0 && arr[i] < *extY){
				extT = i;
				*extY = arr[i];
			}
		}
		else if (diff < 0 && arr[i] > *extY){
			extT = i;
			*extY = arr[i];
		}
	}
	return extT * WAVEFORM_LENGTH_TIME;
}


//double timeQ(int* arr, int* extY, int size, double wnd1, double wnd2, int minmax){
//	double dt = WAVEFORM_LENGTH_TIME; 
//
//	int beg = wnd1 / dt;
//	int end = wnd2 / dt;
//	int n = beg - end + 1;
//	int k = 4; // spline order	
//	double sigma = 0.001; /* noise */
//	double chisq;
//	
//	gsl_bspline_workspace *work = gsl_bspline_alloc(k, 10);
//
//	size_t pn = gsl_bspline_ncontrol(work); 
//	size_t dof = n - pn; /* degrees of freedom */
//
//	gsl_vector *x = gsl_vector_alloc(n);
//	gsl_vector *y = gsl_vector_alloc(n);
//	gsl_vector *w = gsl_vector_alloc(n);
//	gsl_vector *c1 = gsl_vector_alloc(pn);
//
//	int i, j;
//	for (i = beg, j = 0; i < end; ++i, j++)
//	{
//		gsl_vector_set(x, i, beg + (dt * j));
//		gsl_vector_set(y, i, arr[i]);
//		gsl_vector_set(w, i, 1.0 / (sigma * sigma));
//	}
//	/* use uniform breakpoints on [a, b] */
//	gsl_bspline_init_uniform(beg, end, work);
//	/* solve least squares problem */
//	gsl_bspline_wlssolve(x, y, w, c1, &chisq, work);
//	double minTy = 0, maxTy = 0;
//	double minT = 0, maxT = 0;
//	double xi;
//	for (xi = beg; xi <= end; xi += dt)
//	{
//		double result1;
//		gsl_bspline_calc(xi, c1, &result1, work);
//		if (result1 < minTy){
//			minTy = result1;
//			minT = xi;
//		}
//		if (result1 > maxTy){
//			maxTy = result1;
//			maxT = xi;
//		}
//	}
//	if (minmax == 0)
//		*extY = minTy;
//	else
//		*extY = maxTy;
//	return (minmax == 0) ? minT : maxT;
//}



