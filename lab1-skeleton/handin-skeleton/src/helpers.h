#pragma once
#include "selection.h"
#include "operators.h"
#include <stdlib.h>
#include <pthread.h>
#include <spin_barrier.h>

struct prefix_sum_args_t {
  int*               input_vals;
  int*               output_vals;
  bool               spin;
  int                n_vals;
  int                n_threads;
  int                t_id;
  int (*op)(int, int, int);
  int n_loops;

#ifdef SPIN_BARRIER
  spin_barrier* barrier;
#else
  pthread_barrier_t* barrier;
#endif
};

prefix_sum_args_t* alloc_args(int n_threads);

int next_power_of_two(int x);

void fill_args(prefix_sum_args_t *args,
               int n_threads,
               int n_vals,
               int *inputs,
               int *outputs,
               bool spin,
               int (*op)(int, int, int),
               int n_loops);
