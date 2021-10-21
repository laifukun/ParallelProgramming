
#include "kmeans.h"
#include <stdio.h>
#define TPB 32


__device__ __host__ DataType cudaDistance(DataType* v1, DataType* v2, int d) {
    DataType dist = 0.0;
    for (int i = 0; i < d; i++) {
        dist += (*(v1+i) - *(v2+i))*(*(v1+i) - *(v2+i));
    }
    return sqrt(dist);
}


__host__ bool isConverged(DataType* curCentriod, DataType* prevCentriod, DataType threshold, int n_centroids, int dim) {
    //int idx = blockIdx.x*blockDim.x + threadIdx.x;
    for (int i = 0; i < n_centroids; i++)
        if (cudaDistance(curCentriod + i*dim, prevCentriod + i*dim, dim) >= threshold) return false;
    return true;
}



__global__ void assign_centroid(DataType* points, DataType* centroids, int* clusterIds, int n_points, int n_centroids, int dims)
 {
    const int idx = blockIdx.x*blockDim.x + threadIdx.x;
    //printf("Hello from block %d, thread %d\n", blockIdx.x, threadIdx.x);
    if (idx >= n_points) return;

    DataType minDist = INFINITY;

    for (int j = 0; j < n_centroids; j++) {
        DataType dist = cudaDistance(centroids + j*dims, points + idx*dims, dims);
        //printf("dist: %lf, minDist: %lf, clusterIds: %d, j: %d, points: %lf, centroids: %lf\n", dist, minDist, idx, j, *(points+idx*dims), *(centroids + j*dims)); 
        if ( dist < minDist) {
            minDist = dist;
            clusterIds[idx] = j;              
        }            
    }
    
    //clusterIds[idx] = idx;
}

__global__ void assign_centroid_shared(DataType* points, DataType* centroids, int* clusterIds, int n_points, int n_centroids, int dims)
 {
    //const int idx = blockIdx.x*blockDim.x + threadIdx.x;
    if (blockIdx.x >= n_points) return;

    const int threadId = threadIdx.x;
    //extern __shared__ char localMemory[];
    __shared__ double minDist[1];
    
    //DataType* minDist = (DataType*)&localMemory;
    //int* minIdx = (int*)&localMemory[sizeof(DataType)];
    minDist[0] = INFINITY;
    
    __syncthreads();

    
    //for (int j = 0; j < n_centroids; j++) {
    DataType dist = cudaDistance(centroids + threadId*dims, points + blockIdx.x*dims, dims);

    atomicMin((unsigned long long int*)minDist, __double_as_longlong(dist));
        
    __syncthreads();
    
    if (__double_as_longlong(dist) == __double_as_longlong(*minDist)) {
        clusterIds[blockIdx.x] = threadId;
    }

    printf("Block: %d, thread: %d, dist: %lf, minDist: %lf, minCluster: %d\n", blockIdx.x, threadId, dist, *minDist, clusterIds[blockIdx.x]);
    //clusterIds[idx] = idx;
}

__global__ void group_centroid(DataType* points, DataType* centroids,  int* cluster_of_points, int* cluster_size, int n_points, int n_centroids, int dims) {


    const int idx = blockIdx.x*blockDim.x + threadIdx.x;
    if (idx >= n_points) return;
        

    const int clusterId = cluster_of_points[idx];
    for (int j = 0; j < dims; j++) {
        atomicAdd(centroids+clusterId*dims + j, *(points + idx*dims + j));  
        //printf("idx: %d, j: %d, points: %lf, centroids: %lf\n", idx, j, points[idx*dims + j], centroids[clusterId*dims + j]); 
    }
    atomicAdd(&cluster_size[clusterId], 1);
    //printf("idx: %d, clusterId: %d, cluster size: %d\n", idx, clusterId, cluster_size[clusterId]);
    //std::cout<<points_centroid[clusterIds[i]]<<std::endl;

    //__syncthreads();

/*
    if (idx < n_centroids && cluster_size[idx] != 0) {
        centroids[idx] = centroids[idx] / cluster_size[idx] ;
    }
    */

}

__global__ void update_centoid(DataType* centroids, int* cluster_size, int n_centroids, int dims) {


    const int idx = blockIdx.x*blockDim.x + threadIdx.x;
    if (idx < n_centroids && cluster_size[idx] != 0) {
        for (int j = 0; j < dims; j++) {
            centroids[idx*dims + j] /= cluster_size[idx] ;
        }        
    }
    
}


__global__ void group_centroid_shared(DataType* points, DataType* centroids,  int* cluster_of_points, int* cluster_size, int n_points, int n_centroids, int dims) {


    const int idx = blockIdx.x*blockDim.x + threadIdx.x;
    
    if (idx >= n_points) return;
    

    const int threadId = threadIdx.x;
    extern __shared__ int localMemory[];
    
    DataType* block_points = (DataType*)&localMemory;
    printf("Hello from blockId %d, threadId %d, blockDim %d\n", blockIdx.x, threadIdx.x, blockDim.x);
    for (int d = 0; d < dims; d++) {
        block_points[threadId*dims + d] = points[idx*dims + d];
    }
    
    __shared__ int block_cluster_of_points[TPB];
    block_cluster_of_points[threadId] = cluster_of_points[idx];

    
    __syncthreads();

    //int clusterId = block_cluster_of_points[threadId];

    if (threadId == 0) {

        DataType* block_cluster_point_sum = (DataType*)&localMemory[TPB*dims];
        int* block_cluster_size = (int*)&localMemory[TPB*dims + n_centroids*dims];

        for (int i = 0; i < n_centroids; i++) {
            for (int d = 0; d < dims; d++) {                
                block_cluster_point_sum[i*dims + d] = 0.0;                
            }
            block_cluster_size[i] = 0;
        }

        for (int j = 0; j < blockDim.x; j++){
            int clusterId = block_cluster_of_points[j];
            for (int d = 0; d < dims; d++) {                
                block_cluster_point_sum[clusterId*dims + d] += block_points[j*dims + d];                
            }
            block_cluster_size[clusterId] += 1;
        }
            
        
        for (int i = 0; i < n_centroids; i++) {
            for (int d = 0; d < dims; d++)
                atomicAdd(centroids+i*dims + d, block_cluster_point_sum[i*dims+d]);
            atomicAdd(&cluster_size[i], block_cluster_size[i]);
        }
    }

    
    //std::cout<<points_centroid[clusterIds[i]]<<std::endl;

    //__syncthreads();

    //printf("idx: %d, clusterId: %d, cluster size: %d\n", idx, clusterId, cluster_size[clusterId]);
    /*
    if (idx < n_centroids && cluster_size[idx] != 0) {
        for (int j = 0; j < dims; j++) {
            centroids[idx*dims + j] /= cluster_size[idx] ;
        }        
    }
    */

}

void kmean_cuda(kmean_args_t* args, double* timeMeasure, int algo){

    cudaEvent_t start, stop, assignStart, assignEnd, updateStart, updateEnd;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventCreate(&assignStart);
    cudaEventCreate(&assignEnd);
    cudaEventCreate(&updateStart);
    cudaEventCreate(&updateEnd);
    cudaEventRecord(start);

    int n_vals = args->n_vals;
    int n_centroids = args->num_cluster;
    int dims = args->dims;
    args->iter_converge = 0;

    DataType* d_points=0, *d_centroids=0;
    int* cluster_id=0, *cluster_size=0;

    cudaMalloc(&d_points, n_vals*dims*sizeof(DataType));
    cudaMalloc(&d_centroids, n_centroids*dims*sizeof(DataType));
    cudaMalloc((void**)&cluster_id, n_vals*sizeof(int));
    cudaMalloc(&cluster_size, n_centroids*sizeof(int));

    cudaMemcpy(d_points, args->input_vals, n_vals*dims*sizeof(DataType), cudaMemcpyHostToDevice);
    cudaMemcpy(d_centroids, args->centroids, n_centroids*dims*sizeof(DataType), cudaMemcpyHostToDevice);

    DataType* tmpCentroid = (DataType*) malloc(n_centroids* dims * sizeof(DataType));
    cudaEventRecord(start);
    //std::cout<<"Iter: " <<args->iter_converge<<std::endl;
    while (args->iter_converge < args->max_num_iter && !isConverged(args->centroids, tmpCentroid, args->threshold, n_centroids, dims)) {
        args->iter_converge++;     
        //copyCentroids(tmpCentroid, args->centroids, args->num_cluster, args->dims);
        memcpy(tmpCentroid, args->centroids, n_centroids*dims*sizeof(DataType));
        cudaEventRecord(assignStart);

        if (algo == 0) {
            //assign_centroid<<<(n_vals+TPB-1)/TPB,TPB>>>(d_points, d_centroids,  cluster_id, n_vals, n_centroids, dims);
            assign_centroid<<<(n_vals*n_centroids+n_centroids-1)/n_centroids,n_centroids>>>(d_points, d_centroids,  cluster_id, n_vals, n_centroids, dims);
        } else if (algo == 1) {
            assign_centroid_shared<<<(n_vals*n_centroids+n_centroids-1)/n_centroids,n_centroids>>>(d_points, d_centroids,  cluster_id, n_vals, n_centroids, dims);
        }

        cudaEventRecord(assignEnd);
        cudaEventSynchronize(assignEnd);
        float tmp1 = 0;
        cudaEventElapsedTime(&tmp1, assignStart, assignEnd);
        timeMeasure[0] += tmp1;
        //add<<<N,1>>>(cluster_id);
        
        //cudaMemcpy(args->centroids, d_centroids, n_centroids*dims*sizeof(DataType), cudaMemcpyDeviceToHost);
        //cudaMemcpy(args->clusterId_of_point, cluster_id, n_vals*sizeof(int), cudaMemcpyDeviceToHost);

        //std::cout<<args->iter_converge<<std::endl;m
        //std::cout<<std::endl<<"Iter: " <<args->iter_converge<<std::endl;
        //cudaDeviceSynchronize();
        /*
        for (int i = 0; i < n_vals; i++){
            printf("%d ", args->clusterId_of_point[i]);
        }
        printf("\n");
        */
        /*
        for (int clusterId = 0; clusterId < args->num_cluster; clusterId++){
            printf("%d ", clusterId);
            for (int d = 0; d < args->dims; d++)
                printf("%lf ", args->centroids[clusterId*args->dims + d]);
        printf("\n");
        }
        */
        cudaMemset(d_centroids, 0.0, n_centroids*dims*sizeof(DataType));
        cudaMemset(cluster_size, 0, n_centroids*sizeof(int));

        cudaEventRecord(updateStart);

        if (algo == 0) {
            group_centroid<<<(n_vals+TPB-1)/TPB,TPB>>>(d_points, d_centroids,  cluster_id, cluster_size, n_vals, n_centroids, dims);
        } else if (algo == 1) {
            group_centroid_shared<<<(n_vals+TPB-1)/TPB,TPB, (TPB+n_centroids)*dims*sizeof(DataType) + n_centroids*sizeof(int)>>>(d_points, d_centroids,  cluster_id, cluster_size, n_vals, n_centroids, dims);

        }        
        
        update_centoid<<<(n_vals+TPB-1)/TPB,TPB>>>(d_centroids,  cluster_size, n_centroids, dims);
        cudaEventRecord(updateEnd);
        cudaEventSynchronize(updateEnd);
        float tmp2 = 0;
        cudaEventElapsedTime(&tmp2, updateStart, updateEnd);
        timeMeasure[1] += tmp2;

        cudaMemcpy(args->centroids, d_centroids, n_centroids*dims*sizeof(DataType), cudaMemcpyDeviceToHost);
        
        //bool testCondition = isConverged(args->centroids, tmpCentroid, args->threshold, n_centroids, dims);
        //std::cout<<"Test Condition: "<<testCondition<<std::endl;

        
        for (int clusterId = 0; clusterId < args->num_cluster; clusterId++){
            printf("%d ", clusterId);
            for (int d = 0; d < args->dims; d++)
                printf("%lf ", args->centroids[clusterId*args->dims + d]);
        printf("\n");
        }
        
    }
    cudaMemcpy(args->clusterId_of_point, cluster_id, n_vals*sizeof(int), cudaMemcpyDeviceToHost);

    cudaEventRecord(stop);

    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    free(tmpCentroid);
    cudaFree(d_points);
    cudaFree(d_centroids);
    cudaFree(cluster_id);
    cudaFree(cluster_size);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float tmp3 = 0;
    cudaEventElapsedTime(&tmp3, start, stop);
    timeMeasure[2] += tmp3;
    timeMeasure[3] = timeMeasure[2] - timeMeasure[1] - timeMeasure[0];
}


