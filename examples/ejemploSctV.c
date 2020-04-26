#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void inicializarVector(int numItems, int** vec){
	*vec = malloc(sizeof(int)*numItems);
	for(int i=0; i<numItems; i++)
		(*vec)[i] = i;
}

void distribuirCargas(int numItems, int numProcs, int** desplazamientos, int** cuentas){
	*desplazamientos = malloc(sizeof(int)*numProcs);
	*cuentas = malloc(sizeof(int)*numProcs);
	int paquete = numItems / numProcs;
	int excedente = numItems % numProcs;
	for(int i = 0; i<numProcs; i++){
		(*desplazamientos)[i] = (i*paquete + (i<excedente?i:excedente));//Donde empezaria el trozo de cada proceso segun su ID
		(*cuentas)[i] = (paquete + (i<excedente));
	}
}

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv);
	int myID = 0, numProcs = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myID);
	
	if(argc!=2){
		if(myID==0) 
			printf("Se requiere el numero de elementos como parÃ¡metro\n");
	}else{
		int numItems = atoi(argv[1]);//printf("ID: %d / %d -> %d\n", myID, numProcs-1, numItems);//Todos mostramos que proceso somos y sabemos el total de items
		int* vector = 0;
		int* desplazamientos = 0;
		int* cuentas = 0;
		distribuirCargas(numItems, numProcs, &desplazamientos, &cuentas);
		if(myID==0){//https://www.mpich.org/static/docs/v3.1/www3/MPI_Scatterv.html
			inicializarVector(numItems, &vector);//Yo, el proceso 0, creo todo el vector y escribo su contenido
			MPI_Scatterv(vector, cuentas, desplazamientos, MPI_INT, MPI_IN_PLACE, cuentas[myID], MPI_INT, 0, MPI_COMM_WORLD);
		}else{
			vector = malloc(sizeof(int)*cuentas[myID]);//Yo, como proceso estandar, solo pido espacio para lo que necesito
			MPI_Scatterv(vector, cuentas, desplazamientos, MPI_INT, vector, cuentas[myID], MPI_INT, 0, MPI_COMM_WORLD);//El envio nos es irrelevante realmente desde la perspectiva de NO-MASTER		
		}//Aqui, cada proceso deberia tener su parte:
		for(int i = 0; i<cuentas[myID]; i++){//Empezamos logicamente en 0
			printf("[%d] -> %d\n", myID, vector[i]);
		}		
		free(vector);
		free(desplazamientos);
		free(cuentas);
	}

	MPI_Finalize();
}