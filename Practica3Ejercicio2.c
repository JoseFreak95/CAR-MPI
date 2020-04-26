/*
* Practica3Ejercicio2.c
* @author: Jose Carlos Fuentes Angulo
* @date: 26 Abril 2020
* @description: Programa en C para buscar el mínimo valor dentro de un vector de forma
* paralela
*
* Teniendo en cuenta que no existe una limitación en número de procesadores que se tienen que
* utilizar, estableceremos un reparto equitativo respetando la localidad espacial de los 
* elementos del vector.
* La estrategia que vamos a usar es dividir ambos vectores en partes equitativas y asignar a
* cada procesador una parte del vector de esta forma cada procesador podrá calcular su minimo.
* Como requisito el core 0 será el encargado de hacer el reparto de elementos y el core 2 se
* encargará recibir los mínimos calculados de cada core y mostrar el resultado por pantalla
*/

/*Incluimos las cabeceras necesarias*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "mpi.h"

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
double *crearVectorAleatorio(int tam)
{
    double *vec = malloc(sizeof(double) * tam);
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
int main(int argc, char **argv)
{
    srand(time(NULL));          //Actualizamos la semilla para que en cada ejecución se generen valores distintos
    int rank, nProcesos;        //Declaramos las variables para almacenar el identificador del procesador y el total de procesadores
    MPI_Init(&argc, &argv);     //Establecemos el comienzo del flujo paralelo
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
            int longitud = atoi(argv[1]);                    //Capturamos el argumento correspondiente a la longitud del vector
            double *vector = crearVectorAleatorio(longitud); //Creamos un vector aleatorio de valores reales
            int *desplazamientos = malloc(sizeof(int) * nProcesos); //Creamos un vector para almacenar los desplazamientos
            int *cuentas = malloc(sizeof(int) * nProcesos);         //Creamos un vector para almacenar los elementos que procesará cada core
            double minimum = __DBL_MAX__;                           //Declaramos una variable para almacenar el minimo del core 0

            /*Imprimimos el vector de numeros reales por pantalla*/
            printf("Elementos del vector: ");
            for (int j = 0; j < longitud; j++)
            {
                printf("%.2f\t", vector[j]);
            }

            printf("\n");

            /*Dividimos la carga de trabajo*/
            distribuirCargas(longitud, nProcesos, desplazamientos, cuentas);

            /*Enviamos la carga al resto de cores*/
            for (int i = RANK_1; i < nProcesos; i++)
            {
                MPI_Send((vector + desplazamientos[i]), cuentas[i], MPI_DOUBLE, i, 10, MPI_COMM_WORLD);
            }
            
            /*Calculamos el mínimo de la parte del vector que pertenece al core 0*/
            for (int i = 0; i < cuentas[RANK_0]; i++)
            {
                if (vector[i] < minimum)
                {
                    minimum = vector[i];
                }
            }

            /*Enviamos el mínimo del cero 0 al core 2*/
            MPI_Send(&minimum, 1, MPI_DOUBLE, RANK_2, 20, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Status status; //Declaramos variable para almacenar posibles errores de MPI
            int data_len = 0;  //Declaramos la variable para almacenar la cantidad de elementos que vamos a recibir
            double minimum = __DBL_MAX__;    //Declaramos la variable para almacenar el diferencial de ingresos y gastos
            MPI_Probe(RANK_0, 10, MPI_COMM_WORLD, &status); //Consultamos si hay un mensaje disponible en el core 0
            MPI_Get_count(&status, MPI_DOUBLE, &data_len);  //Obtenemos el número de elementos que nos va a enviar el core 0

            double *data = (double *)malloc(sizeof(double) * data_len); //Reservamos memoria para un vector con el tamaño de elementos

            MPI_Recv(data, data_len, MPI_DOUBLE, RANK_0, 10, MPI_COMM_WORLD, &status);  //Despachamos el mensaje enviado por el core 0 para obtener los datos

            for (int i = 0; i < data_len; i++)  //Calculamos el mínimo "local" en cada core
            {
                if (data[i] < minimum)
                {
                    minimum = data[i];
                }
            }

            if (rank != RANK_2) //Comprobamos el identificador de cada core. En caso de no ser el core 2, enviamos el
            {                   //resultado al core 2
                MPI_Send(&minimum, 1, MPI_DOUBLE, RANK_2, 20, MPI_COMM_WORLD);
            }
            else    //Siendo el core 2, recibimos los resultados enviados por el resto de cores mientras actualizamos el valor de
            {       //mínimo
                double otherRanksMin = 0;
                for (int i = 0; i < nProcesos; i++)
                {
                    if (i != RANK_2){
                        MPI_Recv(&otherRanksMin, 1, MPI_DOUBLE, i, 20, MPI_COMM_WORLD, &status);
                        minimum = (otherRanksMin < minimum) ? otherRanksMin : minimum;
                    }
                }
                /*Imprimimos el valor minimo por pantalla*/
                printf("El valor minimo dentro del vector es: %.2f\n", minimum);
            }
        }
    }
    MPI_Finalize(); //Definimos el punto en el que acaba el flujo paralelo
    return 0;
}
