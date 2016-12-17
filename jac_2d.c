#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#define  Max(a,b) ((a)>(b)?(a):(b))

int itmax = 100;

MPI_Request req[4];
MPI_Status status[4];
int myrank, rank_size, start_row, last_row, nrow, ll, shift;

void init(int N, double *A[], double *B[])
{ 
    #pragma omp parallel for shared(A, B, N, nrow)
    for(int i = 1; i <= nrow; i++) {
        for(int j = 0; j < N; j++) {    
            if(j == 0 || j == N - 1) {
                A[i][j] = 0.;
            } else {
                A[i][j] = (1. + start_row + i - 1 + j);
            }
            B[i][j] = 0;
        }
    }
} 

void relax(int N, double *A[], double *B[])
{
    #pragma omp parallel for shared(A, B, N, nrow, myrank, rank_size)
    for(int i = 1; i <= nrow; i++) {
        if (((i == 1) && (myrank == 0)) ||
           ((i == nrow) && (myrank == rank_size - 1))){
            continue;
        }
        for(int j = 1; j < N - 1; j++) {
            B[i][j] = (A[i - 1][j] + A[i + 1][j] + A[i][j - 1] + A[i][j + 1]) / 4.;
        }
    }
}

void resid(int N, double *A[], double *B[])
{ 
    #pragma omp parallel for shared(A, B, N, nrow)
    for(int i = 1; i <= nrow; i++) {
        for(int j = 1; j <= N - 2; j++) {      
            A[i][j] = B[i][j]; 
        }
    }
}

void run(int N, double *A[], double *B[])
{
    init(N, A, B);
    for(int it = 1; it <= itmax; it++) {
        if(myrank != 0){
            MPI_Irecv(A[0], N, MPI_DOUBLE, myrank - 1, 1, MPI_COMM_WORLD,  &req[0]);
        }
        if(myrank != rank_size - 1) {
            MPI_Isend(A[nrow], N, MPI_DOUBLE, myrank + 1, 1, MPI_COMM_WORLD, &req[2]);
        }
        if(myrank != rank_size - 1){
            MPI_Irecv(A[nrow+1], N, MPI_DOUBLE, myrank + 1, 2, MPI_COMM_WORLD, &req[3]);
        }
        if(myrank != 0){
            MPI_Isend(A[1], N, MPI_DOUBLE, myrank - 1, 2, MPI_COMM_WORLD, &req[1]);
        }
        shift = 0;
        ll = 4;
        if (myrank == 0) {
            ll = 2;
            shift = 2;
        }
        if (myrank == rank_size - 1) {
            ll = 2;
        }
        MPI_Waitall(ll, &req[shift], &status[0]);

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
    //init MPI
    int err_code = 0;
    if (err_code = MPI_Init(&argc, &argv)) {
        printf("Error int MPI_Init\n");
        return err_code;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &rank_size);
    MPI_Barrier(MPI_COMM_WORLD);
    start_row = (myrank * N) / rank_size;
    last_row = ((myrank + 1) * N) / rank_size - 1;
    nrow = last_row - start_row + 1;
    A = calloc(nrow + 2, sizeof(double *));
    B = calloc(nrow + 2, sizeof(double *));
    for(int i = 0; i < nrow + 2; ++i) { 
        A[i] = calloc(N, sizeof(double));
        B[i] = calloc(N, sizeof(double));
    }
    double end_time = 0;
    double start_time = 0;
    if (myrank == 0) {
        start_time = MPI_Wtime();
    }
    for (int cur_test = 0; cur_test < number_of_tests; ++cur_test){
        MPI_Barrier(MPI_COMM_WORLD);
        run(N, A, B);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if (myrank == 0)
    {
        end_time = MPI_Wtime();
        printf("%.6lf\n", end_time - start_time);
    }
    for (int i = 0; i < nrow + 2; ++i){
        free(A[i]);
        free(B[i]);
    }
    free(A);
    free(B);
    MPI_Finalize();
    return 0;
}