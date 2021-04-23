# Author: Ndze'dzenyuy Lemfon
# Purpose: Generate two random matrices, A and B, that will be multipled.

import random

print("THIS PROGRAM ASSUMES THAT WE ARE WORKING WITH SQUARE MATRICES")
dim = int(input('Please enter the dimension (A single number):'))

# Generating the first matrix
matrixA = [[str(random.randint(1,dim)) for i in range(dim)] for k in range(dim)]
# Writing the matrix to a file that will be used for the multiplication
with open('A.txt','w') as file:
    for i in matrixA:
        file.write(" ".join(i)+"\n")

# Generating the second matrix
matrixB = [[str(random.randint(1,dim)) for i in range(dim)] for k in range(dim)]
# Writing the matrix to a file that will be used for the multiplication
with open('B.txt','w') as file:
    for i in matrixB:
        file.write(" ".join(i)+"\n")

print("MATRICES GENERATED. CHECK FILES A.txt & B.txt")
