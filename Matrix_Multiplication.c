/*-------------------------------------------------------------------------------
CS-450  Fall 2013
Programming Assignment #1: Matrix Multiplication using Multithreading
Written By: Team
1- Byron A. Craig IV
2- Ryan Robertson
Submitted on: 10/15/2013
-------------------------------------------------------------------------------*/
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define ARR_SIZE 100
#define DEFAULT 10

/**
 * struct to form the matrices rows and columns
 */
struct matrix {
	int x; // rows
	int y; // columns
}; // end struct

void *run_thread (void *p); // Thread Run Method

// Global Variables
int Ax = 0;
int Ay = 0;
int Asize = 0;
int Bx = 0;
int By = 0;
int Bsize = 0;
int Cx = 0;
int Cy = 0;
int Csize = 0;
int dataIn = 0;
int i = 0;
int j = 0;
int integer = 0;
int A [DEFAULT][DEFAULT] = {};
int B [DEFAULT][DEFAULT] = {};
int C [DEFAULT][DEFAULT] = {};

/**
 * dataFile_input - Reads the matrices from the data file into 2 dimensional
 *					arrays and then formats the matrices for output.
 */
int 
multiply_matrices () {
	// Unassigned local Variables
    FILE *inFile;
	FILE *outFile;

	// Assigned local variables
    int count1 = 0;
    int count2 = 0;
    int data[ARR_SIZE];
    
    // Opening the file
    inFile = fopen("input.dat", "r");
    
    /*----------------------------------
    Reading the file to get the Matrices
    Testing for size of Matrices to make
    sure it is not too big.
    ----------------------------------*/
    while (fscanf(inFile, "%d", &dataIn) != EOF) {
        if (count1 == 0){
            Ax = dataIn;
            // Size testing
            if(Ax > 30){
                printf("Matrix Size is too big!\n");
                return 1;
            } // end if
        } // end if
        else if(count1 == 1){
            Ay = dataIn;
	    // Size testing
            if(Ay > 30){
                printf("Matrix Size is too big!\n");
                return 1; // end program
            }// end if
        } // end else if
        else if (count1 > 1) {
            data[count2] = dataIn;
            count2++;
        } // end else if
        count1++;
    } // end while
    
    /*------------------------------------
    Obtaining both Matrix A and Matrix B
    sizes while getting the x and y values
    for both.
    ------------------------------------*/
    Asize = Ax * Ay;
    Bx = data[Asize];
    By = data[Asize + 1];
    Bsize = Bx * By;

	/*------------------------------------
	Here is where the data for Matrix A is
	put into a 2-dimensional array to form
	the matrix itself.
	------------------------------------*/
	A [Ax][Ay];
	count1 = 0;
	for (i = 0; i < Ax; i++) {
    	for (j = 0; j < Ay; j++) {
			A [i][j] = data[count1];
			count1++;
		} // end for
	} // end for

	/*------------------------------------
	Here is where the data for Matrix B is
	put into a 2-dimensional array to form
	the matrix itself.
	------------------------------------*/
	B [Bx][By];
	count1 = (Asize + 2);
	for (i = 0; i < Bx; i++) {
		for (j = 0; j < By; j++) {
			B [i][j] = data[count1];
			count1++;
		} // end for
	} // end for

    // Size testing if statement
    if(Bx > 30 | By > 30){
        printf("Matrix Size is too big!\n");
        return 1;
    } // end if
    
    /*--------------------------------------
    Formatting the outputs for both Matrix A
    and Matrix B.
    --------------------------------------*/
	outFile = fopen("output.dat", "w");
	printf ("A[ %d][ %d] = {\n", Ax, Ay);
	fprintf (outFile, "A[ %d][ %d] = {\n", Ax, Ay);
    for (i = 0; i < Ax; i++) {
		for (j = 0; j < Ay; j++) {
			printf ("%8d", A [i][j]);
			fprintf (outFile, "%8d", A [i][j]);
		} // end for
		printf ("\n");
		fprintf (outFile, "\n");
	} // end for
	printf ("}\n\n");
	fprintf (outFile, "}\n\n");
	
	// Matrix B print output
	printf ("B[ %d][ %d] = {\n", Bx, By);
	fprintf (outFile, "B[ %d][ %d] = {\n", Bx, By);
    for (i = 0; i < Bx; i++) {
		for (j = 0; j < By; j++) {
			printf ("%8d", B [i][j]);
			fprintf (outFile, "%8d", B [i][j]);
		} // end for
		printf ("\n");
		fprintf (outFile, "\n");
	} // end for
	printf ("}\n\n");
	fprintf (outFile, "}\n\n");

	/*------------------------------------------
	Obtaining the number of rows and columns for
	Matrix C and also getting the size of the C.
	------------------------------------------*/
	Cx = Ax;
	Cy = By;
	Csize = Cx * Cy;
	C [Cx][Cy];
	integer = Ay;

	/*------------------------------------------------------
	Creating the threads, assigning a row and column to each
	thread, then passing it data. pthread_join must wait for
	all threads to complete before the join occurs.
	------------------------------------------------------*/
	count1 = 0;
	for (i = 0; i < Cx; i++) {
		for (j = 0; j < Cy; j++) {
			struct matrix *m = (struct matrix*) malloc(sizeof(struct matrix));
			m->x = i;
			m->y = j;
			// Thread ID
			pthread_t thread_id; 
			// Set attributes
			pthread_attr_t thread_attr; 
			// Get attributes
			pthread_attr_init(&thread_attr);
			// Thread creation
			pthread_create(&thread_id, &thread_attr, run_thread, m);
			// Thread join
			pthread_join(thread_id, NULL); 
			count1++;
		} // end for
	} // end for

	/*--------------------------------------------------
	Formatting the final output for the product Matrix C
	and confirming that all threads were terminated.
	--------------------------------------------------*/
	printf ("### ALL threads have terminated\n\n");
	printf ("The matrix product C[ %d][ %d] = {\n", Cx, Cy);
	fprintf (outFile, "C[ %d][ %d] = {\n", Cx, Cy);
	for (i = 0; i < Cx; i++) {
		for (j = 0; j < Cy; j++) {
			printf ("%8d", C [i][j]);
			fprintf (outFile, "%8d", C [i][j]);
		} // end for
		printf ("\n");
		fprintf (outFile, "\n");
	} // end for
	printf ("}");
	fprintf (outFile, "}");

	fclose(inFile);
	fclose(outFile);
    return 0;
} // end dataFile_input	

/**
 * runner - Completes all of the math using threads. This method gets the
 *			products to create the final product -- Matrix C
 */
void *run_thread (void *p) {
	// Assigned local variables	
	struct matrix *m = p;
	int t = 0;
	int sum = 0;

	// Getting the products and adding them
	// together.
	for (t = 0; t < integer; t++) {
		sum += A [m->x][t] * B [t][m->y];
	} // end for

	/*--------------------------------
	Sending the sum values to Matrix C
	and then exit all of the threads.
	--------------------------------*/
	C [m->x][m->y] = sum;
	pthread_exit(0);	
} // end runner

/**
 * main - Main method that runs the entire programming assignment.
 */
int 
main () {
	multiply_matrices();
} // end main
