#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define RANK_0 0
#define RANK_1 1
#define RANK_2 2
#define RANK_3 3
#define MAX_NAME_SZ       256 
#define DOUBLE_VECTOR_SZ  10
#define DOUBLE_VEC_RESULT 2
#define TEXT_EJER3_SZ     30

double *crearVectorAleatorio(int tam)
{
    double *vec = malloc(sizeof(double) * tam);
    for (int i = 0; i < tam; i++)
        vec[i] = rand() % 66;
    return vec;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 4)
    {
        printf("Se esperaban 4 procesos\n");
        exit(0);
    }

    if (rank == RANK_0)
    {
        MPI_Status status;
        int option;
        do
        {
            printf("Bienvenido al panel del control del ejecicio 6!\n");
            printf("Estas son las opciones disponibles para la sesion de hoy:\n\t");
            printf("1.-Convertir un texto en mayusculas.\n\t");
            printf("2.-Sumar los elementos de un vector y calcular su raiz cuadrada.\n\t");
            printf("3.-Hallar el valor numerico de las letras de una frase predeterminada y devolver su suma.\n\t");
            printf("4.-Ejecutar todas las opciones anteriores a la vez.\n\t");
            printf("0.-Salir del programa.\n");
            printf("Por favor elija alguna de las opciones:\n");
            scanf("%d", &option);
            printf("\n");

            if (option == 1 || option == 4)
            {
                char temp;
                char *text_r0 = (char *)malloc(sizeof(char) * MAX_NAME_SZ);

                MPI_Send(&option, 1, MPI_INT, RANK_1, 50, MPI_COMM_WORLD);

                printf("Introduzca el texto que desea convertir a mayusculas:\n");
                scanf("%c", &temp);
                scanf("%[^\n]", text_r0);
                int len = strlen(text_r0);
                MPI_Send(text_r0, len, MPI_CHAR, RANK_1, 11, MPI_COMM_WORLD);
                printf("Nuestros lemmings estan trabajando en ello, por favor espere...\n");
                MPI_Recv(text_r0, len, MPI_CHAR, RANK_1, 12, MPI_COMM_WORLD, &status);
                printf("Su texto en mayusculas es: %s\n", text_r0);

                free(text_r0);
            }

            if (option == 2 || option == 4)
            {
                double sum_vector = .0, square_root = .0;
                double *vector = crearVectorAleatorio(DOUBLE_VECTOR_SZ);
                double *resultado = crearVectorAleatorio(DOUBLE_VEC_RESULT);

                MPI_Send(&option, 1, MPI_INT, RANK_2, 50, MPI_COMM_WORLD);

                printf("Generando vector de numeros reales...\n");
                MPI_Send(vector, DOUBLE_VECTOR_SZ, MPI_DOUBLE, RANK_2, 21, MPI_COMM_WORLD);
                printf("Nuestros lemmings estan trabajando en ello, por favor espere...\n");
                MPI_Recv(resultado, DOUBLE_VEC_RESULT, MPI_DOUBLE, RANK_2, 22, MPI_COMM_WORLD, &status);
                printf("La suma de los elementos del vector es: %.2f\n", resultado[0]);
                printf("La raiz cuadrada de la suma de los elementos del vector es: %.2f\n", resultado[1]);

                free(vector);
                free(resultado);
            }

            if (option == 3 || option == 4)
            {
                int value_text_r3 = 0;
                char *text_r3 = "Entrando en funcionalidad 3";

                MPI_Send(&option, 1, MPI_INT, RANK_3, 50, MPI_COMM_WORLD);

                printf("El texto predefinido es:\n");
                printf("\"%s\"\n", text_r3);
                MPI_Send(text_r3, TEXT_EJER3_SZ, MPI_CHAR, RANK_3, 31, MPI_COMM_WORLD);
                printf("Nuestros lemmings estan trabajando en ello, por favor espere...\n");
                MPI_Recv(&value_text_r3, 1, MPI_INT, RANK_3, 32, MPI_COMM_WORLD, &status);
                printf("La sumatoria del valor de las letras del texto es: %d\n", value_text_r3);
            }

            if (option == 0)
            {
                MPI_Send(&option, 1, MPI_INT, RANK_1, 50, MPI_COMM_WORLD);
                MPI_Send(&option, 1, MPI_INT, RANK_2, 50, MPI_COMM_WORLD);
                MPI_Send(&option, 1, MPI_INT, RANK_3, 50, MPI_COMM_WORLD);
            }
        } while (option != 0);
    }
    else if (rank == RANK_1)
    {
        MPI_Status status;
        int opt;
        do
        {
            MPI_Recv(&opt, 1, MPI_INT, RANK_0, 50, MPI_COMM_WORLD, &status);
            if (opt != 0)
            {
                int text_len = 0;

                MPI_Probe(RANK_0, 11, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_CHAR, &text_len);

                char *text_r1 = (char *)malloc(sizeof(char) * text_len);

                MPI_Recv(text_r1, text_len, MPI_CHAR, RANK_0, 11, MPI_COMM_WORLD, &status);
                for (int i = 0; i < text_len; i++)
                {
                    text_r1[i] = toupper(text_r1[i]);
                }
                MPI_Send(text_r1, text_len, MPI_CHAR, RANK_0, 12, MPI_COMM_WORLD);
                free(text_r1);
            }
        } while (opt != 0);
    }
    else if (rank == RANK_2)
    {
        MPI_Status status;
        int opt;
        do
        {
            MPI_Recv(&opt, 1, MPI_INT, RANK_0, 50, MPI_COMM_WORLD, &status);
            if (opt != 0)
            {
                double sum_vector = .0, square_root = .0;
                double *vec = malloc(sizeof(double) * DOUBLE_VECTOR_SZ);
                double *resultado = malloc(sizeof(double) * DOUBLE_VEC_RESULT);

                MPI_Recv(vec, DOUBLE_VECTOR_SZ, MPI_DOUBLE, RANK_0, 21, MPI_COMM_WORLD, &status);

                for (int i = 0; i < DOUBLE_VECTOR_SZ; i++)
                {
                    sum_vector += vec[i];
                }
                square_root = sqrt(sum_vector);

                resultado[0] = sum_vector;
                resultado[1] = square_root;
                MPI_Send(resultado, DOUBLE_VEC_RESULT, MPI_DOUBLE, RANK_0, 22, MPI_COMM_WORLD);
                free(vec);
                free(resultado);
            }
        } while (opt != 0);
    }
    else if (rank == RANK_3)
    {
        MPI_Status status;
        int opt;
        do
        {
            MPI_Recv(&opt, 1, MPI_INT, RANK_0, 50, MPI_COMM_WORLD, &status);
            if (opt != 0)
            {
                int text_len = 0, sum_value_char = 0;

                MPI_Probe(RANK_0, 31, MPI_COMM_WORLD, &status);
                MPI_Get_count(&status, MPI_CHAR, &text_len);

                char *text_r3 = (char *)malloc(sizeof(char) * text_len);

                MPI_Recv(text_r3, text_len, MPI_CHAR, RANK_0, 31, MPI_COMM_WORLD, &status);
                for (int i = 0; i < text_len; i++)
                {
                    sum_value_char += text_r3[i];
                }
                MPI_Send(&sum_value_char, 1, MPI_INT, RANK_0, 32, MPI_COMM_WORLD);
                free(text_r3);
            }
        } while (opt != 0);
    }
    MPI_Finalize();
    return 0;
}
