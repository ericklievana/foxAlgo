all: foxMPI foxOMP secuencial lectura

foxMPI:
	mpicc cortosFoxMPI.c -o foxMPI -lm

foxOMP:
	mpicc cortosFoxOMP.c -o foxOMP -lm -fopenmp

secuencial:
	mpicc cortosSec.c -o secuencial -lm

lectura:
	gcc lectura.c -o lectura
