#pragma once
#include <stdlib.h>
#include "argparse.h"
#include "defines.h"


void fill_args(kmean_args_t *args,
               options_t* option,
               DataType* inputs,
               int n_vals,
               DataType* centroids,
               int* clusterIds
               );

void initilize_centroid(kmean_args_t* args);
