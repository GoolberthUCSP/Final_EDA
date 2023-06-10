<<<<<<< HEAD:source/eig_aux.h
#include "eigen/Dense"
=======
#include "eigen\Dense"
>>>>>>> 4c3ca7cfd93a8ce9eff9caf3271f2405c32c1f9f:source/eig_functions.h
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
        const double* point = records[i]->get_point();
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
Eigen::VectorXd get_max_eigen_value(const std::vector<Record<T,ndim>*>& records){
    Eigen::MatrixXf matrix = createMatrix(records);
    Eigen::MatrixXf cov = matrix.transpose() * matrix;
    Eigen::EigenSolver<Eigen::MatrixXf> es(cov);
    //Return the maximum eigenvalue in point class
    Point<T,ndim> max_eigen_value;
    max_eigen_value.set_point(es.eigenvalues().real().maxCoeff());
    return max_eigen_value;
}