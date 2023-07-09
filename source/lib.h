#ifndef LIB_H
#define LIB_H

#define EIGEN_USE_THREADS
#include <eigen3/Eigen/Dense>
#include <thread>
#include <vector>
#include <algorithm>
#include "record.h"

using namespace Eigen;

/*
    * @brief Crea una matriz de Eigen a partir de un vector de records
    * @param records: vector de records
    * @return: matriz de Eigen
*/
template<int ndim>
MatrixXf createMatrix(const std::vector<Record<ndim>*>& records) {
    // Tamaño de la matriz
    int numRecords = records.size();
    int numDimensions = records[0]->getDimension();

    // Crear la matriz con espacio reservado
    MatrixXf matrix(numRecords, numDimensions);

    // Definir el número de hilos
    int numThreads = std::thread::hardware_concurrency();

    // Dividir el trabajo entre los hilos
    int divSize = numRecords / numThreads;
    int start, end;

    // Función que asigna los puntos a la matriz en paralelo
    auto assignPointsToMatrix = [&](int start, int end) {
        for (int i = start; i < end; i++) {
            matrix.row(i) = records[i]->point.transpose();
        }
    };

    // Crear los hilos y asignar el trabajo
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads-1; i++) {
        start = i * divSize;
        end = start + divSize;
        threads.emplace_back(assignPointsToMatrix, start, end);
    }
    threads.emplace_back(assignPointsToMatrix, end, numRecords);

    // Esperar a que todos los hilos terminen
    for (auto& thread : threads) {
        thread.join();
    }
    return matrix;
}

/*
    * @brief Calcula el eigenvector de mayor valor propio de una matriz usando PCA
    * @param records: vector de records
    * @return: eigenvector de mayor valor propio
*/
template<int ndim>
VectorXf getMaxEigenvectPCA(const std::vector<Record<ndim>*>& records){
    MatrixXf matrix = createMatrix<ndim>(records);
    //Normalizar la matriz
    VectorXf mean = matrix.colwise().mean();
    matrix.rowwise() -= mean.transpose();
    //Calcular la matriz de covarianza
    MatrixXf cov = (matrix.transpose() * matrix)/(matrix.rows()-1);
    EigenSolver<MatrixXf> solver(cov);
    MatrixXf eigenvectors = solver.eigenvectors().real();
    return eigenvectors.col(0);
}

/*
    * @brief Calcula el eigenvector de mayor valor propio de una matriz usando SVD
    * @param records: vector de records
    * @return: eigenvector de mayor valor propio
*/
template<int ndim>
VectorXf getMaxEigenvectSVD(const std::vector<Record<ndim>*>& records){
    MatrixXf matrix = createMatrix<ndim>(records);
    //Normalizar la matriz
    VectorXf mean = matrix.colwise().mean();
    matrix.rowwise() -= mean.transpose();
    JacobiSVD<MatrixXf> svd(matrix, ComputeThinU | ComputeThinV);
    MatrixXf eigenvectors = svd.matrixV();
    return eigenvectors.col(0);
}

/*
    * @brief Método de la potencia para calcular el eigenvector aproximado de mayor valor propio de una matriz. Algoritmo iterativo para encontrar el eigenvector dominante.
    * @param records: vector de records
    * @return: eigenvector aproximado de mayor valor propio
*/
template<int ndim>
VectorXf powerIteration(MatrixXf &matrix, int iterations){
    VectorXf eigenvector = VectorXf::Random(ndim);
    eigenvector.normalize();
    for (int i=0; i<iterations; i++){
        eigenvector = matrix * eigenvector;
        eigenvector.normalize();
    }
    return eigenvector;
}

/*
    * @brief Calcula el eigenvector aproximado de mayor valor propio de una matriz usando el método de la potencia
    * @param records: vector de records
    * @return: eigenvector aproximado de mayor valor propio
*/
template<int ndim>
VectorXf getMaxEigenvectApprox(const std::vector<Record<ndim>*>& records, int iterations){
    MatrixXf matrix = createMatrix<ndim>(records);
    //Normalizar la matriz
    VectorXf mean = matrix.colwise().mean();
    matrix.rowwise() -= mean.transpose();
    //Calcular la matriz de covarianza
    MatrixXf cov = (matrix.transpose() * matrix)/(matrix.rows()-1);
    return powerIteration<ndim>(cov, iterations);
}

/*
    * @brief Calcula el factor de proyección de un punto sobre un eigenvector
    * @param eigenvect: eigenvector
    * @param point: punto
    * @return: factor de proyección
*/
float proyFactor(VectorXf &eigenvect, VectorXf &point){
    return eigenvect.dot(point) / eigenvect.squaredNorm();
}

/*
    * @brief Ordena un vector de records por el factor de proyección sobre el eigenvector de mayor valor propio
    * @param records: vector de records
*/
template<int ndim>
void sortByProyFactor(vector<Record<ndim>*> &records){

    // Obtener el eigenvector de mayor valor propio
    VectorXf eigenvect = getMaxEigenvectApprox<ndim>(records, 500);

    //Calcular el factor de proyección de cada punto sobre el eigenvector
    for (auto record : records){
        record->proyFactor= proyFactor(eigenvect, record->point);
    }

    // Ordena records por el factor de proyección sobre eigenvect
    sort(records.begin(), records.end(), [&eigenvect](Record<ndim> *a, Record<ndim> *b){
        return a->proyFactor < b->proyFactor;
    });
}

#endif // LIB_H