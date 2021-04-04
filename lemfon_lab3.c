/*
*  School: Ashesi University 
*  Course: CS434: Introduction to Parallel & Distribted Computing 
*  Topic: Parallel Programming with Message Passing Interface (MPI)
*  Lab Number: Lab 3
*  Author: Ndze'dzenyuy, Lemfon. 
*  Date: 30th March 2021.
*/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//#define dim 4    /* dimensions of the matrixes */
#define master 0            
#define master_message 1    // Tag that will be put on the messages from the master process carrying parts of the matrices   
#define worker_message 2        // Tag that will be put on the messages from the worker processes carrying results.


// Useful functions. The comments for each function can be found in the function definitions below.
void determine_valid_procs(int n);
int **allocarray(int r, int c);
int ** get_division_cols(int **A, int dimension, int start, int end);
int ** create_matrix(int dimension);
int ** create_matrix_value(int dimension, int var);
void print_matrix(int **mat, int dimension, char name);
int ** multiply_seq(int **A, int **B, int a_rows, int a_cols, int b_rows, int b_cols);
void print_nonsquare(int **mat, int rows, int cols);
int ** get_division_rows(int **A, int dimension, int start, int end);
int * matrix_linearize(int **mat, int rows, int cols);
void printvector(int *vec, int l);
int * allocvector(int vec_size);
int **vec_to_array(int *vec, int l, int rows, int cols);

// The struct that will tell the master processor at which index to start placing the code it recieved
struct start_location {
    int x_start;
    int y_start;
};

int main (int argc, char *argv[]){

    int	numprocs, my_rank, numworkers, source, dest, mtype, rows;   // Declaring some useful variables
    int n = atoi(argv[1]);
    int	i, j, k, rc; 
    int	**a, **b, **c;  
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

    // Creating our special data type.
    MPI_Datatype position_type;
    int lengths[2] = { 1, 1};
    const MPI_Aint displacements[2] = { 0, sizeof(int)};
    MPI_Datatype types[2] = { MPI_INT, MPI_INT};
    MPI_Type_create_struct(2, lengths, displacements, types, &position_type);
    MPI_Type_commit(&position_type);


    if (numprocs < 2 ) {
        printf("Sorry, but we will need more than 2 processes to do this multiplication. \n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
    }

    // Here we check if the number of processes is a number whose square root is divisible by the dimension.
    double rt_temp = sqrt(numprocs - 1); 
    if(fmod(n,rt_temp) != 0){
        if(my_rank == master){
        printf("Because n is %d, you can only run this program with the following number or processes: ", n);
        determine_valid_procs(n);
        printf(". \nPlease note that 1 process will always act as the master process. So for example, if you said you want to use 5 processes, 4 will be used for the calculations and 1 will be a master process.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
        }
    }

    numworkers = numprocs-1;
    //int rt_p = 3; // Square root of 9.
    int rt_p = (int) sqrt(numprocs - 1);
    int group_size = n / rt_p; // The sizes of the blocks that will be sent
    int buffer_size = group_size * n; // The size of the buffer that will be sent


   // The code for the master process.
   if (my_rank == master)
   {
      printf("mpi_mm has started with %d tasks.\n",numprocs);
      // Generating and printing matrix a
      a = create_matrix(n);
      print_matrix(a, n, 'A');
      // Generating and printing matrix b  
      b = create_matrix(n);
      print_matrix(b, n, 'B');
      
      //Generating C
      c = create_matrix_value(n, 0);

      /* Measure start time */
      double start = MPI_Wtime();

      /* Send matrix data to the worker tasks */
      mtype = master_message;
      for (dest=1; dest<=numworkers; dest++){

        int row = ((dest - 1) / rt_p) * group_size;
        int col = ((dest - 1) % rt_p) * group_size;
        
        // Sending the rows of matrix A as needed.
        int **subRows = get_division_rows(a, n, row, row + group_size - 1);
        int *subRowsFinal = matrix_linearize(subRows, group_size, n);
        MPI_Send(&(subRowsFinal[0]), buffer_size, MPI_INT, dest, 10, MPI_COMM_WORLD);
        
        //Sending the columns of matrix B as needed.
        int **subCols = get_division_cols(b, n, col, col + group_size - 1);
        int *subColsFinal = matrix_linearize(subCols, n, group_size);
        MPI_Send(&(subColsFinal[0]), buffer_size, MPI_INT, dest, 4, MPI_COMM_WORLD);
      }


      /* Receive results from worker tasks */
      mtype = worker_message;
      for (i=1; i<=numworkers; i++){
         int incoming = i;
         int *y = allocvector(group_size * group_size); 
         MPI_Recv(&(y[0]), group_size * group_size, MPI_INT, incoming, 5, MPI_COMM_WORLD, &status);
         struct start_location p_start;
         MPI_Recv(&p_start, 1, position_type, incoming, 15, MPI_COMM_WORLD, &status);

         int **mini_res = vec_to_array(y, group_size * group_size, group_size, group_size);
         printf("From process %d: Whose block size starts at row index %d and column index %d.\n", i, p_start.x_start, p_start.y_start);
         print_matrix(mini_res, group_size, i);

         //Putting the various answers in their appropriate positions in matrix C.
         
         for(int m = 0; m < group_size; m++){
             for(int n = 0; n < group_size; n++){
                 c[p_start.x_start + m][p_start.y_start + n] = mini_res[m][n];
             }
         }
      }

      /* Measure finish time */
      double finish = MPI_Wtime();  

      //Printing C, the final result.
      printf("\nThe final result is obtained.\n");
      print_matrix(c, n, 'C');
      printf("\nThe parallel computation was done in %f seconds.\n", finish - start);

      // Doing the serial computation.
      clock_t begin = clock();
      int **D = multiply_seq(a,b, n, n, n, n);
      clock_t end = clock();
      printf("The serial computation was done in %f seconds.\n", (double)(end - begin) / CLOCKS_PER_SEC);

   }


   // The code for the various worker processes. 
   if (my_rank > master)
   {
      mtype = master_message;

      // Calculating the starting indices of the final result.
      int rowp = ((my_rank - 1) / rt_p) * group_size;
      int colp = ((my_rank - 1) % rt_p) * group_size;

      // Recive the rows in a vector format, and then converting them back to a normal array to ease multiplication
      int *subRowsFinal = allocvector(buffer_size);
      MPI_Recv(&(subRowsFinal[0]), buffer_size, MPI_INT, master, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      int **subRows = vec_to_array(subRowsFinal, buffer_size, group_size, n);

      // Revieve the cols in vector format, and then converting them back to a normal array form to ease multiplication
      int *subColsFinal = allocvector(buffer_size);;
      MPI_Recv(&(subColsFinal[0]), buffer_size, MPI_INT, master, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      int **subCols = vec_to_array(subColsFinal, buffer_size, n, group_size);

      // multiplying the answers gotten from the rows and cols and linearizing for sending to the master
      int **tempAns = multiply_seq(subRows, subCols, group_size, n, n, group_size);
      int *finRes = matrix_linearize(tempAns, group_size, group_size);

      // Sending the temp answer back to the master
      MPI_Send(&(finRes[0]), group_size * group_size, MPI_INT, master, 5, MPI_COMM_WORLD);

      // The process also has to send information about the index at which it's answers will be placed in the final matrix.
      struct start_location buffer;
      buffer.x_start = rowp;
      buffer.y_start = colp;
      MPI_Send(&buffer, 1, position_type, master, 15, MPI_COMM_WORLD);
   }
   MPI_Finalize();
   return 0;
}

/*
    This function will help us to determine the set of valid number of procecces that the user can use for the computation.
    The idea is that the square of the number of processes will always be a factor of the dimension of the matrix for easy work.
*/
void determine_valid_procs(int n){
    for (int i = 2; i < n; ++i) {
        if (n % i == 0) {
            int p = (i * i) + 1;
            printf("%d, ", p);
        }
    }
}

/*
    Creates a matrix dynammically. The matrices are created in a manner that ensures that the biggest value in a matrix is always 100.
*/
int ** create_matrix(int dimension){
    // creating the array
    int **matrix = (int **) malloc(dimension * sizeof(int *));
    
    for (int i = 0; i < dimension; i++) {
        matrix[i] = (int *) malloc(dimension * sizeof(int));
    }
    //randomly generating the numbers and filling them.
    srand(time(0));
    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            matrix[i][j] = (rand() % 100) + 1 ;
            //matrix[i][j] = 2;
        }

    }
    return matrix;
}

/*
    This function also creates a matrix, but initializes all the values to an argument that is passed by the user. 
*/
int ** create_matrix_value(int dimension, int value){
    // creating the array
    int **matrix = (int **) malloc(dimension * sizeof(int *));
    
    for (int i = 0; i < dimension; i++) {
        matrix[i] = (int *) malloc(dimension * sizeof(int));
    }
    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            matrix[i][j] = value;
        }

    }
    return matrix;
}

/*
    Function to print a matrix, alongside its letter name.
*/
void print_matrix(int **mat, int dimension, char name){
    printf("******************************************************\n");
      printf("Matrix %c:\n", name);
      for (int i=0; i<dimension; i++)
      {
         for (int j=0; j<dimension; j++) 
            printf("%5d   ", mat[i][j]);
        printf("\n");
      }
}

/*
    Function to print a matrix. Most appropriate when the matrix is not square matrix.
*/
void print_nonsquare(int **mat, int rows, int cols){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            printf("%-5d  ", mat[i][j]);
        }
        printf("\n");
    }
}

/*
    Allocates memory for an array. Could also work for two-dimensional arrays that are not square matrices. 
    The user has to specify the rows and the columns.
*/
int **allocarray(int r, int c) {
    int **temp = (int **) malloc(r * sizeof(int *));
    for (int i = 0; i < r; i++) {
        temp[i] = (int *) malloc(c * sizeof(int));
    }
    return temp;
}

/*
    This function takes in a matrix, and indices for columns, and returns a slice of the matrix.
    The indices are used in an inclusive manner. In other words, this function returns a continuous set of columns.
*/
int ** get_division_cols(int **A, int dimension, int start, int end){
    // Creating the matrix to be returned.
    int size = end - start + 1;
    int **temp = (int **) malloc(dimension * sizeof(int *));
    for (int i = 0; i < dimension; i++) {
        temp[i] = (int *) malloc(size * sizeof(int));
    }
    // Copying the elements from the matrix to temp
    for(int i = start; i <= end; i++){
        for(int j = 0; j < dimension; j++){
            temp[j][i - start] = A[j][i];
        }
    }
    return temp;
}

/* This function multiplies two n-square matrices and returns a matrix containing the result. 
*  The assumption is that the dimensions of the function are already correct
*/
int ** multiply_seq(int **A, int **B, int a_rows, int a_cols, int b_rows, int b_cols){
    // Creating the matrix C
    int **C = (int **) malloc(a_rows * sizeof(int *));
    
    for (int i = 0; i < a_rows; i++) {
        C[i] = (int *) malloc(b_cols * sizeof(int));
    }

    // Carrying out the multiplication
    for(int i = 0; i < a_rows; i++){
        for(int j = 0; j < b_cols; j++){
            C[i][j] = 0;
            for(int k = 0; k < a_cols; k++){
                C[i][j] = C[i][j] + (A[i][k] * B[k][j]);
            }
        }
    }
    return C;
}

/* This function takes in a matrix and some row indices and returns a slice of columns. 
    The indices are used in an inclusive manner
*/
int ** get_division_rows(int **A, int dimension, int start, int end){
    // Creating the matrix to be returned.
    int size = end - start + 1;
    int **temp = (int **) malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        temp[i] = (int *) malloc(dimension * sizeof(int));
    }
    // Copying the elements from the matrix to temp
    for(int i = start; i <= end; i++){
        for(int j = 0; j < dimension; j++){
            temp[i - start][j] = A[i][j];
        }
    }
    return temp;
}

/* This function prints a vector. Used mainly for debugging purposes */
void printvector(int *vec, int l){
    for(int i = 0; i < l; i++){
        printf("%d ", vec[i]);
    }
    printf("\n");
}

/* This function takes a matrix and linearizes it, returning a vector that could later on be transformed into a matrix */
int * matrix_linearize(int **mat, int rows, int cols){
    int *temp = (int *)malloc(rows * cols * sizeof(int));
    int k = 0;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            temp[k] = mat[i][j];
            k++;
        }
    }
    return temp;
}

/* This function allocates space for a vector dynamically*/
int * allocvector(int vec_size){
    int *temp = (int *)malloc(vec_size * sizeof(int));
    return temp;
}

int **vec_to_array(int *vec, int l, int rows, int cols){
    int **temp = allocarray(rows, cols);
    for(int i = 0; i < l; i++){
        int r = i / cols;
        int c = i % cols;
        temp[r][c] = vec[i];
    }
    return temp;
}