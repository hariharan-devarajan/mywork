/* Gaussian elimination without pivoting.
 * Compile with "gcc gauss.c"
 */

/* ****** ADD YOUR CODE AT THE END OF THIS FILE. ******
 * You need not submit the provided code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
#include <execinfo.h>
#include <signal.h>
#include <fstream>
#include <string.h>
/* Program Parameters */
#define MAXN 2000
/* Max value of N */
/* Matrix size */
/* Matrices and vectors */

/* A * X = B, solve for X */
volatile float A[MAXN][MAXN], B[MAXN], X[MAXN];
int N;

char* fileName;

/* Prototype */
void forwardElemination(int rank, int worldSize);
void backwardSubstitution();
/* The function you will provide.
 * It is this routine that is timed.
 * It is called only on the parent.
 */

/* returns a seed for srand based on the time */
unsigned int time_seed() {
	struct timeval t;
	struct timezone tzdummy;

	gettimeofday(&t, &tzdummy);
	return (unsigned int) (t.tv_usec);
}

/* Set the program parameters from the command-line arguments */
/* Set the program parameters from the command-line arguments */
void parameters(int argc, char **argv) {
	int seed = 0; /* Random seed */
	char uid[32]; /*User name */

	/* Read command-line arguments */
	srand(time_seed()); /* Randomize */
	bool isSet=false;

	if(argc >=2){
		N = atoi(argv[1]);
		if (N < 1 || N > MAXN) {
			printf("N = %i is out of range.\n", N);
			exit(0);
		}
		isSet=true;
	}
	if (argc >= 3) {
		seed = atoi(argv[2]);
		srand(seed);
		isSet=true;

	}else{
		isSet=false;
	}
	if (argc >= 4) {
		freopen(argv[3], "w", stdout);

	}
	if (!isSet) {
		printf("Usage: %s <matrix_dimension> [random seed] [log file]<>\n", argv[0]);
		exit(0);
	}

	/* Print parameters */
	printf("\nMatrix dimension N = %i.\n", N);
	printf("Random seed = %i\n", seed);
}

/* Initialize A and B (and X to 0.0s) */
void initialize_inputs() {
	int row, col;

	printf("\nInitializing...\n");
	for (col = 0; col < N; col++) {
		for (row = 0; row < N; row++) {
			A[row][col] = (float) rand() / 32768.0;
		}
		B[col] = (float) rand() / 32768.0;
		X[col] = 0.0;
	}

}
/* Print input matrices */
void print_inputs() {
	int row, col;

	printf("\nN =%d\n\t", N);
	if (N < 10) {

		printf("\nA =\n\t");
		for (row = 0; row < N; row++) {
			for (col = 0; col < N; col++) {
				printf("%5.2f%s", A[row][col],
						(col < N - 1) ? ", " : ";\n\t");
			}
		}
		printf("\nB = [");
		for (col = 0; col < N; col++) {
			printf("%5.2f%s", B[col], (col < N - 1) ? "; " : "]\n");
		}
	}
}
void handler(int sig) {
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}
void print_X() {
	int row;

	if (N < 100) {
		printf("\nX = [");
		for (row = 0; row < N; row++) {
			printf("%5.2f%s", X[row], (row < N - 1) ? "; " : "]\n");
		}
	}
}
int main(int argc, char **argv) {
	parameters(argc, argv);
	printf("Parallel MPI");
	signal(SIGSEGV, handler);
	/* Timing variables */
	const int tagForSendMainData = 13;
	MPI_Init(&argc, &argv);
	struct timeval etstart, etstop; /* Elapsed times using gettimeofday() */
	struct timezone tzdummy;
	clock_t etstart2, etstop2; /* Elapsed times using times() */
	unsigned long long usecstart, usecstop;
	struct tms cputstart, cputstop; /* CPU times for my processes */
	int worldRank;
	MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
	int worldSize;
	MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

	if (worldRank == 0) {
		printf("\nEntered root processor\n");
		/* Process program parameters */
		printf("\nRank : %d\n", worldRank);
		printf("\nSize : %d\n", worldSize);

		/* Initialize A and B */

		initialize_inputs();

		/* Print input matrices */
		print_inputs();
		/* Start Clock */

		printf("\nStarting clock.\n");
		gettimeofday(&etstart, &tzdummy);
		etstart2 = times(&cputstart);
		printf("\nGet Data from all other processors for merging\n");
	}

	printf("\nBroadcast data to all processors\n");

	forwardElemination(worldRank, worldSize);

	if (worldRank == 0) {
		printf("\nPerform Back Substitution\n");
		backwardSubstitution();
		print_inputs();
		/* Stop Clock */
		gettimeofday(&etstop, &tzdummy);
		etstop2 = times(&cputstop);

		printf("Stopped clock.\n");
		usecstart = (unsigned long long) etstart.tv_sec * 1000000
				+ etstart.tv_usec;
		usecstop = (unsigned long long) etstop.tv_sec * 1000000
				+ etstop.tv_usec;

		/* Display output */
		print_X();
		/* Display timing results */

		printf("\nElapsed time = %g ms.\n",
				(float) (usecstop - usecstart) / (float) 1000);

		printf("(CPU times are accurate to the nearest %g ms)\n",
				1.0 / (float) CLOCKS_PER_SEC * 1000.0);
		printf("My total CPU time for parent = %g ms.\n",
				(float) ((cputstop.tms_utime + cputstop.tms_stime)
						- (cputstart.tms_utime + cputstart.tms_stime))
						/ (float) CLOCKS_PER_SEC * 1000);
		printf("My system CPU time for parent = %g ms.\n",
				(float) (cputstop.tms_stime - cputstart.tms_stime)
						/ (float) CLOCKS_PER_SEC * 1000);
		printf("My total CPU time for child processes = %g ms.\n",
				(float) ((cputstop.tms_cutime + cputstop.tms_cstime)
						- (cputstart.tms_cutime + cputstart.tms_cstime))
						/ (float) CLOCKS_PER_SEC * 1000);
		/* Contrary to the man pages, this appears not to include the parent */
		printf("--------------------------------------------\n");
	}
	MPI_Finalize();
	fclose(stdout);

	exit(0);
}
void MergeArray(int *in, int *inout, int *len, MPI_Datatype *datatype) {
	for (int i = 0; i < *len; i++) {
		*inout = *in;
		in++;
		inout++;
	}
}

/* ------------------ Above Was Provided --------------------- */

/****** You will replace this routine with your own parallel version *******/
/* Provided global variables are MAXN, N, A[][], B[], and X[],
 * defined in the beginning of this code.  X[] is initialized to zeros.
 */
void forwardElemination(int worldRank, int worldSize) {
	int norm, row, col, n = 3; /* Normalization row, and zeroing
	 * element row and col */
	float multiplier;
	int root = 0;
	printf("Computing Serially.\n");

	/* Gaussian elimination */
	for (norm = 0; norm < N - 1; norm++) {
		/*Synchromise data between processes before next step of normalization*/
		MPI_Bcast((void*)&A[0][0], N * N, MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Bcast((void*)&B[0], N, MPI_FLOAT, 0, MPI_COMM_WORLD);
		for (int i = norm + 1 + worldRank % worldSize; i < N; i++) {
			multiplier = A[i][norm] / A[norm][norm];
			for (int j = norm; j < N; j++) {
				A[i][j] -= A[norm][norm] * multiplier;

			}
			B[i] -= A[norm][norm] * multiplier;
		}

	}
}
/* (Diagonal elements are not normalized to 1.  This is treated in back
 * substitution.)
 */
void backwardSubstitution() {
	int norm, row, col;
	/* Back substitution */
	for (row = N - 1; row >= 0; row--) {
		X[row] = B[row];
		for (col = N - 1; col > row; col--) {
			X[row] -= A[row][col] * X[col];
		}
		X[row] /= A[row][row];
	}
}
