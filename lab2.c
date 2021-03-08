#include<stdlib.h>
#include<pthread.h>
#include<time.h>

// This method prints an array.
void print_matrix(int **mat, int dim);

// This function takes in an integer and returns a two dimensional array that has the elements randomly generated.
int ** create_matrix(int dim);




int main(){

}

// This function takes in an integer and returns a two dimensional array that has the elements randomly generated.
int ** create_matrix(int dim){

    // creating the array
    int **matrix = (int **) malloc(dim * sizeof(int *));
    
    for (int i = 0; i < rows; i++) {
        matrix[i] = (int *) malloc(dim * sizeof(int));
    }

    //randomly generating the numbers and filling them.
    srand(time(0));
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            matrix[i][j] = rand();
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