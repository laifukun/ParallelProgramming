
#include <iostream>
#include <argparse.h>
#include "threads.h"
#include <io.h>
#include <chrono>
#include <cstring>
#include "pre_kmean.h"
#include "utils.h"
#include "kmean_seq.h"


using namespace std;

int main(int argc, char **argv)
{
    
    // Parse args
    struct options_t opts;
    get_opts(argc, argv, &opts);
    
    bool sequential = true;

    int n_vals;
    double **input_vals, **centroids;
    int* clusterId_of_point;
    read_file(&opts, &n_vals, &input_vals, &centroids, &clusterId_of_point);

    kmean_args_t* kmean_args = (kmean_args_t*)malloc(sizeof(kmean_args_t));;

    fill_args(kmean_args, &opts, input_vals, 
        n_vals, centroids, clusterId_of_point);

    kmeans_srand(opts.seed);
    //cout<<"n_vals "<<n_vals<<endl;
    // Start timer
    auto start = std::chrono::high_resolution_clock::now();
 
    if (sequential)  {
        kmean_seq(kmean_args);
    }

    //End timer and print out elapsed
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    //std::cout << "time: " << diff.count() << std::endl;

    printf("%d,%lf\n", kmean_args->iter_converge, (double)diff.count()/kmean_args->iter_converge);
    // Write output data
    write_file(&opts, kmean_args);

    // Free other buffers
    //free(&kmean_args);
    free(input_vals);
    free(centroids);
    free(clusterId_of_point);
}
