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
		integral += fabs(arr[i]); 
	}
	return QK * pow(10.0, -gain * gainK / 20.0) * integral;

}

int timeQ(int* arr, int size){
	// j - num of maxs
	int i, j;
	int maxY = 0;
	int maxT = 0;
	for (i = 0; (i < (size - 1)); i++){
		 int diff = arr[i+1] - arr[i];
		 if (diff < 0){
		 	if (arr[i] > maxY){
				maxT = i;
				maxY = arr[i];
			}
		}
	}
	return maxT;
}
