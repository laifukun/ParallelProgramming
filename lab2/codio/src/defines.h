#ifndef _DEFINES_H_
#define _DEFINES_H_

#define DataType double

struct options_t {
    int num_cluster;
    int dims;    
    char *in_file;
    int max_num_iter;
    DataType threshold;
    bool output_centroids;
    int seed;
    char *algo;
};

struct kmean_args_t {
  DataType*           input_vals;
  DataType*           centroids;
  int*              clusterId_of_point;
  int                n_vals;
  int                dims;
  int                max_num_iter;
  int               iter_converge;
  DataType           threshold;
  int               num_cluster;
};

#endif