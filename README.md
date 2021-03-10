# Introduction
This code is submitted as answers to the Parallel and Distributed Computing Lab 2
* Author: Ndze'dzenyuy, Lemfon K.
* Date: 6th March 2021
* Instructions: This code calculates the transpose of a matrix using four main approaches: a unthreaded naive approach, naive and an optimised approach using OpenMP and an optimised approach
* using Pthreads. 

# How to run the code
Visit and clone the repository at https://github.com/WybeTuring/PDC-Labs.git. Navigate to the branch named lab2. Ensure that you are in the folder that contains the file lab2.c and then open the command line. 

To compile: gcc -g -Wall -fopenmp -o lab2 lab2.c -lpthread

To run: ./lab2 <size_of_the_matrix> <1 to print transpose, 0 not to print>