#include <mpi.h>
#include <stdio.h> //Para el printf
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define RANK_0 0
#define RANK_1 1
#define RANK_2 2
#define RANK_3 3

void **generarMatrizAleatoria(int rows, int columns, int sizeofTipo)
{ //Contigua
    int i;
    void **h;

    void *h_container = calloc(rows * columns, sizeofTipo);

    h = malloc(sizeof(void *) * rows);
    for (int i = 0; i < rows; i++)
    {
        h[i] = &(h_container[i * sizeofTipo * columns]);
    }

    return h;
}

void inicializarMatriz(int **matriz, int rows, int columns)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            matriz[i][j] = rand() % 66;
        }
    }
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
            int **matriz = (int**)generarMatrizAleatoria(rows, columns, sizeof(int));
            int **matrizTrasp = (int**)generarMatrizAleatoria(rows, columns, sizeof(int));
            int *desplazamientos = malloc(sizeof(int) * nProcesos);
            int *cuentas = malloc(sizeof(int) * nProcesos);
            inicializarMatriz(matriz, rows, columns);
            int sumaTotal = 0;

            printf("Elementos de la matriz\n");
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < columns; j++)
                {
                    printf("%d\t", matriz[i][j]);
                    matrizTrasp[j][i] = matriz[i][j];
                }
                printf("\n");
            }

            distribuirCargas(columns, nProcesos, desplazamientos, cuentas);

            for (int i = RANK_1; i < nProcesos; i++)
            {
                int rowVecLen = rows * cuentas[i];

                MPI_Send(matrizTrasp[desplazamientos[i]], rowVecLen, MPI_INT, i, 10, MPI_COMM_WORLD);
            }

            for (int i = 0; i < cuentas[RANK_0]; i++)
            {
                for (int j = 0; j < columns; j++)
                {
                    sumaTotal += matrizTrasp[i][j];
                }
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