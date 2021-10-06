#include "pre_kmean.h"
#include <math.h>
#include <stdlib.h>

void fill_args(kmean_args_t *args,
               options_t* option,
               double** inputs,
               int n_vals,
               double** centroids,
               int* clusterId_of_point
               ) {
    args = (kmean_args_t*)malloc(sizeof(kmean_args_t));
    args->dims = option->dims;
    args->input_vals = inputs;
    args->max_num_iter = option->max_num_iter;
    args->n_vals = n_vals;
    args->num_cluster = option->num_cluster;
    args->seed = option->seed;
    args->threshold = option->threshold;
    args->centroids = centroids;
    args->clusterId_of_point = clusterId_of_point;
    args->iter_converge = 0;
}

void initilize_centroid(kmean_args_t* args){
    //srand(args->seed);
    
    for (int i = 0; i < args->num_cluster; i++)
        for (int j = 0; j < args->dims; j++) {
            args->centroids[i][j] = rand();
        }
}