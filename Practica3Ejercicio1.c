/*
* Practica3Ejercicio1.c
* @author: Jose Carlos Fuentes Angulo
* @date: 26 Abril 2020
* @description: Programa en C para restar el total de un vector de ingresos y el total
* un vector de gastos de forma paralela.
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
int *crearVectorAleatorio(int tam)
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
            int *ingresos = crearVectorAleatorio(longitud); //Creamos un vector aleatorio para los ingresos
            int *gastos = crearVectorAleatorio(longitud);   //Creamos un vector aleatorio para los gastos
            int *desplazamientos = malloc(sizeof(int) * nProcesos); //Creamos un vector para almacenar los desplazamientos
            int *cuentas = malloc(sizeof(int) * nProcesos);         //Creamos un vector para almacenar los elementos que procesará cada core
            int totalResult = 0;    //Declaramos una variable para almacenar el resultado
			int localIngresos = 0;  //Declaramos una variable para almacenar el total de ingresos que calculará el core 0
			int localGastos = 0;    //Declaramos una variable para almacenar el total de gastos que calculará el core 0

            /*Imprimimos el vector de ingresos*/
            printf("Ingresos: ");
            for (int j = 0; j < longitud; j++)
            {
                printf("%d ", ingresos[j]);
            }

            printf("\n");

            /*Imprimimos el vector de gastos*/
            printf("Gastos: ");
            for (int j = 0; j < longitud; j++)
            {
                printf("%d ", gastos[j]);
            }

            printf("\n");

            /*Dividimos la carga de trabajo*/
            distribuirCargas(longitud, nProcesos, desplazamientos, cuentas);

            /*Enviamos la carga al resto de cores*/
            for (int i = RANK_1; i < nProcesos; i++)
            {
                int newVecLen = cuentas[i] * 2; //Calculamos el tamaño de un vector que será el doble de la cantidad de elementos 
                                                //que tiene que procesar el core
                int * vectorAux = malloc(sizeof(int) * newVecLen);  //Reservamos memoria para un vector de dicho tamaño

                for (int j = 0; j < newVecLen; j++) //Iterativamente copiamos los ingresos y los gastos que tiene que procesar cada vector en el
                {                                   //el vector auxiliar
                    int vecTwoIndex = (j-cuentas[i]);
                    vectorAux[j] = ((j < cuentas[i]) ? ingresos[j + desplazamientos[i]] : gastos[vecTwoIndex + desplazamientos[i]]);
                    //Cada vector auxiliar tiene la forma {[][][][][][][][][][][][]}
                    //                                     | ingresos ||  gastos  |
                }

                MPI_Send(vectorAux, newVecLen, MPI_INT, i, 10, MPI_COMM_WORLD); //Enviamos el vector auxiliar al core correspondiente
                free(vectorAux); //Liberamos la memoria reservada para el vector auxiliar
            }

            /*Calculamos la suma de ingresos y la suma de gastos que le corresponde al core 0*/
			for (int i = 0; i < cuentas[RANK_0]; i++)
            {
                localIngresos += ingresos[i];
				localGastos += gastos[i];
            }

            /*Calculamos la diferencia de ingresos menos gastos correspondiente al core 0*/
			totalResult = localIngresos - localGastos;

            /*Recogemos iteretivamente los resultados enviados por el resto de cores y actualizamos la variable*/
            for (int i = RANK_1; i < nProcesos; i++)
            {
                int resultVecAux = 0;
                MPI_Recv(&resultVecAux, 1, MPI_INT, i, 20, MPI_COMM_WORLD, &status);
                totalResult += resultVecAux;
            }

            /*Imprimimos el resultado por pantalla*/
            printf("Ingresos - Gastos = %d\n", totalResult);
        }
        else    //Definimos el código que ejecutará el resto de procesadores
        {
            MPI_Status status; //Declaramos variable para almacenar posibles errores de MPI
            int data_len = 0;  //Declaramos la variable para almacenar la cantidad de elementos que vamos a recibir
            int vector_len = 0; //Declaramos la variable que almacenará el tamaño de cada "sub-vector" (ingresos/gastos)
            int ingresosLocal = 0;  //Declaramos la variable para almacenar el total de ingresos
			int gastosLocal = 0;    //Declaramos la variable para almacenar el total de gastos
			int localResult = 0;    //Declaramos la variable para almacenar el diferencial de ingresos y gastos

            MPI_Probe(RANK_0, 10, MPI_COMM_WORLD, &status); //Consultamos si hay un mensaje disponible en el core 0
            MPI_Get_count(&status, MPI_INT, &data_len); //Obtenemos el número de elementos que nos va a enviar el core 0

            int * data = malloc(sizeof(int) * data_len); //Reservamos memoria para un vector con el tamaño de elementos
            vector_len = data_len / 2;  //Calculamos el tamaño de cada "sub-vector" (ingresos/gastos)

            MPI_Recv(data, data_len, MPI_INT, RANK_0, 10, MPI_COMM_WORLD, &status); //Despachamos el mensaje enviado por el core 0 para obtener los datos

            /*Calculamos la suma "local" de los ingresos y la suma "local" de los gastos*/
            for (int i = 0; i < vector_len; i++)
            {
                ingresosLocal += data[i];
				gastosLocal += data[i + vector_len];
            }
			
            /*Calculamos la diferencia "local" entre ingresos y gastos*/
			localResult = ingresosLocal - gastosLocal;

            /*Enviamos el resultado calculado al core 0*/
            MPI_Send(&localResult, 1, MPI_INT, RANK_0, 20, MPI_COMM_WORLD);
            
            /*Liberamos la memoria para el vector de recepción*/
            free(data);
        }
    }
    MPI_Finalize(); //Definimos el punto en el que acaba el flujo paralelo

    return 0;
}