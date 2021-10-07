#include "utils.h"
#include <math.h>


double distance(double* v1, double* v2, int d) {
    double dist = 0.0;
    for (int i = 0; i < d; i++) {
        dist += (v1[i] - v2[i])*(v1[i] - v2[i]);
    }
    return sqrt(dist);
}
static unsigned long int next = 1;
static unsigned long kmeans_rmax = 32767;

void copyCentroids(double** oldCentroids, double** newCentroid, int n, int d) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < d; j++) {
            oldCentroids[i][j] = newCentroid[i][j];
    }
}

int kmeans_rand() {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % (kmeans_rmax+1);
}
void kmeans_srand(unsigned int seed) {
    next = seed;
}