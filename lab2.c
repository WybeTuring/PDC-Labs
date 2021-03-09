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

// Naive OpenMP algorithms 
int ** naive_openmp(int **mat, int dim);
int ** diag_openmp(int **mat, int dim);

// PThread algorithms 
void diag_pthreads(int **mat, int dim);
void* pth_diag(void* rank);



int main(){

    int ** t = create_matrix(4);
    print_matrix(t,4);
    puts("\n");

    /*
    print_matrix(naive_openmp(t,4),4);
    puts("\n");
    print_matrix(diag_openmp(t,4),4);
    */

    // The pthreads section
    mat = t;
    dim = 4;
    diag_pthreads(mat,dim);
    print_matrix(mat,dim);


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
            printf("%d  ", mat[i][j]);
        }
        printf("\n");
    }
}

// This function transposes the matrix using the naive algorithm
int ** naive_openmp(int **mat, int dim){
    # pragma omp parallel for num_threads(dim)
    for(int i = 0; i < dim; i++){
        # pragma omp parallel for num_threads(dim)
        for(int j = 0; j < i; j++){
            int temp = mat[i][j];
            mat[i][j] = mat[j][i];
            mat[j][i] = temp; 
        }
    }
    return mat;
}

// This function transposes the matrix using the diagonal threads approach.
int ** diag_openmp(int **mat, int dim){
    # pragma omp parallel for num_threads(dim)
    for(int i = 0; i < dim; i++){
        for(int j = i; j < dim; j++){
            int temp = mat[i][j];
            mat[i][j] = mat[j][i];
            mat[j][i] = temp;
        }
    }
    return mat;
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