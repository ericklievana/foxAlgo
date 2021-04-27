all: foxMPI foxOMP secuencial lectura

foxMPI:
	mpicc enterosFoxMPI.c -o foxMPI -lm

foxOMP:
	mpicc enterosFoxOMP.c -o foxOMP -lm -fopenmp

secuencial:
	mpicc enterosSec.c -o secuencial -lm

lectura:
	gcc lectura.c -o lectura
