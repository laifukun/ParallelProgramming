#include "io.h"
#include "pre_kmean.h"

void read_file(struct options_t* args,
               int*              n_vals,
               DataType**             input_vals,
               DataType**             output_vals,
               int**     clusterId_of_point) {

  	// Open file
	std::ifstream in;
	in.open(args->in_file);
	// Get num vals
	in >> *n_vals;
	int n_alloc = *n_vals;

	// Alloc input and output arrays
	*clusterId_of_point = (int*) malloc(n_alloc * sizeof(int));

    *output_vals = (DataType*) malloc(args->num_cluster * args->dims * sizeof(DataType));

    /*
	*output_vals = (DataType**) malloc(args->num_cluster * sizeof(DataType*));
    for (int i = 0; i < args->num_cluster; i++) {
        (*output_vals)[i] = (DataType*)malloc(args->dims * sizeof(DataType));
    }
    */
   *input_vals = (DataType*) malloc(n_alloc* args->dims*sizeof(DataType));
   /*
    *input_vals = (DataType**) malloc(n_alloc*sizeof(DataType*));
    for (int i = 0; i < n_alloc; i++) {
        (*input_vals)[i] = (DataType*)malloc(args->dims * sizeof(DataType));
    }
*/
	// Read input vals
    int id = 0;
	for (int i = 0; i < n_alloc; ++i) {
        in >> id;
        for (int j = 0; j < args->dims; j++){
            in >> (*input_vals)[i * args->dims + j];
        }
		//    in >> (*input_vals)[i][j];
		//(*output_vals)[i] = (*input_vals)[i];
		
	}

}

void write_file(options_t*         opts,
                kmean_args_t* args) {
  // Open file
	//std::ofstream out;
	//out.open(args->out_file, std::ofstream::trunc);

	// Write solution to output file
    if (!opts->output_centroids) {
        printf("clusters:");
        for (int p=0; p < args->n_vals; p++)
            printf(" %d", args->clusterId_of_point[p]);
    } else {
        for (int clusterId = 0; clusterId < args->num_cluster; clusterId++){
            printf("%d ", clusterId);
            for (int d = 0; d < args->dims; d++)
                printf("%lf ", args->centroids[clusterId*args->dims + d]);
        printf("\n");
        }
    }

	// Free memory
	//free(opts->input_vals);
	//free(opts->output_vals);
}

