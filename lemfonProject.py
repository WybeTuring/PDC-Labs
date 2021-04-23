# Author: Ndze'dzenyuy, Lemfon K.
# Date: April 2021
# Description: This is code that implements matrix multiplication using the MapReduce concept. The MapReduce implementation used is Mrs Map Reduce.

# Importing important libraries
import mrs
import string
import sys
import math


class MatrixMultiply(mrs.MapReduce):

    # We define some class attributes that will hold the matrices to be multipled and the result.
    A = []
    B = []
    Result = []
    
    
    @classmethod
    def update_parser(cls, parser):
        parser.add_option('-D', '--dim', dest='dim', type='int')
        parser.add_option('-T', '--tasks', dest='tasks',type='int')
        return parser

    def populateResult(self,rank, l, N,blocklen):
        pos = []
        start = (rank-1)*blocklen
        i = ((start)//N)*blocklen
        j = (start)%N
        for k in range(blocklen*blocklen):
            u = (k//blocklen)+i
            v = (k%blocklen)+j
            pos.append((int(u),v))
        for m,z in zip(pos,l):
            self.Result[m[0]][m[1]] = z
    
    def generateMatrix(self,N):
        return [[0 for i in range(N)]for j in range(N)]

    
    def getRows(self,mat,p,blocksize):
        for k in range(int(p)):
            yield mat[k*blocksize:k*blocksize+blocksize]

    def readMatrix(self, mat, argKey):
        with open(self.args[argKey], 'r') as f:
            for l in f.readlines():
                mat.append([int(k) for k in l.strip().split(" ")])

    def writeResult(self):
        with open('Result.txt','w') as file:
            for k in self.Result:
                for i in k:
                    file.write(str(i) + " ")
                file.write("\n")

    def getCols(self,mat,p,blocksize):
        for k in range(int(p)):
            arr = []
            for i in range(len(mat)):
                arr += mat[i][k*blocksize:k*blocksize + blocksize]
            yield self.colsG(arr,blocksize)

    def colsG(self,column, blocklen):
        temp = [[] for k in range(blocklen)]
        for k in range(len(column)):
            temp[k%blocklen].append(column[k])
        return temp

    def map(self, key, value):
        tasks = int(self.opts.tasks)
        N = int(self.opts.dim)
        p = math.sqrt(tasks)
        blocklen = int(N//p)
       
        rank = 1
        for i in self.getRows(value[0],p,blocklen):
            for j in self.getCols(value[1],p,blocklen):
                yield str(rank), i
                yield str(rank), j
                rank += 1
       
    def reduce(self, key, values):
        temp = []
        m = [ k for k in values]
        for r in m[:1][0]:
            for c in m[1:][0]:
                res = 0
                for k in range(len(r)):
                    res += r[k]*c[k]
                temp.append(res)
        yield  temp


    def getKeyValuePairs(self,A,B):
        yield ('key',(A,B))
        
    

    def run(self, job):
        tasks = int(self.opts.tasks)
        N = self.opts.dim
        self.Result = self.generateMatrix(N)
        blocksize = int(N//math.sqrt(tasks))
        # Reading the data for matrix A and B
        self.readMatrix(self.A, 0)
        self.readMatrix(self.B, 1)   
        # Get the key value pairs
        kvpairs = self.getKeyValuePairs(self.A,self.B)
        data = job.local_data(kvpairs)
        unproc = job.map_data(data, self.map)
        data.close()
        res = job.reduce_data(unproc, self.reduce)
        unproc.close()
        job.wait(res)
        res.fetchall()
        for i in res.data():
            self.populateResult(int(i[0]), i[1], N,blocksize)
        self.writeResult()
        return 0

if __name__ == '__main__':
    mrs.main(MatrixMultiply)