all: foxMPI foxOMP secuencial lectura

foxMPI:
	mpicc flotantesFoxMPI.c -o foxMPI -lm

foxOMP:
	mpicc flotantesFoxOMP.c -o foxOMP -lm -fopenmp

secuencial:
	mpicc flotantesSec.c -o secuencial -lm

lectura:
	gcc lectura.c -o lectura
