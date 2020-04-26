#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv);
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if(size!=2){
		if(rank==0){
			printf("Se esperaban 2 procesos\n"); fflush(stdout);
		}
	}else{
		int* vec = 0;//Null
		const int cant = 5;
		if(rank==0){
			vec = malloc(sizeof(int)*cant);//5 enteros, mandemosle los DOS ultimos al proceso 1
			for(int i = 0; i<cant; i++){
				vec[i]=i;
			}
			MPI_Send(&(vec[3]), 2, MPI_INT, 1, 1, MPI_COMM_WORLD);//Yo, como 0, me quedo los 3 primeros: posiciones vec[0], vec[1] y vec[2]. Envio desde la 3 (incluida)
		}else{
			MPI_Status status;
			vec = malloc(sizeof(int)*2);//Sabemos que vamos a tener 2 enteros
			MPI_Recv(vec, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);//Mi "vec" es otro vector / buffer propio, lo enfocamos desde 0 // vec == &(vec[0])
			//Mostramos lo recibido			
			for(int i = 0; i<2; i++){
				printf("[%d]: %d\n", rank, vec[i]);
				fflush(stdout);
			}		
		}
		free(vec);
	}
	MPI_Finalize();
	return 0;
}
