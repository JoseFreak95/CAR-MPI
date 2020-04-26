#include <mpi.h>
#include <stddef.h> // Para la funcion offsetof
#include <stdio.h> //Para el printf
#include <stdlib.h>
#include <ctype.h>
// Este ejemplo crea una estructura, la rellena y manda el valor al resto
typedef struct{
	int entero;
	double doble;
} enteroDoble;

int main(int argc, char* argv[]){
	MPI_Init(&argc, &argv);
	
	MPI_Datatype MPI_ENTERO_DOBLE;//Este sera el nombre que tenga, queda inicializarlo
	int cuenta = 2;//2 campos
	int longitudes[] = {1, 1};//1 elemento de cada tipo interno
	MPI_Aint indices[2]; //Vector para registrar las posiciones relativas en memoria
	
	indices[0] = (MPI_Aint) offsetof(enteroDoble, entero);
	indices[1] = (MPI_Aint) offsetof(enteroDoble, doble);
    
	MPI_Datatype tipos_base[] = {MPI_INT, MPI_DOUBLE};
    
	MPI_Type_create_struct(cuenta, longitudes, indices, tipos_base, &MPI_ENTERO_DOBLE);//Ahora escribimos la informacion del tipo
	MPI_Type_commit(&MPI_ENTERO_DOBLE);//y lo registramos
	
	int id = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);//Leemos nuestra id
	
	enteroDoble miEstructura;//Vacia por defecto
	
	if(id==0){//Solo el proceso 0 la escribe
		miEstructura.entero = 6;
		miEstructura.doble = 3.0;
	}
	
	MPI_Bcast(&miEstructura, 1, MPI_ENTERO_DOBLE, 0, MPI_COMM_WORLD);//La enviamos al resto	
	if(id!=0){
		printf("Soy el proceso %d, esta es mi estructura recibida: [%d, %lf]\n", id, miEstructura.entero, miEstructura.doble);
	} 	
	MPI_Type_free(&MPI_ENTERO_DOBLE);//Liberamos el tipo
	MPI_Finalize();
	return 0;
}