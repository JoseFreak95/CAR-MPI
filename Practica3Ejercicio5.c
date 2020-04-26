#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define RANK_0 0
#define RANK_1 1
#define NUMBER_LIMIT 10

int main(int argc, char **argv)
{
    srand(time(NULL));
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2)
    {
        printf("Se esperaban 2 procesos\n");
    }
    else
    {
        if (rank == RANK_0)
        {
            MPI_Status status;
            int * vector = malloc(sizeof(int) * NUMBER_LIMIT);

            for (int i = 1; i <= NUMBER_LIMIT; i++)
            {
                MPI_Send(&i, 1, MPI_INT, RANK_1, 10, MPI_COMM_WORLD);
            }

            MPI_Recv(vector, NUMBER_LIMIT, MPI_INT, RANK_1, 20, MPI_COMM_WORLD, &status);

            printf("El cuadrado de los 10 primeros numeros naturales es: ");
            for (int i = 0; i < NUMBER_LIMIT; i++)
            {
                printf("%d ", vector[i]);
            }
            printf("\n");
            free(vector);
        }
        else
        {
            MPI_Status status;
            int number = 0;
            int *vector = malloc(sizeof(int) * NUMBER_LIMIT);

            for (int i = 0; i < NUMBER_LIMIT; i++)
            {
                MPI_Recv(&number, 1, MPI_INT, RANK_0, 10, MPI_COMM_WORLD, &status);
                vector[i] = number * number;
            }

            MPI_Send(vector, NUMBER_LIMIT, MPI_INT, RANK_0, 20, MPI_COMM_WORLD);
            free(vector);
        }
    }
    MPI_Finalize();
    return 0;
}