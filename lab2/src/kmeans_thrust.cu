
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/transform.h>
#include <thrust/sequence.h>
#include <thrust/copy.h>
#include <thrust/fill.h>
#include <thrust/replace.h>
#include <thrust/functional.h>
#include <thrust/reduce.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/iterator/discard_iterator.h>
#include "kmeans.h"
#include "defines.h"


template <typename T>
struct index_to_row : public thrust::unary_function<T,T>
{
  T C; // number of columns
  
  __host__ __device__
  index_to_row(T C) : C(C) {}

  __host__ __device__
  T operator()(T i)
  {
    return i / C;
  }
};

template <typename T>
struct square
{
    __host__ __device__
        T operator()(const T& x) const { 
            return x * x;
        }
};

template <typename T>
struct lessThanThreshold
{
    T threshold; 
  
    __host__ __device__
    lessThanThreshold(T t) : threshold(t) {}

    __host__ __device__
        bool operator()(const T& y) const { 
            printf("y: %e, sqrt(y): %e\n", y, sqrt(y));
            //return y >= threshold*threshold ? false : true;
            return (y >= threshold) ? false : true;
        }
};

template <typename T>
struct squareDiff
{
    __host__ __device__
        T operator()(const T& x, const T& y) const { 
            return (x-y) * (x-y);
        }
};

struct boolAnd
{
    __host__ __device__
        bool operator()(const bool x, const bool y) const { 
            return x&&y;
        }
};

typedef thrust::tuple<DataType, DataType> point_center_tuple;

struct point_distance : public thrust::unary_function<point_center_tuple, DataType>
{
  __host__ __device__ DataType operator()(const point_center_tuple &pc_tuple) const {
    DataType temp = thrust::get<0>(pc_tuple) - thrust::get<1>(pc_tuple);
    return temp*temp;
  }
};


struct point_idx
{
  int dim;
  int n_c;

  point_idx(int _dim, int _n) : dim(_dim), n_c(_n) {};

  __host__ __device__ int operator()(const int idx) const {
      return (idx % dim) + (dim * (idx/(dim*n_c)));    
    }
};

struct center_idx
{
  int dim;
  int n_c;

  center_idx(int _dim, int _n) : dim(_dim), n_c(_n) {};

  __host__ __device__ int operator()(const int idx) const {
    return (idx % (dim*n_c));
    }
};

template <typename T>
struct my_sqrt
{
  __host__ __device__ T operator()(const T val) const {
    return sqrt(val);
  }
};

template<typename T>
struct min_index
{
    __host__ __device__
        T operator()(const T& x, const T& y) const { 
            return (x-y) * (x-y);
        }
};


struct size_accumulator {
  int *r;
  size_accumulator(int *_r) : r(_r) {};
  
  template <typename T>
  __host__ __device__
  void operator()(T t) {
    atomicAdd(r+thrust::get<0>(t), 1);  
  }
};


struct centroid_accumulator {
  DataType *c, *p;
  int dim;
  centroid_accumulator(DataType *_c, DataType *_p, int _dim) : c(_c), p(_p), dim(_dim) {};
  
  template <typename T>
  __host__ __device__
  void operator()(const T& t) {
      //printf("val: %lf, idx: %d \n", t, thrust::get<0>(t), thrust::get<1>(t));
      for (int i = 0; i < dim; i++)
        atomicAdd(&c[thrust::get<0>(t)*dim + i], *(p + thrust::get<1>(t)*dim + i));  
    //atomicAdd(c+thrust::get<0>(t), *(p + thrust::get<1>(t)*dim + i));  
  }
};

template <typename T>
struct centroid_mean {

  __host__ __device__
  T operator()(const T& val, const int& count) {
      //printf("val: %lf count: %d\n", val, count);
      //if (count == 0) return val;
      return val/count;
  }
};

__host__ bool isConverged(thrust::host_vector<DataType>& curCentroid, thrust::host_vector<DataType>& prevCentroid, DataType threshold, int n_centroids, int dim) {
    thrust::host_vector<DataType> point_diff(n_centroids);
    //thrust::host_vector<int> point_idx(n_centroids);
    thrust::host_vector<DataType> tmp(n_centroids*dim);
    //thrust::transform(curCentroid.begin(), curCentroid.end(), prevCentroid.begin(), tmp.begin(), squareDiff<DataType>());
    
    for (int i = 0; i < curCentroid.size(); i++) {
        tmp[i] = (curCentroid[i] - prevCentroid[i]) * (curCentroid[i] - prevCentroid[i]);
    }
    /*
    thrust::reduce_by_key
    (thrust::make_transform_iterator(thrust::counting_iterator<int>(0), index_to_row<int>(dim)),
     thrust::make_transform_iterator(thrust::counting_iterator<int>(n_centroids*dim), index_to_row<int>(dim)),
     tmp.begin(),
     thrust::make_discard_iterator(),
     //point_idx.begin(),
     point_diff.begin(),
     thrust::equal_to<int>(),
     thrust::plus<DataType>());
    */
    thrust::fill(point_diff.begin(), point_diff.end(), 0);
    for (int i = 0; i < point_diff.size(); i++) {
        for (int j = 0; j < dim; j++)
            point_diff[i] +=  tmp[i*dim + j];
        point_diff[i] = sqrt(point_diff[i]);
    }

    //thrust::transform(point_diff.begin(), point_diff.end(), point_diff.begin(), my_sqrt<DataType>());
    lessThanThreshold<DataType> ltThreshold(threshold);
    
    

    for(int i = 0; i < point_diff.size(); i++)
        std::cout << "p[" << i << "] = " << point_diff[i] << std::endl;
    

    return thrust::transform_reduce(point_diff.begin(), point_diff.end(), ltThreshold, true,  boolAnd());
}


void assign_centroid(thrust::device_vector<int>& cluster_point,
                    thrust::device_vector<DataType>& points, 
                    thrust::device_vector<DataType>& centroids, 
                    int n_vals, int dim, int n_centroids) {

        thrust::device_vector<DataType> dist(n_vals*n_centroids);
          thrust::reduce_by_key(
              thrust::make_transform_iterator(thrust::make_counting_iterator<int>(0), index_to_row<int>(dim)), 
              thrust::make_transform_iterator(thrust::make_counting_iterator<int>(dim*n_vals*n_centroids), index_to_row<int>(dim)
                  ),
              thrust::make_transform_iterator(
                thrust::make_zip_iterator(
                    thrust::make_tuple(
                                  thrust::make_permutation_iterator(
                                              points.begin(), 
                                              thrust::make_transform_iterator(
                                                  thrust::make_counting_iterator<int>(0), 
                                                  point_idx(dim, n_centroids)
                                                  )
                                            ),
                                  thrust::make_permutation_iterator(
                                      centroids.begin(), 
                                      thrust::make_transform_iterator(
                                          thrust::make_counting_iterator<int>(0), 
                                          center_idx(dim, n_centroids)
                                          )
                                    )                                    
                                )
                            ), 
                            point_distance()
                        ), 
              thrust::make_discard_iterator(), 
              dist.begin()
            );
    /*
    std::cout<<"size "<< dist.size()<<std::endl;
    std::cout<<std::endl;
    for(int i = 0; i < n_vals; i++){
        for (int j = 0; j < n_centroids; j++)
            std::cout << dist[i*n_centroids+j] <<" ";
        std::cout<<std::endl;
    }
    
    std::cout<<"Centroids"<<std::endl;
    for(int i = 0; i < n_centroids; i++){
        for (int j = 0; j < dim; j++)
            std::cout << centroids[i*dim+j] <<" ";
        std::cout<<std::endl;
    }
    */

/*
    std::cout<<"Points"<<std::endl;
    for(int i = 0; i < n_centroids; i++){
        for (int j = 0; j < dim; j++)
            std::cout << points[i*dim+j] <<" ";
        std::cout<<std::endl;
    }
    */
    thrust::device_vector<DataType> minval(n_vals);
    thrust::reduce_by_key
    (thrust::make_transform_iterator(thrust::counting_iterator<int>(0), index_to_row<int>(n_centroids)),
     thrust::make_transform_iterator(thrust::counting_iterator<int>(n_vals*n_centroids), index_to_row<int>(n_centroids)),
     thrust::make_zip_iterator(
            thrust::make_tuple(
                /*thrust::make_permutation_iterator(
                dist.begin(),
                thrust::make_transform_iterator(
                    thrust::make_counting_iterator<int>(0), 
                    //thrust::placeholders::_1 % n_centroids
                    //(thrust::placeholders::_1 % n_vals) * n_centroids + thrust::placeholders::_1 / n_vals
                    //(thrust::placeholders::_1 % n_centroids) * n_vals + thrust::placeholders::_1 / n_centroids
                    //thrust::placeholders::_1 / n_centroids
                    //point_idx(1, n_centroids)
                    )
                ),*/
                dist.begin(),
                thrust::make_transform_iterator(thrust::make_counting_iterator<int>(0), 
                thrust::placeholders::_1 % n_centroids
                //center_idx(1, n_centroids)                
                )
                )
        ),
     thrust::make_discard_iterator(),
     thrust::make_zip_iterator(
                    thrust::make_tuple(
                            minval.begin(),
                            cluster_point.begin())),
     thrust::equal_to<int>(),
     thrust::minimum<thrust::tuple<DataType, int> >()
     );
    /*
     for(int i = 0; i < cluster_point.size(); i++)
        std::cout << "c[" << i << "] = " << minval[i] << " "<<  cluster_point[i] << std::endl;
        */

}


void update_centroid(thrust::device_vector<DataType>& centroids, 
                    thrust::device_vector<int>& cluster_size,
                    thrust::device_vector<int>& cluster_point,
                    thrust::device_vector<DataType>& points,
                    int n_vals, int dim, int n_centroids) {

    for(int i = 0; i < cluster_point.size(); i++)
        std::cout << "d[" << i << "] = " << cluster_point[i] << std::endl;
    /*
    thrust::transform(thrust::make_permutation_iterator(cluster_size.begin(), cluster_point.begin()), 
        thrust::make_permutation_iterator(cluster_size.begin(), cluster_point.end()), 
        thrust::make_permutation_iterator(cluster_size.begin(), cluster_point.begin()),
        add_one());
    */
   thrust::device_vector<int> indices(n_vals);
   thrust::copy(thrust::counting_iterator<int>(0),
                 thrust::counting_iterator<int>(n_vals),
                 indices.begin());
   thrust::sort_by_key(cluster_point.begin(), cluster_point.end(), indices.begin());
   thrust::reduce_by_key(cluster_point.begin(), cluster_point.end(), thrust::constant_iterator<int>(1), thrust::make_discard_iterator(),
   cluster_size.begin());

   //thrust::sort_by_key(indices.begin(), indices.end(), cluster_size.begin());
    //thrust::for_each_n(thrust::make_zip_iterator(thrust::make_tuple(d_a.begin(), d_b.begin())), size_a, my_func(thrust::raw_pointer_cast(d_r.data())));
    //size_accumulator s_a(thrust::raw_pointer_cast(cluster_size.data()));
    //thrust::for_each_n(thrust::make_zip_iterator(thrust::make_tuple(cluster_point.begin(), thrust::constant_iterator<int>(1))), 
    //            n_vals, s_a);
    /*
    for (int i = 0; i < cluster_point.size(); i++) {
        cluster_size[cluster_point[i]]++; 
    }
    */
   /*
   for(int i = 0; i < cluster_size.size(); i++)
        std::cout << "d[" << i << "] = " << cluster_size[i] << std::endl;


    thrust::reduce_by_key(thrust::make_transform_iterator(thrust::counting_iterator<int>(0), index_to_row<int>(n_centroids)),
                        thrust::make_transform_iterator(thrust::counting_iterator<int>(n_vals*n_centroids), index_to_row<int>(n_centroids)),
        cluster_point.begin(), cluster_point.end(), 
        indices.begin(), 
        thrust::make_discard_iterator(),
        centroids.begin(), thrust::equal<int>(), );    
       */ 
        
    centroid_accumulator c_a(thrust::raw_pointer_cast(centroids.data()), thrust::raw_pointer_cast(points.data()), dim);
    thrust::for_each_n(thrust::make_zip_iterator(thrust::make_tuple(cluster_point.begin(), thrust::make_counting_iterator<int>(0))), 
                n_vals, c_a);
    
    /*
    for (int i = 0; i < points.size()/dim; i++) {
        for (int j = 0; j < dim; j++) {
            //printf("%d, %d\n", i, j);
            centroids[cluster_point[i]*dim + j] += points[i*dim + j];
        }
    }
    */
    thrust::transform(centroids.begin(), centroids.end(), 
        thrust::make_permutation_iterator(
                cluster_size.begin(), 
                thrust::make_transform_iterator(thrust::make_counting_iterator<int>(0), point_idx(1, dim))
            ), 
        centroids.begin(), centroid_mean<DataType>());
    
    
    /*
    for(int i = 0; i < cluster_size.size(); i++)
        std::cout << "d[" << i << "] = " << cluster_size[i] << std::endl;
    */
    
}


void kmean_thrust(kmean_args_t* args, double* timeMeasure) {
    int n_vals = args->n_vals;
    int n_centroids = args->num_cluster;
    int dims = args->dims;
    args->iter_converge = 0;
    cudaEvent_t start, stop, assignStart, assignEnd, updateStart, updateEnd;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventCreate(&assignStart);
    cudaEventCreate(&assignEnd);
    cudaEventCreate(&updateStart);
    cudaEventCreate(&updateEnd);
    cudaEventRecord(start);

    thrust::device_vector<DataType> points(args->input_vals, args->input_vals+n_vals*dims);
    thrust::device_vector<DataType> centroids(args->centroids, args->centroids+n_centroids*dims);
    thrust::device_vector<int> cluster_point(args->clusterId_of_point, args->clusterId_of_point+n_vals);
    thrust::device_vector<int> cluster_size(n_centroids);

    thrust::host_vector<DataType> oldCentroid(n_centroids*dims);
    //thrust::host_vector<DataType> curCentroids(args->centroids, args->centroids+n_centroids*dims);
    
    //thrust::host_vector<int> host_cluster_point(args->clusterId_of_point, args->clusterId_of_point+n_vals);
    thrust::host_vector<DataType> h_centroids(centroids);
    
    while (args->iter_converge < args->max_num_iter && !isConverged(h_centroids, oldCentroid, args->threshold, n_centroids, dims)) {
        args->iter_converge++;
        thrust::copy(h_centroids.begin(), h_centroids.end(), oldCentroid.begin());
        //copyCentroids(tmpCentroid, args->centroids, args->num_cluster, args->dims);
        //memcpy(tmpCentroid, args->centroids, args->n_vals*args->dims);
        cudaEventRecord(assignStart);

        assign_centroid(cluster_point, points, centroids, n_vals, dims, n_centroids);

        cudaEventRecord(assignEnd);
        cudaEventSynchronize(assignEnd);
        float tmp1 = 0;
        cudaEventElapsedTime(&tmp1, assignStart, assignEnd);
        timeMeasure[0] += tmp1;

        
        //std::cout<<args->iter_converge<<std::endl;

        thrust::fill(centroids.begin(), centroids.end(), 0.0);
        thrust::fill(cluster_size.begin(), cluster_size.end(), 0);
        
        cudaEventRecord(updateStart);
        update_centroid(centroids, cluster_size, cluster_point, points, n_vals, dims, n_centroids);

        cudaEventRecord(updateEnd);
        cudaEventSynchronize(updateEnd);
        float tmp2 = 0;
        cudaEventElapsedTime(&tmp2, updateStart, updateEnd);
        timeMeasure[1] += tmp2;
        //curCentroids = centroids;
        /*
        std::cout<<"Centroids"<<std::endl;
        for(int i = 0; i < n_centroids; i++){
            for (int j = 0; j < dims; j++)
                std::cout << centroids[i*dims+j] <<" ";
            std::cout<<std::endl;
            
        }
        */
        h_centroids = centroids;
    }
    
    
    thrust::host_vector<int> h_cluster_point(cluster_point);
    /*
    for (int clusterId = 0; clusterId < args->num_cluster; clusterId++){
            printf("%d ", clusterId);
            for (int d = 0; d < args->dims; d++)
                printf("%f ", h_centroids[clusterId*args->dims + d]);
        printf("\n");
        }
    */
    //free(args->centroids);
    //free(args->clusterId_of_point);
    for (int i = 0; i < h_centroids.size(); i++){
        args->centroids[i] = h_centroids[i];
    }
    for (int i = 0; i < h_cluster_point.size(); i++){
        args->clusterId_of_point[i] = h_cluster_point[i];
    }
    //args->centroids = thrust::raw_pointer_cast(h_centroids.data());
    //args->clusterId_of_point = thrust::raw_pointer_cast(h_cluster_point.data());

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float tmp3 = 0;
    cudaEventElapsedTime(&tmp3, start, stop);
    timeMeasure[2] += tmp3;
    timeMeasure[3] = timeMeasure[2] - timeMeasure[1] - timeMeasure[0];
}
