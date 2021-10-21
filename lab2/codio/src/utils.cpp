#include "utils.h"

static unsigned long int next = 1;
static unsigned long kmeans_rmax = 32767;


int kmeans_rand() {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % (kmeans_rmax+1);
}
void kmeans_srand(unsigned int seed) {
    next = seed;
}