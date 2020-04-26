#include<stdio.h>

 
int metodo (int rank){
    printf("Soy el proceso %d\n", rank);
}

int main( int argc, char* argv[] ) {
    int rank;// este debe ser el rank del proceso
    
    metodo(rank);
 
}