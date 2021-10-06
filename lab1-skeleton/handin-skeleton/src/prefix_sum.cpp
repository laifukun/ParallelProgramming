#include "prefix_sum.h"
#include "helpers.h"
#include <iostream>
#include "selection.h"
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
    

    // non-work efficient
#if HS_ALGORITHM == 1
    int stride = 1;
    while (stride <= n) {
        for (int j = tid; j < n; j += p) {
            if (j >= stride) {
                args->output_vals[j] = args->op(args->input_vals[j-stride], args->input_vals[j], args->n_loops);
            } else {
                args->output_vals[j] = args->input_vals[j];
            }
            
        }
        stride *= 2;

    #ifdef SPIN_BARRIER
        int local = 0;
        spin_barrier_wait(args->barrier, &local);
    #else
        pthread_barrier_wait(args->barrier);
    #endif

        for (int j = tid; j < n; j += p) {
            args->input_vals[j] = args->output_vals[j];
        }
        
    #ifdef SPIN_BARRIER
        spin_barrier_wait(args->barrier, &local);
    #else
        pthread_barrier_wait(args->barrier);
    #endif
    }    

#elif TWOLEVEL_ALGORITHM == 1
   
    int dn = n/(p+1);    
    int last = (tid+1)*dn - 1;
    args->output_vals[tid * dn] = args->input_vals[tid * dn];
    
    for (int j = tid * dn + 1; j < (tid + 1)*dn; j++) {       
       args->output_vals[j] = args->op(args->output_vals[j-1], args->input_vals[j], args->n_loops);
    }
    args->input_vals[last] = args->output_vals[last];

#ifdef SPIN_BARRIER
    int local = 0;
    spin_barrier_wait(args->barrier, &local);
#else
   pthread_barrier_wait(args->barrier);
#endif

    if (args->n_threads > 100) {
        int stride = dn;

        while (stride <= n) {
            if (last >= stride) {
                args->output_vals[last] = op(args->input_vals[last], args->input_vals[last - stride], args->n_loops);
            } 
            else {
                args->output_vals[last] = args->input_vals[last];
            }
            
        #ifdef SPIN_BARRIER
            spin_barrier_wait(args->barrier, &local);
        #else
            pthread_barrier_wait(args->barrier);
        #endif
            args->input_vals[last] = args->output_vals[last];
            stride *= 2;
            
        #ifdef SPIN_BARRIER
            spin_barrier_wait(args->barrier, &local);
        #else
            pthread_barrier_wait(args->barrier);
        #endif

        }
    
    } else if (tid == 0) {
    
        int x = 0;
        for (int i = 1; i <= p; i++) {
            x = args->op(x, args->output_vals[i*dn- 1], args->n_loops);
            args->output_vals[i*dn - 1] = x;
        }
    
    }
#ifdef SPIN_BARRIER
    spin_barrier_wait(args->barrier, &local);
#else
    pthread_barrier_wait(args->barrier);
#endif

    int i = tid+1;
    for (int j = i * dn; j < (i == p ? n : (i + 1)*dn - 1); j++) {
        args->output_vals[j] = args->op(args->output_vals[j-1],args->input_vals[j], args->n_loops);
    }
    
#elif BLELLOCH_ALGORITHM == 1

    int n2 = next_power_of_two(n);
    int stride = 1;
    while (stride <= n2) {
       if (tid < n2/2/stride) {
            for (int j = (tid+1)*2*stride-1; j < n2; j += 2*stride*p) {
                args->output_vals[j] = args->op(args->output_vals[j-stride], args->output_vals[j], args->n_loops);
            }
       }
        stride *= 2;

    #ifdef SPIN_BARRIER
        int local = 0;
        spin_barrier_wait(args->barrier, &local);
    #else
        pthread_barrier_wait(args->barrier);
    #endif
   }

    stride = n2/2;
    
    if (tid == 0)
        args->output_vals[n2-1] = 0;

    #ifdef SPIN_BARRIER
        int local = 0;
        spin_barrier_wait(args->barrier, &local);
    #else
        pthread_barrier_wait(args->barrier);
    #endif

    while (stride > 0) {
        if (tid < n2/2/stride) {
            for (int j = (tid+1)*2*stride-1; j < n2; j += 2*stride*p) {
                int tmp = args->output_vals[j];
                args->output_vals[j] = args->op(args->output_vals[j], args->output_vals[j-stride], args->n_loops);
                args->output_vals[j-stride] = tmp;
            }
        }
        stride /= 2;
    #ifdef SPIN_BARRIER
        spin_barrier_wait(args->barrier, &local);
    #else
        pthread_barrier_wait(args->barrier);
    #endif

    }


#endif
    return 0;
}
