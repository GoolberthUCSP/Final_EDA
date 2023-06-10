#ifndef BALLTREE_H
#define BALLTREE_H

#include<vector>
#include<fstream>
#include<sstream>
#include<thread>
#include<string>
#include "record.h"
#include "eig_aux.h"
#include "lib_aux.h"
const string FILENAME = "cleaned.csv";

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
        os << "Center: " << sphere.center << "-Radius: " << sphere.radius << endl;
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

    BallTree(int maxRecords);
    void load(string filename= FILENAME);
    void indexing();
    bool insert(Record_ &record);
    VecR_ by_atribute(string atribute, string value);
    VecR_ rangeQuery(Point_ &center, T radius);
    VecR_ knnQuery(Point_ &center, int k);

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
    left->build();
    right->build();
}

//BALLTREE METHODS
template<class T, int ndim>
BallTree<T,ndim>::BallTree(int maxRecords){
    //Initialize BallTree with maxRecords
    this->maxRecords = maxRecords;
    load();
    cout << records.size() << " records loaded\n";
    indexing();
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


//Auxiliary functions


#endif