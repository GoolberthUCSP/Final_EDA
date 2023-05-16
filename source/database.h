#ifndef DATABASE_H
#define DATABASE_H

#include<vector>
#include<fstream>
#include<sstream>
#include<cstring>

template<class IS>
class Database{
private:
    vector<Record<T, ndim>> records;
    IS indexingStructure;
public:
    
    static constexpr int ndim = IS::nd_;
    typedef typename IS::T_ T;
    typedef typename IS::Point_ Point;
    typedef typename IS::Record_ Record;

    Database(){}
    void load(std::string filename);
    void indexing();
    bool insert(Record &record);
    bool remove(Record &record);
    bool update(Record &record);
    bool search(Record &record);
    bool rangeSearch(Point &point, T radius);
    bool searchKNN(Point &point, int k);
};

template<class IS>
void Database<IS>::load(std::string filename){
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