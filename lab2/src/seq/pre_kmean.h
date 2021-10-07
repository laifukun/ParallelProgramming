#pragma once
#include <stdlib.h>
#include "argparse.h"

struct kmean_args_t {
  double**           input_vals;
  double**           centroids;
  int*              clusterId_of_point;
  int                n_vals;
  int                dims;
  int                max_num_iter;
  int               iter_converge;
  int               threshold;
  int               seed;
  int               num_cluster;
};

//kmean_args_t* alloc_args(int n_threads);

int next_power_of_two(int x);

void fill_args(kmean_args_t *args,
               options_t* option,
               double** inputs,
               int n_vals,
               double** centroids,
               int* clusterIds
               );

void initilize_centroid(kmean_args_t* args);
