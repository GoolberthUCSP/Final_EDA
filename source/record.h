#ifndef RECORD_H
#define RECORD_H

#include "point.h"

template<class T, int ndim>
class Record{
public:
    using Point_ = Point<T, ndim>;
    using Record_ = Record<T, ndim>;

    Record(T coords[ndim], string songName, string title) : songName(songName), title(title) {
        point.setCoords(coords);
    }

    Point_ &getPoint(){ return point; }
    string getSongName(){ return songName; }
    string getTitle(){ return title; }
    int getDimension(){ return ndim; }
    T distance(Record_ &other){
        return point.distance(other.getPoint());
    }
    T dotProduct(Record_ &other){
        return point.dotProduct(other.getPoint());
    }
    friend ostream& operator<<(ostream &os, Record_ &record){
        //Print title\tgenre\tsongName\tduration_ms\ttempo
        if (record.title != "Null") os << record.title;
        else os << record.songName;
        return os;
    }
private:
    Point_ point;
    string songName, title;
};

#endif