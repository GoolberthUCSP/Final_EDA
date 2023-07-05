#ifndef BALLTREE_H
#define BALLTREE_H

#include<vector>
#include<fstream>
#include<sstream>
#include<thread>
#include<string>
#include<map>
#include<chrono>
#include "node.h"
const string FILENAME = "song_final.csv";

//Constantes de normalización para duration_ms y tempo
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
    VecS_ knnQuery(int id, int k);
    VecS_ knnQuery(string name, int k);
    void normalize(VectorXf &point);
    VectorXf getPoint(string name);
    VectorXf getPoint(int id);
    void printDict();

    long getIndexingTime(){return indexingTime;}
    long getKnnTime(){return knnTime;}
    long getRangeTime(){return rangeTime;}

private:
    Node_ *root;
    VecR_ records;
    int maxRecords;
    VectorXf normalizer;
    map<string, int> coordNames;
    long indexingTime;
    long knnTime;
    long rangeTime;
};

//BALLTREE METHODS
template<int ndim>
BallTree<ndim>::BallTree(int maxRecords, string filename){
    //Inicializar el arbol con maxRecords y cargar los records desde filename
    this->maxRecords = maxRecords;
    normalizer= VectorXf::Ones(ndim);
    // normalizer[8] = MAX_TEMPO;
    // normalizer[9] = MAX_DURATION;
    load(filename);
    auto start = chrono::steady_clock::now();
    indexing();
    auto end = chrono::steady_clock::now();
    indexingTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
}

/*
    @brief Normaliza el punto point
    @param point: VectorXf de tamaño ndim
*/
template<int ndim>
void BallTree<ndim>::normalize(VectorXf &point){
    //Normalizar el punto 
    point= point.cwiseQuotient(normalizer);
}

/*
    @brief Carga los records desde filename
    @param filename: string con el nombre del archivo csv
*/
template<int ndim>
void BallTree<ndim>::load(string filename){
    ifstream file(filename);
    string line, header;

    //Leer el header para obtener los nombres de las coordenadas
    getline(file, line);
    stringstream ss(line);
    for (int i=0; i<ndim; i++){
        getline(ss, header, ',');
        coordNames[header] = i;
    }

    int id= 1;
    while (getline(file, line)){
        stringstream ss(line);
        string coord, name;
        VectorXf point(ndim);
        for (int i=0; i<ndim; i++){
            getline(ss, coord, ',');
            point[i] = stof(coord);
        }
        getline(ss, name, ',');
        Record_ *record = new Record_(id, point, name);
        records.push_back(record);
        id++;
    }
    file.close();
}

/*
    @brief Ordena los records por el factor de proyección para el root y construye el root
*/
template<int ndim>
void BallTree<ndim>::indexing(){
    //Ordenar los records por el factor de proyección para el root
    sortByProyFactor<ndim>(records);
    //Construir el root
    root = new Node_(maxRecords, records);
    //Calcular la esfera para el root
    root->calcSphere();
    //Construir el resto del árbol recursivamente
    root->build();
}


/*
    @brief Devuelve los nombres de las canciones que tienen el valor value en el atributo atribute
    @param atribute: nombre del atributo
    @param value: valor del atributo
    @return vector<string> con los nombres de las canciones que cumplen la condición
*/
template<int ndim>
vector<string> BallTree<ndim>::by_atribute(string atribute, float value){
    vector<string> result;
    for (Record_ *record: records){
        if (record->point[coordNames[atribute]] == value){
            result.push_back(record->name);
        }
    }
    return result;
}

/*
    @brief Llama a rangeQuery del root
    @param center: centro de la esfera
    @param radius: radio de la esfera
    @return vector<string> con los nombres de las canciones que están dentro de la esfera
*/
template<int ndim>
vector<string> BallTree<ndim>::rangeQuery(VectorXf &center, float radius){
    //normalize(center);
    return root->rangeQuery(center, radius);
}

/*
    @brief Obtiene el punto de la canción con name
    @param name: nombre de la canción
    @return VectorXf con el punto de la canción
*/
template<int ndim>
VectorXf BallTree<ndim>::getPoint(string name){
    for (Record_ *record: records){
        if (record->name == name){
            return record->point;
        }
    }
    return VectorXf::Zero(ndim);
}

/*
    @brief Obtiene el punto de la canción con id
    @param id: id de la canción
    @return VectorXf con el punto de la canción
*/
template<int ndim>
VectorXf BallTree<ndim>::getPoint(int id){
    for (Record_ *record: records){
        if (record->id == id){
            return record->point;
        }
    }
    return VectorXf::Zero(ndim);
}

/*
    @brief Llama a knnQuery del root
    @param id: id de la canción
    @param k: cantidad de vecinos más cercanos a buscar
    @return vector<string> con los nombres de las k canciones más cercanas
*/
template<int ndim>
vector<string> BallTree<ndim>::knnQuery(int id, int k){
    //Obtener el punto de la canción con id
    VectorXf center = getPoint(id);
    multiset<neighbor<ndim>> neighbors;
    float radius = 1.0;
    initialize(neighbors, radius);

    auto start = chrono::steady_clock::now();
    root->knnQuery(center, k, radius, neighbors);
    auto end = chrono::steady_clock::now();
    knnTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    vector<string> result;
    for (auto it= neighbors.begin(); it!= neighbors.end(); it++){
        result.push_back(it->name);
    }
    return result;
}
/*
    @brief Llama a knnQuery del root
    @param name: nombre de la canción
    @param k: cantidad de vecinos más cercanos a buscar
    @return vector<string> con los nombres de las k canciones más cercanas
*/
template<int ndim>
vector<string> BallTree<ndim>::knnQuery(string name, int k){
    //Obtener el punto de la canción con id
    VectorXf center = getPoint(name);
    multiset<neighbor<ndim>> neighbors;
    float radius = k/10.0;

    auto start = chrono::steady_clock::now();
    root->knnQuery(center, k, radius, neighbors);
    auto end = chrono::steady_clock::now();
    knnTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    
    vector<string> result;
    for (auto it= neighbors.begin(); it!= neighbors.end(); it++){
        result.push_back(it->name);
    }
    return result;
}


template<int ndim>
void BallTree<ndim>::printDict(){
    for (auto it= coordNames.begin(); it!= coordNames.end(); it++){
        cout << it->first << ": " << it->second << endl;
    }
}

#endif