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
struct Node{
    using Sphere_ = Sphere<ndim>;
    using Node_ = Node<ndim>;
    using Record_ = Record<ndim>;
    using VecR_ = vector<Record_*>;

    Node(size_t maxRecords, VecR_ &records);
    void build();
    VecR_ rangeQuery(VectorXf &center, float radius);
    void knnQuery(VectorXf &center, 
                    int k, 
                    float &radius_, 
                    multiset<neighbor<ndim>> &neighbors_);
    void calcSphere();
    void welzl();

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
    thread leftSphereThread(&Node_::welzl, left);
    thread rightSphereThread(&Node_::welzl, right);
    leftSphereThread.join();
    rightSphereThread.join();
    
    //Paralelizar la llamada a build para cada nodo
    thread leftBuildThread(&Node_::build, left);
    thread rightBuildThread(&Node_::build, right);
    leftBuildThread.join();
    rightBuildThread.join();

    records.clear();
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
    @brief Busca los k vecinos más cercanos a un punto
    @param center_ : punto central de la esfera de búsqueda
    @param k : número de vecinos a buscar
    @param radius_ : radio de la esfera de búsqueda
    @param neighbors_ : multiset de vecinos encontrados
    @return Guarda los k vecinos más cercanos en neighbors_
*/
template<int ndim>
void Node<ndim>::knnQuery(VectorXf &center_, int k, float &radius_, multiset<neighbor<ndim>> &neighbors_){
    if (sphere.distance(center_) > radius_)
        return;

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
    }
    else{
        //Si la distancia es menor que el radio, el nodo intersecta   
        if (left->sphere.distance(center_) <= radius_)
            left->knnQuery(center_, k, radius_, neighbors_);

        if (right->sphere.distance(center_) <= radius_)
            right->knnQuery(center_, k, radius_, neighbors_);
    }
}

/*
    @brief Calcula la esfera mínima que contiene a todos los puntos de un nodo en complejidad O(n)
*/
template<int ndim>
void Node<ndim>::welzl() {
    size_t n = records.size();
    VectorXf center= records[0]->point;
    float radius = 0.0f;
    
    auto record= records.begin();
    record++;

    for (; record != records.end(); record++) {
        Record_ *rec= *record;
        VectorXf& p= rec->point;
        if ((p - center).norm() > radius) {
            float d = (p - center).norm();
            float newRadius = (d + radius) / 2.0f;
            float ratio = (newRadius - radius) / d;
            center += ratio * (p - center);
            radius = newRadius;
        }
    }
    this->sphere= Sphere_(center, radius);
}

#endif // NODE_H