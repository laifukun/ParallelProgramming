#include <io.h>
//#include "helpers.h"
#include "pre_kmean.h"

void read_file(struct options_t* args,
               int*              n_vals,
               double***             input_vals,
               double***             output_vals,
               int**     point_cluster) {

  	// Open file
	std::ifstream in;
	in.open(args->in_file);
	// Get num vals
	in >> *n_vals;

	int n_alloc = *n_vals;

	// Alloc input and output arrays
	*point_cluster = (int*) malloc(n_alloc * sizeof(int));
	*output_vals = (double**) malloc(args->num_cluster * sizeof(double*));
    for (int i = 0; i < args->num_cluster; i++) {
        (*output_vals)[i] = (double*)malloc(args->dims * sizeof(double));
    }

    *input_vals = (double**) malloc(n_alloc*sizeof(double*));
    for (int i = 0; i < n_alloc; i++) {
        (*input_vals)[i] = (double*)malloc(args->dims * sizeof(double));
    }

	// Read input vals
    int id = 0;
	for (int i = 0; i < n_alloc; ++i) {
        in >> id;
        for (int j = 0; j < args->dims; j++)
		    in >> (*input_vals)[i][j];
		//(*output_vals)[i] = (*input_vals)[i];
		
	}

}

void write_file(options_t*         opts,
                kmean_args_t* args) {
  // Open file
	//std::ofstream out;
	//out.open(args->out_file, std::ofstream::trunc);

	// Write solution to output file
    if (opts->output_centroids) {
        printf("clusters:");
        for (int p=0; p < args->n_vals; p++)
            printf(" %d", args->clusterId_of_point[p]);
    } else {
        for (int clusterId = 0; clusterId < args->num_cluster; clusterId++){
            printf("%d ", clusterId);
            for (int d = 0; d < args->dims; d++)
                printf("%lf ", args->centroids[clusterId][d]);
        printf("\n");
        }
    }

	// Free memory
	//free(opts->input_vals);
	//free(opts->output_vals);
}

