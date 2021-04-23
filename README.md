# Introduction
This code is submitted as answers to the Parallel and Distributed Computing Lab 4 and Final Project. The following sections explain how to run the code.
* Author: Ndze'dzenyuy, Lemfon K.
* Date: April 2021

# First thing to do

Visit the repository [here](https://github.com/WybeTuring/PDC-Labs). Please check your email and confirm that you have access to the repository. 

Clone the repository and check out the branch named lab4. 

# Run Code for Lab 4
The code submitted for lab 4 simply runs the classical wordcount code in MapReduce on the file named dickens.txt
Please follow the steps below:

1. Ensure that you have mrs-mapreduce installed on your system. If you do not, please visit it's [documentation](https://pythonhosted.org/mrs-mapreduce/installation.html) to install. The prefered method of installing should be by downloading the tar file.
2. Untar the tar file, and cd into the mrs-mapreduce directory 
3. Run the command ```sudo python setup.py install``` from the terminal. 
4. Run the command ``` python examples/wordcount.py dickens.txt tutorial_outdir```
5. Go to the tutorial_outdir directory and check the results in the most recently created file. 

# Run code for the Final Project
1. Ensure that you are in the top of the lab4 directory
2. Run the command ```python matrix.py``` and specify the dimensions of the matrix to generate two random matrices to be stored in the files A.txt and B.txt
3. Run the command ```time python3 lemfonProject.py A.txt  B.txt ./ -D 6 -T 9``` to multiply the matrices and store the result in Result.txt. D specifies the dimensions of the matrix and T specifies the number of tasks. Please ensure that the square root of the tasks is always a factor of the dimension of the matrices. No verification is done for this condition and the result may not be as expected if it is not met.
4. Check the Result.txt file for the solutions
5. To run the serial multiplication, run the command ```python serial.py``` and input the common dimension of the square matrices stored in A.txt and B.txt.
6. Open the file SerialResult.txt to see the result.
