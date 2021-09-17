#include "prefix_sum.h"
#include "helpers.h"
#include <iostream>

using namespace std;
void* compute_prefix_sum(void *a)
{
    prefix_sum_args_t *args = (prefix_sum_args_t *)a;

    /************************
     * Your code here...    *
     * or wherever you like *
     ************************/
    int n = args->n_vals; // number of elements
    int p = args->n_threads; // number of threads/process;
    int tid = args->t_id; // current thread Id
    int stride = 1;
    
    
    while (stride <= n) {
        for (int j = tid * n/(p); j < (tid + 1)*n/(p); j++) {
            if (j >= stride) {
                args->output_vals[j] = args->input_vals[j - stride] + args->input_vals[j];
            } else {
                args->output_vals[j] = args->input_vals[j];
            }
            
        }
        stride *= 2;

        pthread_barrier_wait(args->barrier);

        //args->input_vals = args->output_vals;
        //args->output_vals 
        for (int j = tid * n/(p); j < (tid + 1)*n/(p); j++) {
            args->input_vals[j] = args->output_vals[j];
        }
        pthread_barrier_wait(args->barrier);
    }    
    

   /*
    while(stride <= n/2) {
        int id = (tid + 1)*stride*2-1;
        if (id < n) {
            args->output_vals[id] += args->output_vals[id - stride];
        }
        stride *= 2;
        pthread_barrier_wait(args->barrier);
    }

    stride = n/4;
    while (stride > 0) {
        int id = (tid + 1)*stride*2-1;
        if ((id + stride) < n) {
            args->output_vals[id + stride] += args->output_vals[id];
        }
        stride /= 2;
        pthread_barrier_wait(args->barrier);
    }

*/
    return 0;
}
