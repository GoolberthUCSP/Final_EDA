#define EIGEN_USE_THREADS
#include <eigen/Dense>
#include <thread>
#include <vector>
#include "record.h"

// Función para crear la matriz desde un vector de punteros a Records utilizando la paralelización
template<int ndim>
MatrixXf createMatrix(const std::vector<Record<ndim>*>& records) {
    // Tamaño de la matriz
    int numRecords = records.size();
    int numDimensions = records[0]->getDimension();
    // Crear la matriz y reservar espacio
    MatrixXf matrix(numRecords, numDimensions);

    // Definir el número de hilos
    int numThreads = std::thread::hardware_concurrency();

    // Dividir el trabajo entre los hilos
    int chunkSize = numRecords / numThreads;
    int start = 0;
    int end = 0;

    // Función que asigna los puntos a la matriz en paralelo
    auto assignPointsToMatrix = [&](int start, int end) {
        for (int i = start; i < end; i++) {
            matrix.row(i) = records[i]->getPoint().transpose();
        }
    };

    // Crear los hilos y asignar el trabajo
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads-1; i++) {
        start = i * chunkSize;
        end = start + chunkSize;
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
VectorXf getMaxEigenvectPCA(const std::vector<Record<ndim>*>& records){
    /*
    Crear la matriz de puntos
    Calcular la matriz de covarianza
    Calcular los eigenvectores
    Retornar el eigenvector que representa la dirección de mayor varianza
    */
    MatrixXf matrix = createMatrix(records);
    //Normalizar la matriz
    VectorXf mean = matrix.colwise().mean();
    matrix.rowwise() -= mean.transpose();
    //Calcular la matriz de covarianza
    MatrixXf cov = (matrix.transpose() * matrix)/(matrix.rows()-1);
    EigenSolver<MatrixXf> solver(cov);
    MatrixXf eigenvectors = solver.eigenvectors().real();
    return eigenvectors.col(0);
}

template<int ndim>
VectorXf getMaxEigenvectSVD(const std::vector<Record<ndim>*>& records){
    /*
    Crear la matriz de puntos
    Calcular los eigenvectores
    Retornar el eigenvector que representa la dirección de mayor varianza
    */
    MatrixXf matrix = createMatrix(records);
    //Normalizar la matriz
    VectorXf mean = matrix.colwise().mean();
    matrix.rowwise() -= mean.transpose();
    JacobiSVD<MatrixXf> svd(matrix, ComputeThinU | ComputeThinV);
    MatrixXf eigenvectors = svd.matrixV();
    return eigenvectors.col(0);
}

template<int ndim>
VectorXd powerIteration(MatrixXf &matrix, int iterations){
    VectorXf eigenvector = VectorXf::Random(ndim);
    eigenvector.normalize();
    for (int i=0; i<iterations; i++){
        eigenvector = matrix * eigenvector;
        eigenvector.normalize();
    }
    return eigenvector;
}

template<int ndim>
VectorXd getMaxEigenvectApprox(const std::vector<Record<ndim>*>& records, int iterations){
    MatrixXf matrix = createMatrix(records);
    //Normalizar la matriz
    VectorXf mean = matrix.colwise().mean();
    matrix.rowwise() -= mean.transpose();
    //Calcular la matriz de covarianza
    MatrixXf cov = (matrix.transpose() * matrix)/(matrix.rows()-1);
    return powerIteration<ndim>(cov, iterations);
}
