/*
 ============================================================================
 Name        : mpiio-write.c
 Author      : Hariharan
 ============================================================================

 *****************************************************************************
 * *
 * MPI IO Example - Writing to MPI Files  *
 * *
 * Each of the processes write a specified number of blocks to a *
 * single output file. *
 * *
 *****************************************************************************/
#include <stdio.h> /* all IO stuff lives here */
#include <stdlib.h> /* exit lives here */
#include <unistd.h> /* getopt lives here */
#include <string.h> /* strcpy lives here */
#include <mpi.h> /* MPI and MPI-IO live here */

#define MASTER_RANK 0
#define TRUE 1
#define FALSE 0
#define BOOLEAN int
#define BLOCK_SIZE 1048576
#define MBYTE 1048576
#define SYNOPSIS printf ("synopsis: %s -f <file> -l <blocks> -o <outputfileName>\n", argv[0])
static void handle_error(int errcode, char *str) {
	char msg[MPI_MAX_ERROR_STRING];
	int resultlen;
	MPI_Error_string(errcode, msg, &resultlen);
	fprintf(stderr, "%s: %s\n", str, msg);
	/* Aborting on error might be too aggressive.  If
	 * you're sure you can
	 * continue after an error, comment or remove
	 * the following line */
	MPI_Abort(MPI_COMM_WORLD, 1);
}

#define MPI_CHECK(fn) { int errcode; errcode = (fn); \
    if (errcode != MPI_SUCCESS) handle_error(errcode, #fn ); }
int main(argc, argv)
	int argc;char *argv[]; {
	/* my variables */

	int my_rank, pool_size, number_of_blocks = 0, i, count;
	BOOLEAN i_am_the_master = FALSE, input_error = FALSE;
	char *filename = NULL;
	char *outputFileName = NULL;
	int filename_length;
	int *junk;
	int number_of_integers, number_of_bytes;
	long long total_number_of_integers, total_number_of_bytes;
	MPI_Offset my_offset, my_current_offset;
	MPI_File fh;
	MPI_Status status;
	/* getopt variables */

	extern char *optarg;
	int c;
	/* read the command line */

	while ((c = getopt(argc, argv, "f:l:o:h")) != EOF)
		switch (c) {
		case 'f':
			filename = optarg;
#ifdef DEBUG
			printf("output file: %s\n", filename);
#endif
			break;
		case 'l':
			if ((sscanf(optarg, "%d", &number_of_blocks) != 1)
					|| (number_of_blocks < 1)) {
				SYNOPSIS;
				input_error = TRUE;
			}
#ifdef DEBUG
			else
			printf("each process will write %d blocks of integers\n",
					number_of_blocks);
#endif
			break;
		case 'o':
			outputFileName = optarg;
			freopen(outputFileName, "w", stdout);
#ifdef DEBUG
			printf("output file: %s\n", outputFileName);
#endif
			break;
		case 'h':
			SYNOPSIS;
			input_error = TRUE;
			break;
		case '?':
			SYNOPSIS;
			input_error = TRUE;
			break;
		}

	/* Check if the command line has initialized filename and
	 * number_of_blocks.
	 */

	if ((filename == NULL) || (number_of_blocks == 0)) {
		SYNOPSIS;
		input_error = TRUE;
	}

	if (input_error)
		MPI_CHECK(MPI_Abort(MPI_COMM_WORLD, 1));
	/* This is another way of exiting, but it can be done only
	 if no files have been opened yet. */

	filename_length = strlen(filename) + 1;
	double start, finish, io_time, longest_io_time;

	/* ACTION */

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &pool_size);
	if (my_rank == MASTER_RANK)
		i_am_the_master = TRUE;
	MPI_CHECK(MPI_Bcast(&number_of_blocks, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD));
	MPI_CHECK(MPI_Bcast(&filename_length, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD));
	if (!i_am_the_master)
		filename = (char*) malloc(filename_length);
	MPI_CHECK(MPI_Bcast(filename, filename_length, MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD));
#ifdef DEBUG
	printf("%3d: received broadcast\n", my_rank);
	printf("%3d: filename = %s\n", my_rank, filename);
#endif

	number_of_integers = (number_of_blocks * BLOCK_SIZE) / (sizeof(int));
	number_of_bytes = sizeof(int) * number_of_integers;

	/* number_of_bytes must be just plain integer, because we are
	 going to use it in malloc */

	total_number_of_integers = (long long) pool_size
			* (long long) number_of_integers;
	total_number_of_bytes = (long long) pool_size * (long long) number_of_bytes;
	my_offset = (long long) my_rank * (long long) number_of_bytes;

#ifdef DEBUG
	if (i_am_the_master) {
		printf("number_of_bytes = %d/process\n", number_of_bytes);
		printf("total_number_of_bytes = %lld\n", total_number_of_bytes);
		printf("size of offset = %d bytes\n", sizeof(MPI_Offset));
	}
#endif

	MPI_CHECK(MPI_File_open(MPI_COMM_WORLD, filename,
	MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh));
	MPI_CHECK(MPI_File_seek(fh, my_offset, MPI_SEEK_SET));
	MPI_CHECK(MPI_File_get_position(fh, &my_current_offset));
#ifdef DEBUG
	printf ("%3d: my current offset is %lld\n", my_rank, my_current_offset);
#endif

	/* generate random integers */

	junk = (int*) malloc(number_of_bytes);
	srand(28 + my_rank);
	for (i = 0; i < number_of_integers; i++)
		*(junk + i) = rand();

	/* write the stuff out */

	start = MPI_Wtime();
	MPI_CHECK(MPI_File_write(fh, junk, number_of_integers, MPI_INT, &status));
	finish = MPI_Wtime();
	io_time = finish - start;
	MPI_CHECK(MPI_Get_count(&status, MPI_INT, &count));
#ifdef DEBUG
	printf("%3d: wrote %d integers\n", my_rank, count);
#endif
	MPI_CHECK(MPI_File_get_position(fh, &my_current_offset));
#ifdef DEBUG
	printf ("%3d: my current offset is %lld\n", my_rank, my_current_offset);
#endif
	MPI_File_close(&fh);

	MPI_Allreduce(&io_time, &longest_io_time, 1, MPI_DOUBLE, MPI_MAX,
	MPI_COMM_WORLD);

	if (i_am_the_master) {
		printf("longest_io_time = %f seconds\n", longest_io_time);
		printf("total_number_of_bytes = %lld\n", total_number_of_bytes);
		printf("transfer rate = %f MB/s\n",
				total_number_of_bytes / longest_io_time / MBYTE);
	}

	MPI_Finalize();
	exit(0);
}
