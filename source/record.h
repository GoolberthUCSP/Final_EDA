#ifndef RECORD_H
#define RECORD_H

#include "point.h"

template<class T, int ndim>
class Record{
public:
    using Point_ = Point<T, ndim>;
    using Record_ = Record<T, ndim>;

    Record(Point_ &point, string song_name, string title) :    
            point(point), song_name(song_name), title(title) {}
    Point_ &getPoint(){ return point; }
    string getSong_name(){ return song_name; }
    string getTitle(){ return title; }
    int getDimension(){ return ndim; }
    T distance(Record_ &other){
        return point.distance(other.getPoint());
    }
    T dotProduct(Record_ &other){
        return point.dotProduct(other.getPoint());
    }
    friend ostream& operator<<(ostream &os, Record_ &record){
        //Print title\tgenre\tsong_name\tduration_ms\ttempo
        if (record.title != "Null") os << record.title;
        else os << record.song_name;
        return os;
    }
private:
    Point_ point;
    string song_name, title;
};

#endif