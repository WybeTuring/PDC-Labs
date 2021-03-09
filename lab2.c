/*
* This code is submitted as answers to the Parallel and Distributed Computing Lab 2
* Author: Ndze'dzenyuy, Lemfon K.
* Date: 6th March 2021
* Instructions: This code calculates the transpose of a matrix using three main approaches: a naive and an optimised approach using OpenMP and an optimised approach
* using Pthreads. 
* To compile the program, run:  gcc -g -Wall -fopenmp -o lab2 lab2.c -lpthread 
* To run the program, use: ./lab2 <size of the matrix> <1 to print transpose, 0 not to print>
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
#include <time.h>

// Defining some global variables 
int **mat;
int dim;

// This method prints an array.
void print_matrix(int **mat, int dim);

// This function takes in an integer and returns a two dimensional array that has the elements randomly generated.
int ** create_matrix(int dim);

// OpenMP algorithms 
void naive_openmp(int **mat, int dim);
void diag_openmp(int **mat, int dim);

// PThread algorithms 
void diag_pthreads(int **mat, int dim);
void* pth_diag(void* rank);

//Basic algorithm
void transpose_basic(int **mat, int dim);



int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Please make sure that you are entering the dimension of the matrix and 1 if you want to print out the matrix, 0 otherwise. Please be warned that for large matrix sizes, printing may be hard to read.");
        return 0;
    }
    else{
        // Getting the dimension of the matrix from the command line
        dim = strtol(argv[1], NULL, 10);
        int print = strtol(argv[2], NULL, 10);

        mat = create_matrix(dim);
        if(print){
            printf("The matrix generated using random numbers is presented below:\n");
            print_matrix(mat,dim);
            puts("\n");
        }else{
           printf("The matrix has been generated!\n"); 
           puts("\n");
        }
        
        //Transposing the matrix using the basic approach
        clock_t begin = clock();
        transpose_basic(mat,dim);
        clock_t end = clock();
        double time_used = (double)(end - begin) / CLOCKS_PER_SEC;
        if(print){
            print_matrix(mat,dim);
        }
        //Because the transposing was done in place, we have to redo it to get the original matrix
        transpose_basic(mat,dim);
        printf("To transpose the matrix with the basic approach we used %f seconds\n\n", time_used);

        //Transposing the matrix using the naive openmp approach
        begin = clock();
        naive_openmp(mat,dim);
        end = clock();
        time_used = (double)(end - begin) / CLOCKS_PER_SEC;
        if(print){
            print_matrix(mat,dim);
        }
        //Because the transposing was done in place, we have to redo it to get the original matrix
        naive_openmp(mat,dim);
        printf("To transpose the matrix with the naive approach and OpenMP we used %f seconds\n\n", time_used);

        //Transposing the matrix using the diagonal threads approach and OpenMP
        begin = clock();
        diag_openmp(mat,dim);
        end = clock();
        time_used = (double)(end - begin) / CLOCKS_PER_SEC;
        //Because the transposing was done in place, we have to redo it to get the original matrix
        if(print){
            print_matrix(mat,dim);
        }
        diag_openmp(mat,dim);
        printf("To transpose the matrix with the diagonal threads approach and OpenMP we used %f seconds\n\n", time_used);

        //Transposing the matrix using the diagonals approach and PThreads
        begin = clock();
        diag_pthreads(mat,dim);
        end = clock();
        time_used = (double)(end - begin) / CLOCKS_PER_SEC;
        //Because the transposing was done in place, we have to redo it to get the original matrix
        if(print){
            print_matrix(mat,dim);
        }
        diag_pthreads(mat,dim);
        printf("To transpose the matrix with the diagonal threads approach and PThreads we used %f seconds\n\n", time_used);
    }


}


// This function takes in an integer and returns a two dimensional array that has the elements randomly generated.
int ** create_matrix(int dim){

    // creating the array
    int **matrix = (int **) malloc(dim * sizeof(int *));
    
    for (int i = 0; i < dim; i++) {
        matrix[i] = (int *) malloc(dim * sizeof(int));
    }

    //randomly generating the numbers and filling them.
    srand(time(0));
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            matrix[i][j] = (rand() % dim) + 1 ;
        }

    }

    return matrix;

}

// This function takes a matrix and prints it in the console
void print_matrix(int **mat, int dim){
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            printf("%-5d  ", mat[i][j]);
        }
        printf("\n");
    }
}

// This function transposes a matrix using the basic approach i.e no multithreading
void transpose_basic(int **mat, int dim){
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < i; j++){
            int temp = mat[i][j];
            mat[i][j] = mat[j][i];
            mat[j][i] = temp; 
        }
    }
}

// This function transposes the matrix using the naive algorithm
void naive_openmp(int **mat, int dim){
    # pragma omp parallel for num_threads(dim)
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < i; j++){
            int temp = mat[i][j];
            mat[i][j] = mat[j][i];
            mat[j][i] = temp; 
        }
    }
}

// This function transposes the matrix using the diagonal threads approach.
void diag_openmp(int **mat, int dim){
    # pragma omp parallel for num_threads(dim)
    for(int i = 0; i < dim; i++){
        for(int j = i; j < dim; j++){
            int temp = mat[i][j];
            mat[i][j] = mat[j][i];
            mat[j][i] = temp;
        }
    }
}

// This function transposes the matrix by using the diagonal threads approach and pthreads.
void* pth_diag(void* rank){
    long i = (long) rank;
    for(long j = i; j < dim; j++){
        int temp = mat[i][j];
        mat[i][j] = mat[j][i];
        mat[j][i] = temp;
    }
    return NULL;
}

void diag_pthreads(int **mat, int dim){
    //Creating the threads 
    long thread;
    pthread_t* thread_array;
    thread_array = malloc(dim * sizeof(pthread_t));
    for(thread = 0; thread < dim; thread++){
       pthread_create(&thread_array[thread], NULL, pth_diag, (void*) thread); 
    }
    for(thread = 0; thread < dim; thread++){
        pthread_join(thread_array[thread], NULL);
    }
    free(thread_array);
}