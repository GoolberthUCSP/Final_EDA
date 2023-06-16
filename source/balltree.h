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

//SPHERE METHODS



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