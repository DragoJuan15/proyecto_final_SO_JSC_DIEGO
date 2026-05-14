#!/bin/bash
if [[ $# -eq 2 ]]; then
    echo $#
    nombre_archivo=$0
    argumento1=$1
    argumento2=$2
    resultado=$(($argumento1+$argumento2))
    echo "$nombre_archivo y ahora $argumento1 y finalmente $argumento2 adios, $resultado"
elif [[ $# -eq 1 ]]; then
    echo "tu numero es $1"
else
    echo "ponlo bien we $#"
fi

