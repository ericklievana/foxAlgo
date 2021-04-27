#!/bin/bash

paste promediosMPITiempo.txt promediosOMPTiempo.txt promediosSecTiempo.txt ordenes.txt > promediosTiempo.txt
paste promediosMPIMemoria.txt promediosOMPMemoria.txt promediosSecMemoria.txt ordenes.txt > promediosMemoria.txt

gnuplot graficaTiempo.ptl
gnuplot graficaMemoria.ptl
