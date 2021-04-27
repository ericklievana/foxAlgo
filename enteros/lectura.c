//Este programa es para visualizar la division de los procesos y sacar un promedio
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
  char buffer[50];
  if(argc != 4){
    printf("Ejecucion: ./lectura datosMemoria datosTiempo tipoDato\n");
  }else{
    double memoria,total=0,repeticones=0,subtotal=0;
    FILE *file;
    file = fopen(argv[1], "rb");
    if(file == NULL){
      printf("ERROR\n");
    }else{
      while (!feof(file)) {
        //fread(&memoria,sizeof(double), 1, file);
        fscanf(file,"%lf\n",&memoria);
        printf ("Memoria: %f \n", memoria);
        subtotal = subtotal + memoria;
        repeticones++;
      }
      total = subtotal/repeticones;
      printf ("Memoria promedio: %f \n", total);
      strcpy(buffer,argv[3]);
      strcat(buffer,"Memoria.txt");
      FILE *file2;
      file2 = fopen(buffer,"a+");
      if(file == NULL){
        printf("Error\n");
      }else{
        fprintf(file2,"%lf\n",total);
      }
    }
    printf("\n");
    double tiempo;
    total = 0;
    subtotal = 0;
    repeticones = 0;
    file =fopen(argv[2], "rb");
    if(file == NULL){
      printf("ERROR\n");
    }else{
      while (!feof(file)) {
        //fread(&tiempo,sizeof(double), 1, file);
        fscanf(file,"%lf\n",&tiempo);
        printf ("Tiempo: %f \n", tiempo);
        subtotal = subtotal + tiempo;
        repeticones++;
      }
      total = subtotal/repeticones;
      printf ("Tiempo promedio: %f \n", total);
      strcpy(buffer,argv[3]);
      strcat(buffer,"Tiempo.txt");
      FILE *file2;
      file2 = fopen(buffer,"a+");
      if(file == NULL){
        printf("Error\n");
      }else{
        fprintf(file2,"%lf\n",total);
      }
    }
  }
  return 0;
}
