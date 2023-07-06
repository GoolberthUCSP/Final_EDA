#ifndef NODE_H
#define NODE_H

#include <vector>
#include<thread>
#include<string>
#include<set>
#include "record.h"
#include "sphere.h"
#include "lib.h"

using namespace std;

template<int ndim>
struct neighbor{
    Record<ndim> *record;
    float distance;
    neighbor(Record<ndim> *record, float distance){
        this->record = record;
        this->distance = distance;
    }
    bool operator<(const neighbor &other) const{
        return distance < other.distance;
    }
};

template<int ndim>
class Node{
public:
    using Sphere_ = Sphere<ndim>;
    using Node_ = Node<ndim>;
    using Record_ = Record<ndim>;
    using VecR_ = vector<Record_*>;

    Node(size_t maxRecords, VecR_ &records);
    void build();
    Record_ &search(VectorXf &point);
    bool insert(Record_ &record);
    VecR_ rangeQuery(VectorXf &center, float radius);
    void knnQuery(VectorXf &center, int k, float &radius_, multiset<neighbor<ndim>> &neighbors_);
    void calcSphere();
    float getDistance(VectorXf &center, float &radius_);
    bool isInside(VectorXf &center);
private:
    Sphere_ sphere;
    Node_ *left, *right;
    VecR_ records;
    bool isLeaf;
    size_t maxRecords;
};

//NODE METHODS
template<int ndim>
Node<ndim>::Node(size_t maxRecords, VecR_ &records){
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
        centroid = centroid + record->point;
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
    thread leftSortThread(&sortByProyFactor<ndim>, ref(leftRecords));
    thread rightSortThread(&sortByProyFactor<ndim>, ref(rightRecords));
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

/*
    @brief Distancia entre la esfera del nodo a la esfera de búsqueda
    @param center_ : punto central de la esfera de búsqueda
    @param radius_ : radio de la esfera de búsqueda
    @return distancia entre la esfera del nodo y la esfera de búsqueda
*/
template<int ndim>
float Node<ndim>::getDistance(VectorXf &center, float &radius_){
    return (sphere.center-center).norm() - sphere.radius - radius_;
}

/*
    @brief Booleano que indica si el punto está dentro de la esfera del nodo
    @param center_ : punto central de la esfera de búsqueda
    @return true si el punto está dentro de la esfera del nodo, false en caso contrario
*/
template<int ndim>
bool Node<ndim>::isInside(VectorXf &center){
    return ((sphere.center-center).norm() - sphere.radius) <= 0.0;
}
/*Knn-constrained

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

/*
    @brief Busca los k vecinos más cercanos a un punto
    @param center_ : punto central de la esfera de búsqueda
    @param k : número de vecinos a buscar
    @param radius_ : radio de la esfera de búsqueda
    @param neighbors_ : multiset de vecinos encontrados
    @return Guarda los k vecinos más cercanos en neighbors_
*/
template<int ndim>
void Node<ndim>::knnQuery(VectorXf &center_, int k, float &radius_, multiset<neighbor<ndim>> &neighbors_){
    if (isLeaf){
        float distance, maxOfList;
        for (Record_ *record: records){

            distance = record->distance(center_);
            
            if (distance <= radius_){
                neighbors_.insert(neighbor<ndim>(record, distance));
                if (neighbors_.size() > k){
                    maxOfList = (--neighbors_.end())->distance;
                    neighbors_.erase(--neighbors_.end());
                    radius_ = maxOfList;
                }
            }

        }
        return;
    }
    //Si la distancia es negativa, el nodo está dentro de la esfera de búsqueda
    float dLeft= left->getDistance(center_, radius_);
    float dRight= right->getDistance(center_, radius_);
    
    if (dLeft <= 0.0)
        left->knnQuery(center_, k, radius_, neighbors_);

    if (dRight <= 0.0)
        right->knnQuery(center_, k, radius_, neighbors_);

}

//Welzl's algorithm
/*
    @brief Calcula la esfera mínima que contiene a todos los puntos de un nodo
*/
void welzl(const std::vector<VectorXf>& points, VectorXf& center, float& radius) {
    size_t n = points.size();
    
    if (n == 0) {
        center.setZero();
        radius = 0.0f;
        return;
    }
    
    center = points[0];
    radius = 0.0f;
    
    for (size_t i = 1; i < n; ++i) {
        const VectorXf& p = points[i];
        if ((p - center).norm() > radius) {
            float d = (p - center).norm();
            float newRadius = (d + radius) / 2.0f;
            float ratio = (newRadius - radius) / d;
            center += ratio * (p - center);
            radius = newRadius;
        }
    }
}

#endif // NODE_H