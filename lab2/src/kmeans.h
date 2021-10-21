#ifndef _KMEAN_SEQ_
#define _KMEAN_SEQ_

#include "pre_kmean.h"
void kmean_seq(kmean_args_t* args, double* timeMeasure);
void kmean_thrust(kmean_args_t* args, double* timeMeasure);
void kmean_cuda(kmean_args_t* args, double* timeMeasure, int algo);
#endif