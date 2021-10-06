#include <io.h>
#include "helpers.h"
#include "search.h"

void read_file(struct options_t* args,
               int*              n_vals,
               int**             input_vals,
               int**             output_vals) {

  	// Open file
	std::ifstream in;
	in.open(args->in_file);
	// Get num vals
	in >> *n_vals;

	int n_alloc = *n_vals;
#if BLELLOCH_ALGORITHM == 1
	n_alloc = next_power_of_two(*n_vals);
#endif
	// Alloc input and output arrays
	*input_vals = (int*) malloc(n_alloc * sizeof(int));
	*output_vals = (int*) malloc(n_alloc * sizeof(int));

	// Read input vals
	for (int i = 0; i < n_alloc; ++i) {
		if (i < *n_vals) {
			in >> (*input_vals)[i];
			(*output_vals)[i] = (*input_vals)[i];
		} else {
			(*input_vals)[i] = 0;
			(*output_vals)[i] = 0;
		}
		
	}

}

void write_file(struct options_t*         args,
               	struct prefix_sum_args_t* opts) {
  // Open file
	std::ofstream out;
	out.open(args->out_file, std::ofstream::trunc);

	// Write solution to output file
	for (int i = 0; i < opts->n_vals; ++i) {
		out << opts->output_vals[i] << std::endl;
	}

	out.flush();
	out.close();
	
	// Free memory
	free(opts->input_vals);
	free(opts->output_vals);
}
