#include<stdio.h>
#include<mpi.h>

int main( int argc, char* argv[] ) {
	int rank, size;
  
	// Paralelismo
	MPI_Init( &argc, &argv );
	// Indice del proceso
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	// Tamano del comunicador seleccionado
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	printf( "Hola mundo! Soy el proceso numero %d. En total somos %d procesos.\n", rank, size );
 
	MPI_Finalize();
}