#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv);
	int id;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	srand(1234);
	double randVal = ((double) rand() / RAND_MAX);
	printf("[%d] -> randVal: %.2lf\n", id, randVal);
	MPI_Finalize();
}