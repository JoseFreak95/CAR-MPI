#include <stdio.h>
#include <stdlib.h>

const int SIZE = 3;

void inicializarMatriz(int** matriz){
	int contador = 0;
	for(int i = 0; i<SIZE; i++){
		for(int j = 0; j<SIZE; j++){
			matriz[i][j] = contador;
			contador++;
		}
	}
}

void limpiarMatriz(int** matriz){
	for(int i = 0; i<SIZE; i++){
		for(int j = 0; j<SIZE; j++){
			matriz[i][j] = 0;
		}
	}
}

void mostrarMatriz(int** matriz){
	for(int i = 0; i<SIZE; i++){
		for(int j = 0; j<SIZE; j++){
			printf("%d ", matriz[i][j]);
		}
		printf("\n");
	}
}

void volcarADisco(int** matriz){
	FILE* output = fopen("salida.txt", "w");//Escritura
	if(output){
		for(int i = 0; i<SIZE; i++){
			for(int j = 0; j<SIZE; j++){
				fprintf(output, "%d ", matriz[i][j]);
			}
			fprintf(output, "\n");
		}
		fclose(output);
	}else{
		printf("No se ha podido abrir el archivo de salida\n");
	}	
}

void leerDeDisco(int** matriz){
	FILE* input = fopen("salida.txt", "r");//Lectura
	if(input){
		int count = 0;
		for(int i = 0; i<SIZE; i++){
			count = fscanf(input, "%d %d %d\n", &matriz[i][0], &matriz[i][1], &matriz[i][2]);
			if(count!=SIZE){
				printf("Error, lÃ­nea incompleta\n");
				fclose(input);
				return;
			}
		}
		fclose(input);
	}else{
		printf("No se ha podido abrir el archivo de lectura\n");
	}
}

int main(int argc, char* argv[]){
	int** matriz = malloc(sizeof(int*)*SIZE);//Vamos a reservar una matriz aunque garantizando continuidad	
	int* matriz_container = malloc(sizeof(int)*SIZE*SIZE);//en memoria
	for(int i = 0; i<SIZE; i++){
		matriz[i] = &(matriz_container[i*SIZE]);//Ya no hacemos un malloc por fila, guardamos el inicio de cada una
	}
	inicializarMatriz(matriz);
	mostrarMatriz(matriz);
	volcarADisco(matriz);
	limpiarMatriz(matriz);
	mostrarMatriz(matriz);
	leerDeDisco(matriz);
	mostrarMatriz(matriz);
	free(matriz[0]);//Liberamos el contenedor subyacente
	free(matriz);//Liberamos el conjunto de punteros a fila
	return 0;
}