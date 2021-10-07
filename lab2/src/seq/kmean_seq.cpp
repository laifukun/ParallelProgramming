#include "kmean_seq.h"
#include "pre_kmean.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>

void kmean_seq(kmean_args_t* args) {

    initilize_centroid(args);
    args->iter_converge = 0;

    double** tmpCentroid = (double**) malloc(args->num_cluster * sizeof(double*));
    for (int i = 0; i < args->num_cluster; i++) {
        tmpCentroid[i] = (double*)malloc(args->dims * sizeof(double));
    }
    while (args->iter_converge < args->max_num_iter && !isConverged(args->centroids, tmpCentroid, args->threshold, args->num_cluster, args->dims)) {
        args->iter_converge++;
        
        copyCentroids(tmpCentroid, args->centroids, args->num_cluster, args->dims);
        //memcpy(tmpCentroid, args->centroids, args->n_vals*args->dims);
        assignCentroid(args);
        //std::cout<<args->iter_converge<<std::endl;

        updateCentroid(args);
        //if (args->iter_converge == 1) break;
    }
}


bool isConverged(double** curCentriod, double** prevCentriod, double threshold, int n, int dim) {
    for (int i = 0; i < n; i++)
        //for (int j = 0; j < dim; j++){
        if (distance(curCentriod[i], prevCentriod[i], dim) > threshold) return false;
        //}
    return true;
}

void assignCentroid(kmean_args_t* args) {
    double** points = args->input_vals;
    double** centroids = args->centroids;
    int* clusterIds = args->clusterId_of_point;
    int n_vals = args->n_vals;
    int n_centroid = args->num_cluster;

    for (int i = 0; i < n_vals; i++) {
        double minDist = DBL_MAX;
        for (int j = 0; j < n_centroid; j++) {
            double dist = distance(centroids[j], points[i], args->dims);
            
            if ( dist < minDist) {
                minDist = dist;
                clusterIds[i] = j;
            }            
        }
    }
}

void updateCentroid(kmean_args_t* args) {
    double** points = args->input_vals;
    double** centroids = args->centroids;
    int* clusterIds = args->clusterId_of_point;
    int n_vals = args->n_vals;
    int n_centroid = args->num_cluster;
    int* points_centroid = (int*)malloc(n_centroid*sizeof(int));

    for (int i = 0; i < n_centroid; i++) {
        points_centroid[i] = 0;
    }
    for (int i = 0; i < n_centroid; i++)
        for (int j = 0; j < args->dims; j++) {
            centroids[i][j] = 0.0;
        }
    
    for (int i = 0; i < n_vals; i++) {
        for (int j = 0; j < args->dims; j++) {
            centroids[clusterIds[i]][j] += points[i][j];            
        }
        points_centroid[clusterIds[i]]++;
        //std::cout<<points_centroid[clusterIds[i]]<<std::endl;
    }

    for (int i = 0; i < n_centroid; i++) {
        if (points_centroid[i] == 0) {
            int id = kmeans_rand() % args->n_vals;
            for (int j = 0; j < args->dims; j++) {
                centroids[i][j] = points[id][j];
            }
        } else {
            for (int j = 0; j < args->dims; j++) {
                centroids[i][j] /= points_centroid[i];
            }
        }
    }
    free(points_centroid);

}
