#include "kmeans.h"
#include "pre_kmean.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "defines.h"
#include <chrono>

static DataType distance(DataType* v1, DataType* v2, int d) {
    DataType dist = 0.0;
    for (int i = 0; i < d; i++) {
        dist += (v1[i] - v2[i])*(v1[i] - v2[i]);
    }
    return sqrt(dist);
}


static void copyCentroids(DataType* oldCentroids, DataType* newCentroid, int n, int d) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < d; j++) {
            oldCentroids[i*d + j] = newCentroid[i*d + j];
    }
}

static bool isConverged(DataType* curCentriod, DataType* prevCentriod, DataType threshold, int n, int dim) {
    for (int i = 0; i < n; i++)
        //for (int j = 0; j < dim; j++){
        if (distance(curCentriod + i*dim, prevCentriod + i*dim, dim) >= threshold) return false;
        //}
    return true;
}

static void assignCentroid(kmean_args_t* args) {
    DataType* points = args->input_vals;
    DataType* centroids = args->centroids;
    int* clusterIds = args->clusterId_of_point;
    int n_vals = args->n_vals;
    int n_centroid = args->num_cluster;
    int dim = args->dims;

    for (int i = 0; i < n_vals; i++) {
        DataType minDist = DBL_MAX;
        for (int j = 0; j < n_centroid; j++) {
            DataType dist = distance(centroids + j*dim, points + i*dim, args->dims);
            
            if ( dist < minDist) {
                minDist = dist;
                clusterIds[i] = j;
            }            
        }
    }
}

static void updateCentroid(kmean_args_t* args) {
    DataType* points = args->input_vals;
    DataType* centroids = args->centroids;
    int* clusterIds = args->clusterId_of_point;
    int n_vals = args->n_vals;
    int n_centroid = args->num_cluster;
    int* points_centroid = (int*)malloc(n_centroid*sizeof(int));
    int dim = args->dims;

    for (int i = 0; i < n_centroid; i++) {
        points_centroid[i] = 0;
    }
    for (int i = 0; i < n_centroid; i++)
        for (int j = 0; j < args->dims; j++) {
            centroids[i*dim + j] = 0.0;
        }
    
    for (int i = 0; i < n_vals; i++) {
        for (int j = 0; j < args->dims; j++) {
            centroids[clusterIds[i]*dim + j] += points[i*dim + j];            
        }
        points_centroid[clusterIds[i]]++;
        //std::cout<<points_centroid[clusterIds[i]]<<std::endl;
    }

    for (int i = 0; i < n_centroid; i++) {
        if (points_centroid[i] == 0) {
            int id = kmeans_rand() % args->n_vals;
            for (int j = 0; j < args->dims; j++) {
                centroids[i*dim + j] = points[id*dim + j];
            }
        } else {
            for (int j = 0; j < args->dims; j++) {
                centroids[i*dim + j] /= points_centroid[i];
            }
        }
    }
    free(points_centroid);

}


void kmean_seq(kmean_args_t* args, double* timeMeasure) {

    //initilize_centroid(args);
    args->iter_converge = 0;
    auto start = std::chrono::high_resolution_clock::now();

    DataType* tmpCentroid = (DataType*) malloc(args->num_cluster * args->dims * sizeof(DataType));
    
    while (args->iter_converge < args->max_num_iter && !isConverged(args->centroids, tmpCentroid, args->threshold, args->num_cluster, args->dims)) {
        args->iter_converge++;
        
        copyCentroids(tmpCentroid, args->centroids, args->num_cluster, args->dims);
        //memcpy(tmpCentroid, args->centroids, args->n_vals*args->dims);
        auto startAssign = std::chrono::high_resolution_clock::now();
        assignCentroid(args);
        auto endAssign = std::chrono::high_resolution_clock::now();
        timeMeasure[0] += std::chrono::duration<double, std::milli>(endAssign - startAssign).count();

        auto startUpdate = std::chrono::high_resolution_clock::now();
        updateCentroid(args);
        auto endUpdate = std::chrono::high_resolution_clock::now();
        timeMeasure[1] += std::chrono::duration<double, std::milli>(endUpdate - startUpdate).count();
        //if (args->iter_converge == 1) break;
    }

    free(tmpCentroid);
    auto end = std::chrono::high_resolution_clock::now();
    //auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    timeMeasure[2] = std::chrono::duration<double, std::milli>(end - start).count();
    timeMeasure[3] = timeMeasure[2] - timeMeasure[1] - timeMeasure[0];
}
