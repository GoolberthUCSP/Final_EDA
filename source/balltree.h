#ifndef BALLTREE_H
#define BALLTREE_H

#include<vector>
#include<fstream>
#include<sstream>
#include<thread>
#include<string>
#include<mutex>
#include "node.h"
const string FILENAME = "cleaned.csv";
//Constantes de normalización para dimensiones 9 y 10
const float MAX_DURATION = 913052.0;
const float MAX_TEMPO = 220.29;

using namespace std;

template<class T, int ndim>
class BallTree{
private:
public:
    using Record_ = Record<T, ndim>;
    using Point_ = Point<T, ndim>;
    using Sphere_ = Sphere<T, ndim>;
    using Node_ = Node<T, ndim>;
    using VecR_ = vector<Record_*>;
    using VecS_ = vector<string>;
    static constexpr int ndim_ = ndim;
    using T_ = T;

    BallTree(int maxRecords, string filename= FILENAME);
    void load(string filename);
    void indexing();
    bool insert(Record_ &record);
    VecS_ by_atribute(string atribute, T value);
    VecS_ rangeQuery(Point_ &center, T radius);
    VecS_ knnQuery(Point_ &center, int k);
    VecR_ knnConstrained(VecR_ finded, Node_ &actual_node, float range, Point_ &center, int k);
    void normalize(Point_ &point);

private:
    Node_ *root;
    VecR_ records;
    int maxRecords;
};

//BALLTREE METHODS
template<class T, int ndim>
BallTree<T,ndim>::BallTree(int maxRecords, string filename){
    //Initialize BallTree with maxRecords
    this->maxRecords = maxRecords;
    load(filename);
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
vector<string> BallTree<T,ndim>::by_atribute(string atribute, T value){
    //Retorna los records que tienen el valor value en el atributo atribute
    vector<Record<T,ndim>*> result;
    vector<string> songs;
    for (Record_ *record: records){
        if (record->getAtribute(atribute) == value){
            result.push_back(record);
        }
    }
    //Retornar el nombre de la canción y el título de cada record usando getters
    //Retornar un vector de strings con la salida ostream de cada record
    stringstream ss;
    for (Record_ *record: result){
        ss << *record;
        songs.push_back(ss.str());
        ss.str("");
    }
    return songs;
}

template<class T, int ndim>
vector<string> BallTree<T,ndim>::rangeQuery(Point_ &center, T radius){
    //Realiza una búsqueda por rango en el árbol
    //Llama a rangeQuery del root
    normalize(center);
    return root->rangeQuery(center, radius);
}

template<class T, int ndim>
vector<string> BallTree<T,ndim>::knnQuery(Point_ &center, int k){
    //Realiza una búsqueda por k-nn en el árbol
    //Llama a knnQuery del root
    normalize(center);
    return root->knnQuery(center, k);
}

template<class T, int ndim>
vector<Record<T,ndim>*> BallTree<T,ndim>::knnConstrained(VecR_ finded, Node_ &actual_node, float range, Point_ &center, int k){

    //return knnConstrained({}, root, 0, center, k);
}

#endif