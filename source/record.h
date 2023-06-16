#ifndef RECORD_H
#define RECORD_H

#include<map>
#include "point.h"

//danceability,energy,loudness,speechiness,acousticness,instrumentalness,liveness,valence,tempo,duration_ms,genre
const map<string,int> num_atribs = {
    {"danceability", 0},
    {"energy", 1},
    {"loudness", 2},
    {"speechiness", 3},
    {"acousticness", 4},
    {"instrumentalness", 5},
    {"liveness", 6},
    {"valence", 7},
    {"tempo", 8},
    {"duration_ms", 9},
    {"genre", 10}
};

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
    T getAtribute(string atribute){
        return point[num_atribs.at(atribute)];
    }
    string getSongName(string atribute){
        if (atribute == "songName" || atribute == "title")
            //Return ostream loaded
            return atribute == "songName" ? songName : title;
        else return "Null";
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