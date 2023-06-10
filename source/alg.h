#ifndef ALG_H
#define ALG_H

#include<cmath>
#include<vector>
#include "point.h"

//Recibe un vector de puntos y devuelve el radio y el centroide
//de la esfera que los contiene en un vector de tamaño ndim+1: vec={r, point}
template<class T, int ndim>
std::vector<Point<T,ndim>> welzl(std::vector<Point<T,ndim>> &points){

}

#include <iostream>
#include <Eigen/Dense>

int main() {
    // Supongamos que tienes tus datos en una matriz de tamaño 43000x10 llamada "datos"

    int n = 43000; // Número de datos
    int d = 10; // Número de dimensiones

    // Convertir los datos a una matriz Eigen
    Eigen::MatrixXd matrizDatos(n, d);
    // Llenar la matriz de datos con tus datos reales
    // ...

    // Calcular la media de cada columna
    Eigen::VectorXd media = matrizDatos.colwise().mean();

    // Centrar los datos restando la media
    matrizDatos.rowwise() -= media.transpose();

    // Calcular la matriz de covarianza
    Eigen::MatrixXd matrizCovarianza = (matrizDatos.transpose() * matrizDatos) / (n - 1);

    // Calcular los vectores propios y los valores propios de la matriz de covarianza
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(matrizCovarianza);
    Eigen::MatrixXd vectoresPropios = eigenSolver.eigenvectors();

    // El vector propio más representativo será el vector propio correspondiente al valor propio más grande
    Eigen::VectorXd vectorRepresentativo = vectoresPropios.col(d - 1);

    std::cout << "Vector representativo:\n" << vectorRepresentativo << std::endl;

    return 0;
}


#endif