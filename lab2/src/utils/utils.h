#ifndef _UTILS_H_
#define _UTILS_H_

#include "pre_kmean.h"



double distance(double* v1, double* v2, int d);

void copyCentroids(double** oldCentroids, double** newCentroid, int n, int d);

int kmeans_rand();
void kmeans_srand(unsigned int seed);
#endif