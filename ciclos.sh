#!/bin/bash

mi_dir=$1
archivos=`ls $mi_dir`
archivos_espacio=(`ls $mi_dir`)
echo $archivos
cont=0

for archivo in ${archivos[@]}; do
    ruta=$1/$archivo
    if [[ -x $ruta ]]; then
        echo $archivo
        cont=$(($cont+1))
    fi
done
echo "son $cont archivos"