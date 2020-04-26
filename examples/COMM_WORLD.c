// (UAL) Adaptado de: www.mpitutorial.com
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
	MPI_Init(NULL, NULL);

	// Se obtiene el ranking y el numero de procesos del comunicador global.
	int world_rank, world_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Se obtiene el grupo de procesadores del comunicador MPI_COMM_WORLD
	MPI_Group world_group;
	MPI_Comm_group(MPI_COMM_WORLD, &world_group);

	int n = 3;
	const int ranks[3] = {1, 2,3};

	// Se crea un grupo que contiene los procesadores primos del comunicador MPI_COMM_WORLD
	MPI_Group prime_group;
	MPI_Group_incl(world_group, 3, ranks, &prime_group);

	// Se crea un nuevo grupo a partir del comunicador anterior
	MPI_Comm prime_comm;
	MPI_Comm_create_group(MPI_COMM_WORLD, prime_group, 0, &prime_comm);

	int prime_rank = -1, prime_size = -1;
	// Si el rank no esta en el nuevo comunicador, sera MPI_COMM_NULL
	// Usar MPI_COMM_NULL para MPI_comm_rank o MPI_Comm_size es erroneo
	if (MPI_COMM_NULL != prime_comm) {
		MPI_Comm_rank(prime_comm, &prime_rank);
		MPI_Comm_size(prime_comm, &prime_size);
	}

	printf("WORLD RANK/SIZE: %d/%d --- PRIME RANK/SIZE: %d/%d\n", world_rank, world_size, prime_rank, prime_size);

  	MPI_Group_free(&world_group);
	MPI_Group_free(&prime_group);

	if (MPI_COMM_NULL != prime_comm) {
    		MPI_Comm_free(&prime_comm);
  	}

	MPI_Finalize();
}