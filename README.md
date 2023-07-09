# Proyecto final de semestre - Estructuras de datos avanzadas

- Se implementa una estructura **Ball*-tree**, el cual es una estructura de datos que permite realizar consultas de rango y vecinos más cercanos en tiempo logarítmico. Se utiliza para realizar consultas de rango en un conjunto de puntos en un espacio de dimensión arbitraria.

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

## Guía de uso

- Se crea un objeto de la clase BallTree, el cual recibe como parámetros el máximo número de puntos por nodo y el nombre del archivo csv. El archivo csv debe estar en la misma carpeta que el ejecutable.
```c++
BallTree<DIM> balltree(300, "songs_final.csv");
```

- Para la consulta de knn, se debe ingresar el id del punto y el número de vecinos más cercanos a buscar. Retorna un vector de strings con los nombres de los puntos más cercanos.
```c++
vector<string> knn = balltree.knnQuery(id, k);
```
- También se puede realizar la consulta con el nombre del punto.
```c++
vector<string> knn = balltree.knnQuery(name, k);
```
- Para verificar el correcto funcionamiento de la consulta, se puede usar el método linearKnnQuery, el cual realiza la consulta de knn de forma lineal. Retorna un vector de strings con los nombres de los puntos más cercanos.
```c++
vector<string> linearKnn = balltree.linearKnnQuery(id, k);
```
- El paper tiene la función rangeQuery, la cual retorna un vector de strings con los nombres de los puntos que se encuentran dentro de un rango. Se implementó una función similar, la cual retorna un vector de strings con los nombres de los puntos que se encuentran dentro de un rango. Recibe como parámetros el id del punto y el radio del rango.
```c++
vector<string> range = balltree.rangeQuery(id, radius);
```
- En cuanto a los tiempos de ejecución, se pueden obtener mediante getters. Se obtiene el tiempo en nanosegundos y de la última consulta realizada.
```c++
long getKnnTime(){return knnTime;}
long getRangeTime(){return rangeTime;}
long getLinearKnnTime(){return linearKnnTime;}
```
- La medida correcta del tiempo es de la siguiente forma:
```c++
vector<string> knn = balltree.knnQuery(id, k);
long knnTime = balltree.getKnnTime();
```
- Para obtener el tiempo de construcción del árbol, se puede usar el método getIndexingTime.
```c++
long getIndexingTime(){return indexingTime;}
```