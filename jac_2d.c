#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define  Max(a,b) ((a)>(b)?(a):(b))

int itmax = 100;

void init(int N, double **A, double **B)
{ 
    for(int i = 0; i <= N - 1; i++) {
        for(int j = 0; j <= N - 1; j++) {    
            if(i == 0 || i == N - 1 || j == 0 || j == N - 1) {
                A[i][j] = 0.;
            } else {
                A[i][j] = (1. + i + j);
            }
        }
    }
} 

void relax(int N, double **A, double **B)
{
    for(int i = 1; i <= N - 2; i++){
        for(int j = 1; j <= N - 2; j++){
            B[i][j] = (A[i - 1][j] + A[i + 1][j] + A[i][j - 1] + A[i][j + 1]) / 4.;
        }
    }
}

void resid(int N, double **A, double **B)
{ 
    for(int i = 1; i <= N - 2; i++){
        for(int j = 1; j <= N - 2; j++){       
            A[i][j] = B[i][j]; 
        }
    }
}

void run(int N, double **A, double **B)
{
    init(N, A, B);
    for(int it = 1; it <= itmax; it++) {
        relax(N, A, B);
        resid(N, A, B);
    }
}

int main(int argc, char *argv[])
{
    // argv[1] = count of runs (int)
    // argv[2] = N (int)
    int number_of_tests;
    sscanf(argv[1],"%d", &number_of_tests);
    printf("%d\n", number_of_tests);
    int N;
    sscanf(argv[2],"%d", &N);
    printf("%d\n", N);
    double  **A, **B;
    A = calloc(N, sizeof(double *));
    B = calloc(N, sizeof(double *));
    for(int i = 0; i < N; ++i) { 
        A[i] = calloc(N, sizeof(double));
        B[i] = calloc(N, sizeof(double));
    }
    double start_time = clock();
    for (int cur_test = 0; cur_test < number_of_tests; ++cur_test){
        run(N, A, B);
    }
    for(int i = 0; i < N; ++i) { 
        free(A[i]);
        free(B[i]);
    }
    free(A);
    free(B);
    double end_time = clock();
    printf("Result for %d %d:\n", number_of_tests, N);
    printf("%.6lf\n", (end_time - start_time) / CLOCKS_PER_SEC);
    return 0;
}