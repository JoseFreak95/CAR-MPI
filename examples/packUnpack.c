#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG 0
#define DEST 1
#define SOURCE 0
 
int main(int argc, char* argv[]) {
	int   my_rank;
	char  *buffer;
	int   posicion;
	MPI_Status  status;
	float a, b;
	int   n;
	int NBytes;
 
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 
	NBytes = 2*sizeof(float)+sizeof(int);
	buffer=(char*)malloc(NBytes*sizeof(char)); 

	printf(" NÃºmero de bytes = %i \n", NBytes);
	if ( my_rank == SOURCE ) {
		a = 10.3;
		b = 11.2;
		n = 4;
      		posicion = 0;
      		MPI_Pack(&a,1,MPI_FLOAT,buffer,NBytes,&posicion,MPI_COMM_WORLD);
      		MPI_Pack(&b,1,MPI_FLOAT,buffer,NBytes,&posicion,MPI_COMM_WORLD);
      		MPI_Pack(&n,1,MPI_INT,buffer,NBytes,&posicion,MPI_COMM_WORLD);
      		MPI_Send(buffer,NBytes,MPI_PACKED,DEST,TAG,MPI_COMM_WORLD);
	}else{
      		MPI_Recv(buffer,NBytes,MPI_PACKED,SOURCE,TAG,MPI_COMM_WORLD,&status);
		    posicion = 0;
      		MPI_Unpack(buffer,NBytes,&posicion,&a,1,MPI_FLOAT,MPI_COMM_WORLD);
      		printf("   a = %f\n",a);
      		MPI_Unpack(buffer,NBytes,&posicion,&b,1,MPI_FLOAT,MPI_COMM_WORLD);
      		printf("   b = %f\n",b);
      		MPI_Unpack(buffer,NBytes,&posicion,&n,1,MPI_INT,MPI_COMM_WORLD);
      		printf("   n = %d\n",n);
   	}
   	MPI_Finalize();
}