#include <stdio.h>
#include "mpi.h"
#include <unistd.h>// para sleep

int main( int argc, char *argv[] ){
	double t1, t2;
	MPI_Init(0, 0);
	t1 = MPI_Wtime();
	sleep(2);
	t2 = MPI_Wtime();
	printf("MPI_Wtime measured a 2 second sleep to be: %1.2f\n", t2-t1);
	MPI_Finalize();
	return 0;
}