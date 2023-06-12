#ifndef NODE_H
#define NODE_H

#include <vector>
#include<thread>
#include<string>
#include<mutex>
#include "record.h"
#include "sphere.h"
#include "eig_aux.h"
#include "lib_aux.h"

//Cabeceras de funciones auxiliares
template<class T, int ndim>
Sphere<T, ndim> welzlAlgorithm(vector<Record<T, ndim>*> &records);

template<class T, int ndim>
class Node{
public:
    using Point_ = Point<T, ndim>;
    using Sphere_ = Sphere<T, ndim>;
    using Node_ = Node<T, ndim>;
    using Record_ = Record<T, ndim>;
    using VecR_ = vector<Record_*>;

    Node(int maxRecords, VecR_ &records);
    void build();
    Record_ &search(Point_ &point);
    bool insert(Record_ &record);
    VecR_ rangeQuery(Point_ &center, T radius);
    VecR_ knnQuery(Point_ &center, int k);
    void calcSphere();
private:
    Sphere_ sphere;
    Node_ *left, *right;
    VecR_ records;
    bool isLeaf;
    int maxRecords;
};

//NODE METHODS
template<class T, int ndim>
Node<T,ndim>::Node(int maxRecords, VecR_ &records){
    this->maxRecords = maxRecords;
    this->records = records;
    if (records.size() > maxRecords) this->isLeaf = false;
    else this->isLeaf = true;
}

template<class T, int ndim>
void Node<T,ndim>::calcSphere(){
    //Calculates the sphere that contains all the records
    //Center= centroid, radius= max distance to centroid
    Point_ centroid;
    T radius;
    for (Record_ *record: records){
        centroid = centroid + record->getPoint();
    }
    centroid = centroid/records.size();
    for (Record_ *record: records){
        T dist = record->getPoint().distance(centroid);
        if (dist > radius) radius = dist;
    }
    this->sphere = Sphere_(centroid, radius);
}

template<class T, int ndim>
void Node<T,ndim>::build(){
    //Builds the BallTree
    if (isLeaf) return;
    //El vector de records ya está ordenado por el factor de proyección
    int n = records.size();
    int median = n/2;
    int begin = 0;
    int end = n;
    VecR_ leftRecords(records.begin()+begin, records.begin()+median);
    VecR_ rightRecords(records.begin()+median, records.begin()+end);
    //Realizar el calculo del eigenvector para cada nodo en paralelo
    Point_ eigLeft, eigRight;
    thread leftThread([&](){eigLeft = getMaxEigenVector(leftRecords);});
    thread rightThread([&](){eigRight = getMaxEigenVector(rightRecords);});
    leftThread.join();
    rightThread.join();
    //Ordenar los records por el factor de proyección para cada nodo paralelamente
    //Con la función sortByProyFactor
    thread leftSortThread(sortByProyFactor<T,ndim>, ref(leftRecords), ref(eigLeft));
    thread rightSortThread(sortByProyFactor<T,ndim>, ref(rightRecords), ref(eigRight));
    leftSortThread.join();
    rightSortThread.join();
    left = new Node_(maxRecords, leftRecords);
    right = new Node_(maxRecords, rightRecords);
    //Paralelizar el cálculo de la esfera para cada nodo
    thread leftSphereThread(&Node_::calcSphere, left);
    thread rightSphereThread(&Node_::calcSphere, right);
    leftSphereThread.join();
    rightSphereThread.join();
    //Llamar recursivamente a build para cada nodo
    cout << "Left: " << leftRecords.size() << " Right: " << rightRecords.size() << endl;
    //Paralelizar la llamada a build para cada nodo
    thread leftBuildThread(&Node_::build, left);
    thread rightBuildThread(&Node_::build, right);
    leftBuildThread.join();
    rightBuildThread.join();
}

template<class T, int ndim>
vector<Record<T,ndim>*> Node<T,ndim>::rangeQuery(Point_ &center_, T radius_){
    //Realiza una búsqueda por rango en el nodo
    //Si el nodo es una hoja, retorna los records que están dentro del rango
    //Si el nodo no es una hoja, se debe verificar si la esfera del nodo intersecta
    //con la esfera de búsqueda, si no intersecta, retornar un vector vacío
    //Si intersecta, llamar recursivamente a rangeQuery para cada hijo
    //y retornar la unión de los resultados
    vector<Record<T,ndim>*> result;
    if (isLeaf){
        for (Record_ *record: records){
            if (record->getPoint().distance(center_) <= radius_){
                result.push_back(record);
            }
        }
        return result;
    }
    if (sphere.center.distance(center_) > (radius_ + sphere.radius)) return result;
    vector<Record<T,ndim>*> leftResult = left->rangeQuery(center, radius);
    vector<Record<T,ndim>*> rightResult = right->rangeQuery(center, radius);
    result.insert(result.end(), leftResult.begin(), leftResult.end());
    result.insert(result.end(), rightResult.begin(), rightResult.end());
    return result;
}

template<class T, int ndim>
vector<Record<T,ndim>*> Node<T,ndim>::knnQuery(Point_ &center_, int k){
    //Realiza una búsqueda por k-nn en el nodo
    //Si el nodo es una hoja, retorna los k records más cercanos a center_
    //Realiza una búsqueda k-nn restringida de acuerdo al siguiente artículo:
    //Ball*-tree: Efficient spatial indexing for constrained nearest-neighbor search in metric spaces
    //El criterio del artículo es que realiza la búsqueda k-nn usando búsqueda por rango
    VecR_ result;
    if (isLeaf){
        
        return result;
    }
}

//Auxiliary functions

template<class T, int ndim>
Sphere<T,ndim> welzlAlgorithm(vector<Record<T,ndim>*> &records, vector<Record<T,ndim>*> boundary={}){
    //Algoritmo de Welzl para calcular la esfera mínima que contiene a todos los records
    //El algoritmo debe calcular la hipersfera mínima que contiene a todos los records en O(n)
    //El algoritmo debe retornar la esfera mínima
    //El algoritmo debe ser recursivo
    //El algoritmo debe recibir como parámetros:
    //records: los records que deben estar dentro de la esfera
    //boundary: los records que están en el borde de la esfera
    //n: cantidad de records
    //Si n = 0 y boundary.size() = 1, retornar una esfera con centro en el único record de boundary
    //y radio 0
    //Si n = 0 y boundary.size() = 2, retornar una esfera con centro en el punto medio entre los
    //dos records de boundary y radio la distancia entre los dos records de boundary dividido 2
    
}

#endif // NODE_H