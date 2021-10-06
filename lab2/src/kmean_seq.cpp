#include "kmean_seq.h"
#include "pre_kmean.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

void kmean_seq(kmean_args_t* args) {

    initilize_centroid(args);
    args->iter_converge = 0;

    double** tmpCentroid = (double**) malloc(args->num_cluster * sizeof(double*));
    for (int i = 0; i < args->num_cluster; i++) {
        tmpCentroid[i] = (double*)malloc(args->dims * sizeof(double));
    }
    while (args->iter_converge < args->max_num_iter && !isConverged(args->centroids, tmpCentroid, args->threshold, args->n_vals, args->dims)) {
        args->iter_converge++;
        memcpy(tmpCentroid, args->centroids, args->n_vals*args->dims);
        assignCentroid(args);
        updateCentroid(args);
    }
}