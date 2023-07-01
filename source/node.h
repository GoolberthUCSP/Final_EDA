#ifndef NODE_H
#define NODE_H

#include <vector>
#include<thread>
#include<string>
#include<mutex>
#include "record.h"
#include "sphere.h"
#include "lib.h"

template<int ndim>
class Node{
public:
    using Sphere_ = Sphere<ndim>;
    using Node_ = Node<ndim>;
    using Record_ = Record<ndim>;
    using VecR_ = vector<Record_*>;

    Node(int maxRecords, VecR_ &records);
    void build();
    Record_ &search(VectorXf &point);
    bool insert(Record_ &record);
    VecR_ rangeQuery(VectorXf &center, float radius);
    VecR_ knnQuery(VectorXf &center, int k);
    void calcSphere();
private:
    Sphere_ sphere;
    Node_ *left, *right;
    VecR_ records;
    bool isLeaf;
    int maxRecords;
};

//NODE METHODS
template<int ndim>
Node<ndim>::Node(int maxRecords, VecR_ &records){
    this->maxRecords = maxRecords;
    this->records = records;
    if (records.size() > maxRecords) this->isLeaf = false;
    else this->isLeaf = true;
}

template<int ndim>
void Node<ndim>::calcSphere(){
    //Calcular la hiperesfera que contiene a todos los records
    //Center= centroide, radius= maxima distancia al centroide
    VectorXf centroid= VectorXf::Zero(ndim);
    float radius;
    for (Record_ *record: records){
        centroid = centroid + record->getPoint();
    }
    centroid = centroid/records.size();
    for (Record_ *record: records){
        float dist = record->distance(centroid);
        if (dist > radius) radius = dist;
    }
    this->sphere = Sphere_(centroid, radius);
}

/*
    @brief Función que construye el árbol de forma recursiva
*/
template<int ndim>
void Node<ndim>::build(){
    //Construir el árbol de forma recursiva
    if (isLeaf) return;

    //El vector de records ya está ordenado por el factor de proyección
    int end     = records.size();
    int median  = end/2;
    
    VecR_ leftRecords(records.begin(), records.begin()+median);
    VecR_ rightRecords(records.begin()+median, records.begin()+end);

    //Paralelizar el ordenamiento de los records respecto al factor de proyección
    thread leftSortThread(&sortRecords<ndim>, ref(leftRecords));
    thread rightSortThread(&sortRecords<ndim>, ref(rightRecords));
    leftSortThread.join();
    rightSortThread.join();

    //Crear los nodos hijos
    left = new Node_(maxRecords, leftRecords);
    right = new Node_(maxRecords, rightRecords);

    //Paralelizar el cálculo de la esfera para cada nodo
    thread leftSphereThread(&Node_::calcSphere, left);
    thread rightSphereThread(&Node_::calcSphere, right);
    leftSphereThread.join();
    rightSphereThread.join();
    //DEBUGGING: imprimir overlap= distancia entre esferas- suma de radios
    //cout << "Overlap: " << left->sphere.distance(right->sphere) - left->sphere.radius - right->sphere.radius << endl;

    //Llamar recursivamente a build para cada nodo
    //cout << "Left: " << leftRecords.size() << " Right: " << rightRecords.size() << endl; //DEBUGGING
    
    //Paralelizar la llamada a build para cada nodo
    thread leftBuildThread(&Node_::build, left);
    thread rightBuildThread(&Node_::build, right);
    leftBuildThread.join();
    rightBuildThread.join();
}


/*
    @brief Busca los records dentro de un radio de un punto
    @param center_ : punto central de la esfera de búsqueda
    @param radius_ : radio de la esfera de búsqueda
    @return vector de records que se encuentran dentro de la esfera de búsqueda
*/
template<int ndim>
vector<Record<ndim>*> Node<ndim>::rangeQuery(VectorXf &center_, float radius_){
    VecR_ result;
    if (isLeaf){
        for (Record_ *record: records){
            if (record->distance(center_) <= radius_){
                result.push_back(record);
            }
        }
        return result;
    }
    //Si la esfera del nodo no intersecta con la esfera de búsqueda, retornar un vector vacío
    if (sphere.distance(center_) > radius_)
        return result;
    VecR_ leftResult = left->rangeQuery(center_, radius_);
    VecR_ rightResult = right->rangeQuery(center_, radius_);
    result.insert(result.end(), leftResult.begin(), leftResult.end());
    result.insert(result.end(), rightResult.begin(), rightResult.end());
    return result;
}
/*Knn

The K-NN (K-Nearest Neighbors) search always returns the K
 nearest neighbors to the target point. We will briefly 
 explain the K-NN search method in Ball-tree, as proposed by Liu et al [22].
The algorithm considers a list of points P that contains the points found 
so far as the nearest neighbors of the target point (t). Additionally, 
let Ds be the minimum distance from the target point to the previously 
discovered nodes, Ds = max x∈P in |x−t|, and DN be the distance between t and the current node.

DN = max{DN.Parent, |t − center(N)| − radius(N)}

In the K-NN search algorithm, a node is expanded if DN < Ds. If the current node is a leaf, 
then every data point x in N that satisfies ||x − t|| < Ds is added to the results list. 
When the size of the K-NN list exceeds the limit K, the farthest point is removed from the 
list, and Ds is updated for further execution.

Knn-constrained

Constrained K-NN Search in Ball*-Tree
Our key idea for implementing range-constrained K-NN search is to combine the K-NN and range 
search algorithms in ball-tree and benefit from pruning in both. The range constraint limits 
the number of candidate nodes, while K-NN pruning filters the search nodes based on the top K
points found so far. In other words, whenever a node is either too far from the query (in terms 
of range) or is not likely to be among the top K points found so far, it is skipped. Algorithm 2
presents the constrained K-NN search algorithm for ball*-tree.

def constrained_nn_search(Pin, node, r, K):
    if DN >= Ds and DN > r:
        return Pin  # No se cumple el criterio de rango, se devuelve la lista sin cambios
    
    if node es una hoja:
        Pout = Pin
        for x in points(node):
            if |x - t| < Ds:
                add x to Pout
        if |Pout| == K + 1:
            remove farthest neighbor from Pout
            update Ds #Ds = distance from t to farthest neighbor in Pout
    else:
        dR = distance from center of childR(node)
        dL = distance from center of childL(node)
        Ptemp = Pin
        if dR <= radius(childR(node)) + r:
            Ptemp = constrained_nn_search(Ptemp, childR(node), r, K)
        if dL <= radius(childL(node)) + r:
            Pout = constrained_nn_search(Ptemp, childL(node), r, K)
    
    return Pout
*/
template<int ndim>
vector<Record<ndim>*> Node<ndim>::knnQuery(VectorXf &center_, int k){
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


#endif // NODE_H