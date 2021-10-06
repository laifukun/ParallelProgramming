#ifndef _UTILS_H_
#define _UTILS_H_

#include "pre_kmean.h"

bool isConverged(double** curCentriod, double** prevCentriod, double threshold, int n, int dim);

void assignCentroid(kmean_args_t* args);

void updateCentroid(kmean_args_t* args);

double distance(double* v1, double* v2, int d);

#endif