#include "utils.h"
#include <math.h>
#include <float.h>

bool isConverged(double** curCentriod, double** prevCentriod, double threshold, int n, int dim) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < dim; j++){
            if (fabs(curCentriod[i][j] - prevCentriod[i][j]) > threshold) return false;
        }
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
            double dist = distance(centroids[i], points[i], args->dims);
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

    for (int i = 0; i < n_centroid; i++)
        for (int j = 0; j < args->dims; j++) {
            centroids[i][j] = 0.0;
        }
    
    for (int i = 0; i < n_vals; i++) {
        for (int j = 0; j < args->dims; j++) {
            centroids[clusterIds[i]][j] += points[i][j];
            points_centroid[clusterIds[i]]++;
        }
    }

    for (int i = 0; i < n_centroid; i++) {
        if (points_centroid[i] == 0) {
            for (int j = 0; j < args->dims; j++) {
                centroids[i][j] = rand();
            }
        } else {
            for (int j = 0; j < args->dims; j++) {
                centroids[i][j] /= points_centroid[i];
            }
        }
    }
    free(points_centroid);

}




double distance(double* v1, double* v2, int d) {
    double dist = 0.0;
    for (int i = 0; i < d; i++) {
        dist += (v1[i] - v2[i])*(v1[i] - v2[i]);
    }
    return sqrt(dist);
}