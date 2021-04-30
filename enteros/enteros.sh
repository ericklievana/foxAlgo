#!/bin/bash

function mpi(){
  #for i in {1..25}
  #do
    #mpirun -np 4 ./foxMPI $1;
  #done

  mv datosTiempo.txt tiempoMPI$1.txt
  mv datosMemoria.txt memoriaMPI$1.txt

  ./lectura memoriaMPI$1.txt tiempoMPI$1.txt promediosMPI
}

function sec(){
  for i in {1..100}
  do
    mpirun ./secuencial $1;
  done

  mv datosTiempo.txt tiempoSec$1.txt
  mv datosMemoria.txt memoriaSec$1.txt

  ./lectura memoriaSec$1.txt tiempoSec$1.txt promediosSec
}

function omp(){
  #for i in {1..25}
  #do
    #mpirun -np 4 ./foxOMP $1 0;
  #done

  mv datosTiempo.txt tiempoOMP$1.txt
  mv datosMemoria.txt memoriaOMP$1.txt

  ./lectura memoriaOMP$1.txt tiempoOMP$1.txt promediosOMP
}

#FILE=datosTiempo.txt
#if [[ -f "$FILE" ]]; then
  #rm datosTiempo.txt
#fi
#
#FILE=datosMemoria.txt
#if [[ -f "$FILE" ]]; then
  #rm datosMemoria.txt
#fi

#mpi "64"
#mpi "128"
#mpi "256"
#mpi "512"
#mpi "1024"
#mpi "2048"
#mpi "4096"
#mpi "8192"

#sec "64"
#sec "128"
#sec "256"
#sec "512"
#sec "1024"
#sec "2048"
#sec "4096"
#sec "8192"

#omp "64"
#omp "128"
#omp "256"
#omp "512"
#omp "1024"
omp "2048"
#omp "4096"
#omp "8192"

#paste promediosMPITiempo.txt promediosOMPTiempo.txt promediosSecTiempo.txt ordenes.txt > promediosTiempo.txt
#paste promediosMPIMemoria.txt promediosOMPMemoria.txt promediosSecMemoria.txt ordenes.txt > promediosMemoria.txt

#gnuplot graficaTiempo.ptl
#gnuplot graficaMemoria.ptl
