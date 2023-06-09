#ifndef BALLTREE_H
#define BALLTREE_H

#include<vector>
#include<fstream>
#include<sstream>
#include<thread>
#include<string>
#include<unordered_map>
#include<chrono>
#include<climits>
#include<iostream>
#include "node.h"

const string FILENAME = "songs_final.csv";
const float MAX_FLOAT = std::numeric_limits<float>::max();

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
    void insert(Record_ &record);
    VecS_ by_atribute(string atribute, float value);
    VecS_ rangeQuery(int id, float radius);
    VecS_ knnQuery(int id, int k);
    VecS_ knnQuery(string name, int k);
    VecS_ linearKnnQuery(int id, int k);
    void normalize(VectorXf &point);
    VectorXf getPoint(string name);
    VectorXf getPoint(int id);

    long getIndexingTime(){return indexingTime;}
    long getKnnTime(){return knnTime;}
    long getRangeTime(){return rangeTime;}
    long getLinearKnnTime(){return linearKnnTime;}

private:
    Node_ *root;
    VecR_ records;
    int maxRecords;
    VectorXf normalizer;
    unordered_map<string, int> coordNames;
    long indexingTime;
    long knnTime;
    long rangeTime;
    long linearKnnTime;
};  

//BALLTREE METHODS
template<int ndim>
BallTree<ndim>::BallTree(int maxRecords, string filename){
    this->maxRecords = maxRecords;

    //Cargar los records desde filename
    load(filename);
    
    //Inicializar el indexado
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
        getline(ss, name);
        name = name.substr(0, name.size()-1);
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
    root->welzl();
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
    if (coordNames.find(atribute) == coordNames.end()){
        cout << "Atributo no encontrado" << endl;
        return result;
    }
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
vector<string> BallTree<ndim>::rangeQuery(int id, float radius){
    VectorXf center = getPoint(id);
    VecR_ result;
    if (center == VectorXf::Zero(ndim)){
        cout << "Canción no encontrada" << endl;
        return {};
    }
    auto start = chrono::steady_clock::now();
    result= root->rangeQuery(center, radius);
    auto end = chrono::steady_clock::now();
    rangeTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    VecS_ names;
    for (Record_ *record: result){
        names.push_back(record->name);
    }
    return names;
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
    @brief Inserta el record en la lista de records y reconstruye el árbol
    @param record: registro a insertar
*/
template<int ndim>
void BallTree<ndim>::insert(Record_ &record){
    //Insertar el record en el root
    records.push_back(&record);
    //Reconstruir el árbol con el nuevo record añadido
    indexing();
}

/*
    @brief Llama a knnQuery del root
    @param id: id de la canción
    @param k: cantidad de vecinos más cercanos a buscar
    @return vector<string> con los nombres de las k canciones más cercanas
*/
template<int ndim>
vector<string> BallTree<ndim>::knnQuery(int id, int k){
    //Si k es mayor a la cantidad de records, se busca la cantidad máxima de vecinos
    if (k >= records.size()){
        k = records.size()-1;
    }

    //Obtener el punto de la canción con id = record->id
    VectorXf center = getPoint(id);
    if (center == VectorXf::Zero(ndim)){
        cout << "Canción no encontrada" << endl;
        return {};
    }
    multiset<neighbor<ndim>> neighbors;
    float radius = MAXFLOAT;

    auto start = chrono::steady_clock::now();
    root->knnQuery(center, k+1, radius, neighbors);
    auto end = chrono::steady_clock::now();
    knnTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    vector<string> result;
    auto neighbor= neighbors.begin();
    neighbor++;
    for(; neighbor!=neighbors.end(); neighbor++){
        result.push_back(neighbor->record->name);
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
    //Si k es mayor a la cantidad de records, se busca la cantidad máxima de vecinos
    if (k >= records.size()){
        k = records.size()-1;
    }

    //Obtener el punto de la canción con id = record->id
    VectorXf center = getPoint(name);
    if (center == VectorXf::Zero(ndim)){
        cout << "Canción no encontrada" << endl;
        return {};
    }
    multiset<neighbor<ndim>> neighbors;
    float radius = MAXFLOAT;

    auto start = chrono::steady_clock::now();
    root->knnQuery(center, k+1, radius, neighbors);
    auto end = chrono::steady_clock::now();
    knnTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    vector<string> result;
    auto neighbor= neighbors.begin();
    neighbor++;
    for(; neighbor!=neighbors.end(); neighbor++){
        result.push_back(neighbor->record->name);
    }
    return result;
}

/*
    @brief Retorna los k vecinos más cercanos de id usando fuerza bruta
    @param id: id de la canción
    @param k: cantidad de vecinos más cercanos a buscar
    @return vector<string> con los nombres de las k canciones más cercanas
*/
template<int ndim>
vector<string> BallTree<ndim>::linearKnnQuery(int id, int k){
    //Si k es mayor a la cantidad de records, se busca la cantidad máxima de vecinos
    if (k >= records.size()){
        k = records.size()-1;
    }

    //Obtener el punto de la canción con id = record->id
    VectorXf center= getPoint(id);
    if (center == VectorXf::Zero(ndim)){
        cout << "Canción no encontrada" << endl;
        return {};
    }
    multiset<neighbor<ndim>> neighbors;

    auto start = chrono::steady_clock::now();
    for (auto record: records){
        float distance= (center - record->point).norm();
        neighbors.insert(neighbor<ndim>(record, distance));
        if (neighbors.size() > k+1)
            neighbors.erase(--neighbors.end());
    }
    auto end = chrono::steady_clock::now();
    linearKnnTime = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    vector<string> result;
    auto neighbor= neighbors.begin();
    neighbor++;
    for(; neighbor!= neighbors.end(); neighbor++){
        result.push_back(neighbor->record->name);
    }
    return result;
}

#endif