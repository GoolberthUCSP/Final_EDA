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

template<int ndim>
class BallTree{
private:
public:
    using Record_ = Record<ndim>;
    using Sphere_ = Sphere<ndim>;
    using Node_ = Node<ndim>;
    using VecR_ = vector<Record_*>;
    using VecS_ = vector<string>;

    BallTree(int maxRecords, string filename= FILENAME);
    void load(string filename);
    void indexing();
    bool insert(Record_ &record);
    VecS_ by_atribute(string atribute, float value);
    VecS_ rangeQuery(VectorXf &center, float radius);
    VecS_ knnQuery(VectorXf &center, int k);
    VecR_ knnConstrained(VecR_ finded, Node_ &actual_node, float range, VectorXf &center, int k);
    void normalize(VectorXf &point);

private:
    Node_ *root;
    VecR_ records;
    int maxRecords;
    VectorXf normalizer;
};

//BALLTREE METHODS
template<int ndim>
BallTree<ndim>::BallTree(int maxRecords, string filename){
    //Inicializar el arbol con maxRecords y cargar los records desde filename
    this->maxRecords = maxRecords;
    normalizer= VectorXf::Ones(ndim);
    normalizer[8] = MAX_TEMPO;
    normalizer[9] = MAX_DURATION;
    load(filename);
    indexing();
}

template<int ndim>
void BallTree<ndim>::normalize(VectorXf &point){
    //Normalizar el punto 
    point= point.cwiseQuotient(normalizer);
}

template<int ndim>
void BallTree<ndim>::load(string filename){
    //Cargar los records desde el archivo filename
    //Una fila se compone de: ndim floats separados por comas, luego 2 strings
    //El primer string es songName y el segundo title
    //La primera fila del archivo es el header, no se debe cargar
    ifstream file(filename);
    string line;
    getline(file, line); //Skip header
    int id= 0;
    while (getline(file, line)){
        stringstream ss(line);
        string token;
        VectorXf point(ndim);
        for (int i=0; i<ndim; i++){
            getline(ss, token, ',');
            point[i] = stof(token);
        }
        string name;
        getline(ss, name, ',');
        Record_ *record = new Record_(id, point, name);
        records.push_back(record);
        id++;
    }
    file.close();
}

template<int ndim>
void BallTree<ndim>::indexing(){
    //Obtener el eigenvector para todos los records para el root
    VectorXf eig = getMaxEigenVector<ndim>(records);
    //Ordenar los records por el factor de proyección para el root
    sortByProyFactor<ndim>(records, eig);
    //Construir el root
    root = new Node_(maxRecords, records);
    //Calcular la esfera para el root
    root->calcSphere();
    //Construir el resto del árbol recursivamente
    root->build();
}

template<int ndim>
vector<string> BallTree<ndim>::by_atribute(string atribute, float value){
    //Retorna los records que tienen el valor value en el atributo atribute
    vector<Record<ndim>*> result;
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

template<int ndim>
vector<string> BallTree<ndim>::rangeQuery(VectorXf &center, float radius){
    //Realiza una búsqueda por rango en el árbol
    //Llama a rangeQuery del root
    normalize(center);
    return root->rangeQuery(center, radius);
}

template<int ndim>
vector<string> BallTree<ndim>::knnQuery(VectorXf &center, int k){
    //Realiza una búsqueda por k-nn en el árbol
    //Llama a knnQuery del root
    normalize(center);
    return root->knnQuery(center, k);
}

template<int ndim>
vector<Record<ndim>*> BallTree<ndim>::knnConstrained(VecR_ finded, Node_ &actual_node, float range, VectorXf &center, int k){

    //return knnConstrained({}, root, 0, center, k);
}

#endif