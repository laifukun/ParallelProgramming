#ifndef _KMEAN_SEQ_
#define _KMEAN_SEQ_

#include "pre_kmean.h"
void kmean_seq(kmean_args_t* args);

bool isConverged(double** curCentriod, double** prevCentriod, double threshold, int n, int dim);

void assignCentroid(kmean_args_t* args);

void updateCentroid(kmean_args_t* args);
#endif