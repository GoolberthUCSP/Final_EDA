#ifndef BALLTREE_H
#define BALLTREE_H

#include<vector>
#include<fstream>
#include<sstream>
#include<thread>
#include<string>
#include<mutex>
#include "record.h"
#include "eig_aux.h"
#include "lib_aux.h"
const string FILENAME = "cleaned.csv";
//Constantes de normalización para dimensiones 9 y 10
const float MAX_DURATION = 913052.0;
const float MAX_TEMPO = 220.29;

using namespace std;

template<class T, int ndim>
struct Sphere{
    Point<T, ndim> center;
    T radius;
    Sphere(Point<T, ndim> &center, T radius){
        this->center = Point<T, ndim>(center);
        this->radius = radius;
    }
    Sphere() : center(Point<T, ndim>()), radius(0) {}
    void operator=(Sphere<T, ndim> other){
        center = other.center;
        radius = other.radius;
    }
    friend ostream& operator<<(ostream &os, Sphere<T, ndim> &sphere){
        os << "Center: " << sphere.center << "\nRadius: " << sphere.radius << endl;
        return os;
    }
};

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


template<class T, int ndim>
class BallTree{
private:
public:
    using Record_ = Record<T, ndim>;
    using Point_ = Point<T, ndim>;
    using Sphere_ = Sphere<T, ndim>;
    using Node_ = Node<T, ndim>;
    using VecR_ = vector<Record_*>;
    static constexpr int ndim_ = ndim;
    using T_ = T;

    BallTree(int maxRecords, string filename= FILENAME);
    void load(string filename);
    void indexing();
    bool insert(Record_ &record);
    VecR_ by_atribute(string atribute, T value);
    VecR_ rangeQuery(Point_ &center, T radius);
    VecR_ knnQuery(Point_ &center, int k);
    void normalize(Point_ &point);

private:
    Node_ *root;
    VecR_ records;
    int maxRecords;
};

//SPHERE METHODS

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

//BALLTREE METHODS
template<class T, int ndim>
BallTree<T,ndim>::BallTree(int maxRecords, string filename){
    //Initialize BallTree with maxRecords
    this->maxRecords = maxRecords;
    load(filename);
    cout << records.size() << " records loaded\n";
    indexing();
}

template<class T, int ndim>
void BallTree<T,ndim>::normalize(Point_ &point){
    //Normalizar el punto en sus coordenadas 9 y 10 con las constantes
    // MAX_TEMPO y MAX_DURATION
    point[8] = point[8]/MAX_TEMPO;
    point[9] = point[9]/MAX_DURATION;
}

template<class T, int ndim>
void BallTree<T,ndim>::load(string filename){
    //Cargar los records desde el archivo filename
    //Una fila se compone de: ndim floats separados por comas, luego 2 strings
    //El primer string es songName y el segundo title
    //La primera fila del archivo es el header, no se debe cargar
    ifstream file(filename);
    string line;
    getline(file, line); //Skip header
    while (getline(file, line)){
        stringstream ss(line);
        string token;
        T *coords = new T[ndim];
        for (int i=0; i<ndim; i++){
            getline(ss, token, ',');
            coords[i] = stof(token);
        }
        string songName, title;
        getline(ss, songName, ',');
        getline(ss, title, ',');
        Record_ *record = new Record_(coords, songName, title);
        records.push_back(record);
    }
    file.close();
}

template<class T, int ndim>
void BallTree<T,ndim>::indexing(){
    //Obtener el eigenvector para todos los records para el root
    Point_ eig = getMaxEigenVector(records);
    //Ordenar los records por el factor de proyección para el root
    sortByProyFactor<T, ndim>(records, eig);
    //Construir el root
    root = new Node_(maxRecords, records);
    //Calcular la esfera para el root
    root->calcSphere();
    //Construir el resto del árbol recursivamente
    root->build();
}

template<class T, int ndim>
vector<Record<T,ndim>*> BallTree<T,ndim>::by_atribute(string atribute, T value){
    //Retorna los records que tienen el valor value en el atributo atribute
    vector<Record<T,ndim>*> result;
    for (Record_ *record: records){
        if (record->getAtribute(atribute) == value){
            result.push_back(record);
        }
    }
    return result;
}

template<class T, int ndim>
vector<Record<T,ndim>*> BallTree<T,ndim>::rangeQuery(Point_ &center, T radius){
    //Realiza una búsqueda por rango en el árbol
    //Llama a rangeQuery del root
    normalize(center);
    return root->rangeQuery(center, radius);
}

template<class T, int ndim>
vector<Record<T,ndim>*> BallTree<T,ndim>::knnQuery(Point_ &center, int k){
    //Realiza una búsqueda por k-nn en el árbol
    //Llama a knnQuery del root
    normalize(center);
    return root->knnQuery(center, k);
}

//Auxiliary functions


#endif