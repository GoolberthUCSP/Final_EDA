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
    VecS_ knnQuery(VectorXf &center, int k);
    void normalize(VectorXf &point);
    void printDict();

    double getIndexingTime(){return indexingTime;}

private:
    Node_ *root;
    VecR_ records;
    int maxRecords;
    VectorXf normalizer;
    map<string, int> coordNames;
    int indexingTime;
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
    @brief Llama a knnQuery del root
    @param center: centro de la esfera
    @param k: cantidad de vecinos más cercanos a buscar
    @return vector<string> con los nombres de las k canciones más cercanas
*/
template<int ndim>
vector<string> BallTree<ndim>::knnQuery(VectorXf &center, int k){
    //normalize(center);
    multiset<neighbor> neighbors;
    float radius = k/10.0;
    root->knnQuery(center, k, radius, neighbors);
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