#include <mpi.h>
#include <stdio.h> //Para el printf
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define RANK_0 0
#define RANK_1 1
#define RANK_2 2
#define RANK_3 3

int *generarMatrizAleatoria(int tam)
{
    int *vec = malloc(sizeof(int) * tam);
    for (int i = 0; i < tam; i++)
        vec[i] = rand() % 66;
    return vec;
}

void distribuirCargas(int numItems, int numProcs, int *desplazamientos, int *cuentas)
{
    int paquete = numItems / numProcs;
    int excedente = numItems % numProcs;
    for (int i = 0; i < numProcs; i++)
    {
        desplazamientos[i] = (i * paquete + (i < excedente ? i : excedente));
        cuentas[i] = (paquete + (i < excedente));
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    int rank, nProcesos;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesos);

    if (argc != 3)
    {
        if (rank == RANK_0)
            printf("Se requiere el numero de elementos como parÃ¡metro\n");
    }
    else
    {
        if (rank == RANK_0)
        {
            MPI_Status status;
            int rows = atoi(argv[1]);
            int columns = atoi(argv[2]);
            int tamMatix = rows * columns;
            int *matriz = generarMatrizAleatoria(tamMatix);
            int *desplazamientos = malloc(sizeof(int) * nProcesos);
            int *cuentas = malloc(sizeof(int) * nProcesos);
            int sumaTotal = 0;

            printf("Elementos de la matriz\n");
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < columns; j++)
                {
                    printf("%d\t", matriz[(i * rows)+j]);
                }
                printf("\n");
            }

            distribuirCargas(tamMatix, nProcesos, desplazamientos, cuentas);

            for (int i = RANK_1; i < nProcesos; i++)
            {
                MPI_Send((matriz + desplazamientos[i]), cuentas[i], MPI_INT, i, 10, MPI_COMM_WORLD);
            }

            for (int i = 0; i < cuentas[RANK_0]; i++)
            {
                sumaTotal += matriz[i];
            }

            for (int i = RANK_1; i < nProcesos; i++)
            {
                int sumaAux = 0;
                MPI_Recv(&sumaAux, 1, MPI_INT, i, 20, MPI_COMM_WORLD, &status);
                sumaTotal += sumaAux;
            }

            printf("Suma total de los elementos de la matriz: %d\n", sumaTotal);
        }
        else
        {
            MPI_Status status;
            int data_len = 0;
            int vector_len = 0;
            int sumaLocal = 0;

            MPI_Probe(RANK_0, 10, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &data_len);

            int *data = malloc(sizeof(int) * data_len);

            MPI_Recv(data, data_len, MPI_INT, RANK_0, 10, MPI_COMM_WORLD, &status);

            for (int i = 0; i < data_len; i++)
            {
                sumaLocal += data[i];
            }

            MPI_Send(&sumaLocal, 1, MPI_INT, RANK_0, 20, MPI_COMM_WORLD);
            free(data);
        }
    }
    MPI_Finalize();
    return 0;
}