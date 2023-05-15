#ifndef DATABASE_H
#define DATABASE_H

#include<vector>
#include<fstream>
#include<sstream>
#include "record.h"

template<class IS, class T, int ndim>
class Database{
private:
    vector<Record<T, ndim>> records;
    IS indexingStructure;
public:
    Database(){}
    void load(string filename);
    void indexing();
    bool insert(Record<T, ndim> &record);
    bool remove(Record<T, ndim> &record);
    bool update(Record<T, ndim> &record);
    bool search(Record<T, ndim> &record);
    bool rangeSearch(Point<T, ndim> &point, T radius);
    bool searchKNN(Point<T, ndim> &point, int k);
    bool searchByAttribute(string attribute);
};

template<class IS, class T, int ndim>
void Database<IS,T,ndim>::load(string filename){
    //danceability,energy,key,loudness,mode,speechiness,acousticness,instrumentalness,liveness,valence,tempo,duration_ms,genre,song_name,title
    //Primeros 10 campos son flotantes para el punto, el resto=5 son strings
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