#ifndef BALLTREE_H
#define BALLTREE_H

#include<vector>
#include<fstream>
#include<sstream>
#include<cstring>
#include "record.h"

template<class T, int ndim>
struct Circle{
    Point<T, ndim> center;
    T radius;
    Circle(Point<T, ndim> center, T radius);
    Circle(Point<T, ndim> center, Point<T, ndim> point);
};

template<class T, int ndim>
class Node{
public:
    using Point_ = Point<T, ndim>;
    using Circle_ = Circle<T, ndim>;
    using Node_ = Node<T, ndim>;
    using Record_ = Record<T, ndim>;
    using Vector_ = vector<Record_*>;

    Node(int max_records, Point_ &center, T radius);
    Node(int max_records, Point_ &center, Point_ &point);
    Node(int max_records, Vector_ &records);
    void build();

private:
    Circle_ circle;
    Node_ *left, *right;
    Vector_ records;
    bool isLeaf;
    int max_records;
};


template<class T, int ndim>
class BallTree{
private:
public:
    using Record_ = Record<T, ndim>;
    using Point_ = Point<T, ndim>;
    using Circle_ = Circle<T, ndim>;
    using Node_ = Node<T, ndim>;
    using Vector_ = vector<Record_*>;
    static constexpr int ndim_ = ndim;
    using T_ = T;

    BallTree(vector<Record<T, ndim>> &records);
    void build();
    void load(std::string filename);
    bool insert(Record_ &record);
    Vector_ by_atribute(string atribute, string value);
    Vector_ range_query(Point_ &center, T radius);
    Vector_ knn_query(Point_ &center, int k);

private:
    Node_ *root;
    Vector_ records;
};
//CIRCLE METHODS
template<class T, int ndim>
Circle<T,ndim>::Circle(Point<T, ndim> center, Point<T, ndim> point){
    this->center = center;
    this->radius = center.distance(point);
}
template<class T, int ndim>
Circle<T,ndim>::Circle(Point<T, ndim> center, T radius){
    this->center = center;
    this->radius = radius;
}

//NODE METHODS


//BALLTREE METHODS
template<class T, int ndim>
void BallTree<T,ndim>::load(std::string filename){
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
        Record<T,ndim>* record = new Record<T,ndim>(Point<T,ndim>(coords), tempo, duration_ms, genre, song_name, title);
        records.push_back(*record);
    }
}


#endif