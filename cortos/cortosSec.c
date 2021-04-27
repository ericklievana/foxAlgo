#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <sys/resource.h>
#define MAX 10000

void Read_matrix (char *prompt, short *matrix, int n);
void Print_matrix (char *prompt, short *matrix, int n);
void Set_to_zero (short *matrix, int n);
long obtener_memoria();

int main(int argc, char* argv[]) {
	srand(time(NULL));
	int i, j, k;
	int n;
	int c = 0;
	short *matrix_A, *matrix_B, *matrix_C;
	double start, end;
	double temp;
	int my_rank, p;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) {

		if (argc == 2) {
			sscanf(argv[1], "%d", &n);
		} else {
			printf("¿ Cuál es el orden de las matrices ? \n");
			scanf("%d", &n);
		}

		matrix_A = (short *) malloc (MAX*MAX*sizeof(short));
		matrix_B = (short *) malloc (MAX*MAX*sizeof(short));
		matrix_C = (short *) malloc (MAX*MAX*sizeof(short));

		Read_matrix ("Ingrese A : ", matrix_A, n);

		Read_matrix ("Ingrese B : ", matrix_B, n);

		Set_to_zero(matrix_C, n);

		start = MPI_Wtime();
	/*************************************************/
	/* Algoritmo secuencial para la multiplicación de matrices*/
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				for (k = 0; k < n; k++) {
					matrix_C[i + j*n] += matrix_A[i + k*n]*matrix_B[k*n + j];
				}
			}
		}
	/************************************************/
		double memoria = obtener_memoria();

		end = MPI_Wtime();
		MPI_Finalize();

		double tiempo;
    	tiempo = end - start;

    	FILE *file1;
	    file1 = fopen("datosTiempo.txt","a+");
	    //file1 = fopen("datosTiempo.dat","ab+");
	    if(file1 != NULL){
        fprintf(file1,"%lf\n",tiempo);
	      //fwrite(&tiempo,sizeof(double), 1, file1);
	    }
	    fflush(file1);
	    fclose(file1);

	    FILE *file2;
	    //file2 = fopen("datosMemoria.dat","ab+");
	    file2 = fopen("datosMemoria.txt","a+");
	    if(file2 != NULL){
        fprintf(file1,"%lf\n",memoria);
	      //fwrite(&memoria,sizeof(double), 1, file2);
	    }
	    fflush(file2);
	    fclose(file2);

		return 0;

	} else {	/*Si no es el primer procesador entonces finaliza.*/

		MPI_Finalize();

		return 0;

	}


}

long obtener_memoria() {
    struct rusage myusage;

    getrusage(RUSAGE_SELF, &myusage);

    return myusage.ru_maxrss;
}

void Read_matrix (char *prompt, short *matrix, int n) {
	int i, j;
	//printf("%s\n", prompt);
	for(i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				matrix[i*n + j] = rand()% 1001 + 1000;
			}
	}
} /* Read_matrix */

void Print_matrix (char *prompt, short *matrix, int n)  {
	int i, j;
	printf("%s\n", prompt);
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%d ", matrix[i*n + j]);
		}
		printf("\n");
	}
	printf("\n");
} /* Print_matrix */

void Set_to_zero (short * matrix, int n) {
	int i, j;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			matrix[j + i*n] = 0.0;
} /* Set_to_zero */
