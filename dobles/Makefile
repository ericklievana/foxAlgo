all: foxMPI foxOMP secuencial lectura

foxMPI:
	mpicc doublesFoxMPI.c -o foxMPI -lm

foxOMP:
	mpicc doublesFoxOMP.c -o foxOMP -lm -fopenmp

secuencial:
	mpicc doublesSec.c -o secuencial -lm

lectura:
	gcc lectura.c -o lectura
