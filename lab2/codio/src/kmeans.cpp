
#include <iostream>
#include "argparse.h"
#include "io.h"

#include <cstring>
#include "pre_kmean.h"
#include "utils.h"
#include "kmeans.h"


using namespace std;

int main(int argc, char **argv)
{
    
    // Parse args
    struct options_t opts;
    get_opts(argc, argv, &opts);

    //bool sequential = true;

    int n_vals;
    DataType *data_set, *centroids;
    int* clusterId_of_point;
    read_file(&opts, &n_vals, &data_set, &centroids, &clusterId_of_point);

    kmean_args_t* kmean_args = (kmean_args_t*)malloc(sizeof(kmean_args_t));;

    fill_args(kmean_args, &opts, data_set, 
        n_vals, centroids, clusterId_of_point);

    kmeans_srand(opts.seed);
    // Start timer
    initilize_centroid(kmean_args);
    double timeMeasure[4] = {0.0,0.0,0.0,0.0}; 
    if (strcmp(opts.algo, "seq") == 0)  {
        kmean_seq(kmean_args, timeMeasure);
    } else if (strcmp(opts.algo, "thrust") == 0) {
        kmean_thrust(kmean_args, timeMeasure);
    } else if (strcmp(opts.algo, "cuda") == 0) {
        kmean_cuda(kmean_args, timeMeasure, 0);
    } else if (strcmp(opts.algo, "cuda-shmem") == 0) {
        kmean_cuda(kmean_args, timeMeasure, 1);
    }


    //End timer and print out elapsed
    
    //std::cout << "time: " << diff.count() << std::endl;

    printf("%d,%lf\n", kmean_args->iter_converge, timeMeasure[2]/kmean_args->iter_converge);
    //printf("Assign Centroids: %lf\n", timeMeasure[0]/kmean_args->iter_converge);
    //printf("Update Centroids: %lf\n", timeMeasure[1]/kmean_args->iter_converge);
    //printf("Others: %lf\n", timeMeasure[3]);
    // Write output data
    write_file(&opts, kmean_args);

    // Free other buffers
    //free(&kmean_args);
    free(data_set);
    free(centroids);
    free(clusterId_of_point);
}
