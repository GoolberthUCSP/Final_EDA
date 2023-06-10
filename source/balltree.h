#ifndef BALLTREE_H
#define BALLTREE_H

#include<vector>
#include<fstream>
#include<sstream>
#include<cstring>
#include "record.h"
#include "eig_aux.h"

template<class T, int ndim>
struct Sphere{
    Point<T, ndim> center;
    T radius;
    Sphere(Point<T, ndim> center, T radius);
    Sphere(Point<T, ndim> center, Point<T, ndim> point);
};

template<class T, int ndim>
class Node{
public:
    using Point_ = Point<T, ndim>;
    using Sphere_ = Sphere<T, ndim>;
    using Node_ = Node<T, ndim>;
    using Record_ = Record<T, ndim>;
    using VecR_ = vector<Record_*>;

    Node(int max_records, Point_ &center, T radius);
    Node(int max_records, Point_ &center, Point_ &point);
    Node(int max_records, VecR_ &records);
    void build();
    Record_ &search(Point_ &point);
    bool insert(Record_ &record);
    VecR_ range_query(Point_ &center, T radius);
    VecR_ knn_query(Point_ &center, int k);
private:
    Sphere_ Sphere;
    Node_ *left, *right;
    VecR_ records;
    bool isLeaf;
    int max_records;
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

    BallTree(int max_records);
    void load(std::string filename);
    void indexing();
    bool insert(Record_ &record);
    VecR_ by_atribute(string atribute, string value);
    VecR_ range_query(Point_ &center, T radius);
    VecR_ knn_query(Point_ &center, int k);

private:
    Node_ *root;
    VecR_ records;
    int max_records;
};

//SPHERE METHODS
template<class T, int ndim>
Sphere<T,ndim>::Sphere(Point<T, ndim> center, Point<T, ndim> point){
    this->center = center;
    this->radius = center.distance(point);
}
template<class T, int ndim>
Sphere<T,ndim>::Sphere(Point<T, ndim> center, T radius){
    this->center = center;
    this->radius = radius;
}

//NODE METHODS


//BALLTREE METHODS
template<class T, int ndim>
BallTree<T,ndim>::BallTree(int max_records){
    //Initialize BallTree with max_records
    this->max_records = max_records;
    load("data.csv");
    indexing();
}

template<class T, int ndim>
void BallTree<T,ndim>::load(std::string filename="../dataset.csv"){
    //Load records from csv file
    ifstream file(filename);
    string line;
    while (getline(file, line)){
        stringstream ss(line);
        string token;
        T *coords= new T[ndim];
        for (int i = 0; i < ndim; i++){
            getline(ss, token, ',');
            coords[i] = stof(token);
        }
        string tempo, duration_ms, genre, song_name, title;
        getline(ss, tempo, ',');
        getline(ss, duration_ms, ',');
        getline(ss, genre, ',');
        getline(ss, song_name, ',');
        getline(ss, title, ',');
        Record_* record = new Record_(Point_(coords), tempo, duration_ms, genre, song_name, title);
        Record_* record = new Record_(Point_(coords), tempo, duration_ms, genre, song_name, title);
        records.push_back(*record);
    }
}

template<class T, int ndim>
void BallTree<T,ndim>::indexing(){
    root = new Node_(max_records, records);
    root->build();
}


#endif