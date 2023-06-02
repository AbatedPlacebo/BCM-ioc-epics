#include "BCMMath.h"

double calcQ(int* arr, int size, int wnd1, int wnd2, double QK, int gain, double gainK){
	if (wnd1 > wnd2){
		int tmp = wnd1;
		wnd1 = wnd2;
		wnd2 = tmp;
	}
	double integral = 0.0;
	int i;
	for (i = wnd1; i < wnd2; i++){
		integral += fabs(arr[i]) * 320 * pow(10,-6); 
	}
	return QK * pow(10.0, -gain * gainK / 20.0) * integral;
}

double timeQ(int* arr, int size, int wnd1, int wnd2, int minmax){
//	gsl_interp_accel *acc = gsl_interp_accel_alloc();
//	gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, 2);
	if (wnd1 > wnd2){
		int tmp = wnd1;
		wnd1 = wnd2;
		wnd2 = tmp;
	}
	// j - num of maxs
	int i;
	int extY = 0;
	double extT = 0;
	for (i = wnd1; (i < wnd2 - 1); i++){
		 int diff = arr[i+1] - arr[i];
		 if (minmax == 0)
		 {
			 if (diff > 0 && arr[i] < extY){
					extT = i;
					extY = arr[i];
			}
		 }
		 else if (diff < 0 && arr[i] > extY){
				extT = i;
				extY = arr[i];
		}
	}
	return extT * 320.0 / 65536.0;
}
