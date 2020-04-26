/*
* Practica3Ejercicio1.c
* @author: Jose Carlos Fuentes Angulo
* @date: 26 Abril 2020
* @description: Programa en C para calcular el producto vectorial
* de dos vectores de forma paralela
*
* Teniendo en cuenta que no existe una limitación en número de procesadores que se tienen que
* utilizar, estableceremos un reparto equitativo respetando la localidad espacial de los 
* elementos del vector.
* La estrategia que vamos a usar es dividir ambos vectores en partes equitativas y asignar a
* cada procesador una parte del vector de ingresos y su correspondiente en del vector de 
* gastos, de esta forma cada procesador podrá calcular su diferencial.
*/

/*Incluimos las cabeceras necesarias*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

/*Definimos macros para cada procesador que se puede usar*/
#define RANK_0 0
#define RANK_1 1
#define RANK_2 2
#define RANK_3 3

/*
* @description: Funcion para generar vectores de enteros aleatorios
* @params: Tamaño del vector
* @return: Puntero al vector creado
*/
int * crearVectorAleatorio(int tam)
{
    int *vec = malloc(sizeof(int) * tam);
    for (int i = 0; i < tam; i++)
        vec[i] = rand() % 66;
    return vec;
}

/*
* @description: Funcion para calcular la carga que corresponde a cada procesador
* @params: Tamaño del vector, 
            numero de cores, 
            vector para los desplazamientos,
*           vector para los elementos que corresponden a cada procesador.
*/
void distribuirCargas(int numItems, int numProcs, int *desplazamientos, int *cuentas)
{
    int paquete = numItems / numProcs;      //Se calcula el tamaño de cada paquete de elementos
    int excedente = numItems % numProcs;    //Se calcula el sobrante de dividir en paquete iguales
    for (int i = 0; i < numProcs; i++)      //Bucle para asignar cada paquete de elementos a cada core
    {                                       //en caso de existir excedente se añade uno elemento más a cada paquete de datos
        desplazamientos[i] = (i * paquete + (i < excedente ? i : excedente));
        cuentas[i] = (paquete + (i < excedente));
    }
}

/*
* @description: Función principal
* @params: Entero que representa el de argumentos, Puntero al array de argumentos
* @return: Entero
*/
int main(int argc, char *argv[])
{
    srand(time(NULL));      //Actualizamos la semilla para que en cada ejecución se generen valores distintos
    int rank, nProcesos;    //Declaramos las variables para almacenar el identificador del procesador y el total de procesadores
    MPI_Init(&argc, &argv); //Establecemos el comienzo del flujo paralelo
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);       //Obtenemos el identificador del procesador
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesos);  //Obtenemos el total de procesadores a usar

    if (argc != 2) //Comprobamos que se pasamos la longitud de los vectores por parámetro
    {
        if (rank == RANK_0) //Establecemos que sea el core 0 el que imprima el mensaje de error
            printf("Se requiere el numero de elementos como parÃ¡metro\n");
    }
    else
    {
        if (rank == RANK_0) //Definimos el código que se ejecutará exclusivamente en el core 0
        {
            MPI_Status status;  //Declaramos variable para almacenar posibles errores de MPI
            int longitud = atoi(argv[1]);                   //Capturamos el argumento correspondiente a la longitud del vector
            int *vector1 = crearVectorAleatorio(longitud); //Creamos un vector aleatorio de números enteros
            int *vector2 = crearVectorAleatorio(longitud); //Creamos un vector aleatorio de números enteros
            int *desplazamientos = malloc(sizeof(int) * nProcesos); //Creamos un vector para almacenar los desplazamientos
            int *cuentas = malloc(sizeof(int) * nProcesos);         //Creamos un vector para almacenar los elementos que procesará cada core
            int totalScalar = 0;                                    //Declaramos una variable para almacenar el resultado

            /*Imprimimos los elementos del primer vector por pantalla*/
            printf("Elementos del vector 1: ");
            for (int j = 0; j < longitud; j++)
            {
                printf("%d ", vector1[j]);
            }

            printf("\n");

            /*Imprimimos los elementos del primer vector por pantalla*/
            printf("Elementos del vector 2: ");
            for (int j = 0; j < longitud; j++)
            {
                printf("%d ", vector2[j]);
            }

            printf("\n");

            /*Dividimos la carga de trabajo*/
            distribuirCargas(longitud, nProcesos, desplazamientos, cuentas);

            /*Enviamos la carga al resto de cores*/
            for (int i = RANK_1; i < nProcesos; i++)
            {
                int newVecLen = cuentas[i] * 2; //Calculamos el tamaño de un vector que será el doble de la cantidad de elementos
                int * vectorAux = malloc(sizeof(int) * newVecLen); //que tiene que procesar el core

                for (int j = 0; j < newVecLen; j++) //Iterativamente copiamos los ingresos y los gastos que tiene que procesar cada vector en el
                {                                   //el vector auxiliar
                    int vecTwoIndex = (j-cuentas[i]);
                    vectorAux[j] = ((j < cuentas[i]) ? vector1[j + desplazamientos[i]] : vector2[vecTwoIndex + desplazamientos[i]]);
                    //Cada vector auxiliar tiene la forma {[][][][][][][][][][][][]}
                    //                                     |  vector1 ||  vector2  |
                }

                MPI_Send(vectorAux, newVecLen, MPI_INT, i, 10, MPI_COMM_WORLD); //Enviamos el vector auxiliar al core correspondiente
                free(vectorAux); //Liberamos la memoria reservada para el vector auxiliar
            }

            /*Calculamos el prodcutor escalar que le corresponde al core 0*/
            for (int i = 0; i < cuentas[RANK_0]; i++)
            {
                totalScalar += vector1[i] * vector2[i];
            }

            for (int i = RANK_1; i < nProcesos; i++)
            {
                int scalarAux = 0;
                MPI_Recv(&scalarAux, 1, MPI_INT, i, 20, MPI_COMM_WORLD, &status);
                totalScalar += scalarAux;
            }

            printf("Producto escalar de los vectores: %d\n", totalScalar);
        }
        else
        {
            MPI_Status status;
            int data_len = 0;
            int vector_len = 0;
            int productScalar = 0;

            MPI_Probe(RANK_0, 10, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &data_len);

            int * data = malloc(sizeof(int) * data_len);
            vector_len = data_len / 2;

            MPI_Recv(data, data_len, MPI_INT, RANK_0, 10, MPI_COMM_WORLD, &status);

            for (int i = 0; i < vector_len; i++)
            {
                productScalar += (data[i] * data[i + vector_len]);
            }

            MPI_Send(&productScalar, 1, MPI_INT, RANK_0, 20, MPI_COMM_WORLD);
            free(data);
        }
    }
    MPI_Finalize();
    return 0;
}