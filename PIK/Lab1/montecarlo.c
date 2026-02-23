#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]){

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    if(argc != 2){
        return 1;
    }

    const char* threads = getenv("OMP_NUM_THREADS");

    if(threads == NULL) return 1;

    long n = strtoul(argv[1], NULL, 10);
    if(n == 0) return 1;

    const double a = 0.0;
    const double b = 10.0;
    double sum = 0.0;
    double variance = 0.0;

    #pragma omp parallel
    {
        unsigned int seed;
        seed = (unsigned int)time(NULL) ^ (omp_get_thread_num() + 1) ^ (unsigned int)(uintptr_t)&seed;

        double local_sum = 0.0;
        double local_variance = 0.0;

        #pragma omp for schedule(static)
        for(int i = 0; i < n; i++){
            double u = rand_r(&seed)/((double)RAND_MAX + 1.0);
            double x = a + (b - a) * u;
            double fx = exp(-x * x);
            local_sum += fx;
            local_variance += fx*fx;
        }

        #pragma omp atomic
        sum += local_sum;
        #pragma omp atomic
        variance += local_variance;
    }

    double E = sum/(double)n;
    double E2 = variance/(double)n;
    double i = (b - a)*E;
    double diff = E2 - E*E;
    if(diff < 0.0) diff = 0.0;

    double sigma = sqrt((b - a)*diff);
    double error = sigma/sqrt(n);

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed_time = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_nsec - t_start.tv_nsec)/1e9;

    printf("Vrijednost integrala: %.6f\n", i);
    printf("Pogreška: %.6f\n", error);
    printf("Vrijeme izvođenja programa: %.6f\n", elapsed_time);

    return 0;
}