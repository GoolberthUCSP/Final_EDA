# Proyecto final de semestre - Estructuras de datos avanzadas

- Se implementa una estructura Ball*-tree, el cual es una estructura de datos que permite realizar consultas de rango y vecinos más cercanos en tiempo logarítmico. Se utiliza para realizar consultas de rango en un conjunto de puntos en un espacio de dimensión arbitraria.

- El archivo csv cumple con el siguiente formato: primera fila (nombres de las columnas), resto de filas (datos); primeras *ndim* columnas (coordenadas), última columna (nombre del punto).

## Requerimientos

- Eigen3
- C++11

## Compilación

```terminal
cmake CMakeLists.txt
make
```

## Ejecución

- En linux: 
```terminal
./main
```
- En Windows:
```terminal
main.exe
```

## Eliminar archivos generados

```terminal
make clean-all
```