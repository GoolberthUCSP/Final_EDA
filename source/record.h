#ifndef RECORD_H
#define RECORD_H

#include "point.h"

template<class T, int ndim>
class Record{
public:
    using Point_ = Point<T, ndim>;
    using Record_ = Record<T, ndim>;

    Record(Point_ &point, string tempo, string duration_ms, 
            string genre, string song_name, string title) :    
            point(point), tempo(tempo), duration_ms(duration_ms), 
            genre(genre), song_name(song_name), title(title) {}
    Point_ &getPoint(){ return point; }
    string getTempo(){ return tempo; }
    string getDuration_ms(){ return duration_ms; }
    string getGenre(){ return genre; }
    string getSong_name(){ return song_name; }
    string getTitle(){ return title; }
    int getDimension(){ return ndim; }
    T distance(Record_ &other){
        return point.distance(other.getPoint());
    }
    T product(Record_ &other){
        return point.product(other.getPoint());
    }
    friend ostream& operator<<(ostream &os, Record_ &record){
        //Print title\tgenre\tsong_name\tduration_ms\ttempo
        os << record.title << "\t" << record.genre << "\t" << record.song_name 
            << "\t" << record.duration_ms << "\t" << record.tempo;
        return os;
    }
private:
    Point_ point;
    string tempo, duration_ms, genre, song_name, title;
};

#endif