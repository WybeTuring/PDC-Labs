# Author: Ndze'dzenyuy, Lemfon K.
# Date: April 2021
# Description: This is code that implements matrix multiplication using the serial approach

import time

def generateMatrix(N):
    return [[0 for i in range(N)]for j in range(N)]

def writeResult(R):
        with open('SerialResult.txt','w') as file:
            for k in R:
                for i in k:
                    file.write(str(i) + " ")
                file.write("\n")

def readMatrix(mat, name):
        with open(name, 'r') as f:
            for l in f.readlines():
                mat.append([int(k) for k in l.strip().split(" ")])

def matrixMult(A, B, N):
    C = generateMatrix(N)
    for i in range(N):
        for j in range(N):
            for k in range(N): 
                C[i][j] += A[i][k] * B[k][j]
    return C

def main():
    A = []
    B = []
    N = int(input("Please enter the dimension of the matrix: "))
    readMatrix(A, "A.txt")
    readMatrix(B, "B.txt")
    start_time = time.time()
    C = matrixMult(A, B, N)
    end_time = time.time()
    writeResult(C)
    print("--- %s seconds ---" % (end_time - start_time))

main()

                