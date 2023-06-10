#include "eigen/Dense"
#include <thread>
#include <vector>
#include "record.h"

// Función para crear la matriz desde un vector de punteros a Records utilizando la paralelización
template<class T, int ndim>
Eigen::MatrixXf createMatrix(const std::vector<Record<T,ndim>*>& records) {
    // Tamaño de la matriz
    int numRecords = records.size();
    int numDimensions = records[0]->getDimension();
    // Crear la matriz y reservar espacio
    Eigen::MatrixXf matrix;
    matrix.resize(numRecords, numDimensions);

    // Definir el número de hilos
    int numThreads = std::thread::hardware_concurrency();

    // Dividir el trabajo entre los hilos
    int chunkSize = numRecords / numThreads;
    int start = 0;
    int end = 0;

    // Función que asigna los puntos a la matriz en paralelo
    auto assignPointsToMatrix = [&records, &matrix, numDimensions](int start, int end) {
        for (int i = start; i < end; i++) {
        Point<T,ndim> point = records[i]->getPoint();
        for (int j = 0; j < numDimensions; j++) {
            matrix(i, j) = point[j];
            }
        }
    };

    // Crear los hilos y asignar el trabajo
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads-1; i++) {
        start = i * chunkSize;
        end = (i == numThreads - 1) ? numRecords : start + chunkSize;
        threads.emplace_back(assignPointsToMatrix, start, end);
    }
    threads.emplace_back(assignPointsToMatrix, end, numRecords);

    // Esperar a que todos los hilos terminen
    for (auto& thread : threads) {
        thread.join();
    }
    return matrix;
}

template<class T, int ndim>
Point<T,ndim> getMaxEigenVector(const std::vector<Record<T,ndim>*>& records){
    Eigen::MatrixXf matrix = createMatrix(records);
    Eigen::MatrixXf cov = matrix.transpose() * matrix;
    Eigen::EigenSolver<Eigen::MatrixXf> es(cov);
    //Obtener el eigenvector más grande, no confundir con el eigenvalue más grande
    Eigen::VectorXf maxEigenVector = es.eigenvectors().col(0).real();
    Point<T,ndim> eigenvector;
    for (int i = 0; i < ndim; i++){
        eigenvector[i]= maxEigenVector[i];
    }
    return eigenvector;
}