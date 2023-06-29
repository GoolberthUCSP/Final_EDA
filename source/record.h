#ifndef RECORD_H
#define RECORD_H

#define EIGEN_USE_THREADS
#include <eigen/Dense>
#include <string>

using namespace Eigen;

template <int ndim>
class Record{
private:
    VectorXf point;
    int id;
    string name;
public:
    using Record_ = Record<ndim>;
    Record() : point(ndim), id(-1), name("") {}
    Record(int id, VectorXf point, string name) : point(point), id(id), name(name) {}
    Record(const Record_ &other) : point(other.point), id(other.id), name(other.name) {}
    Record_& operator=(const Record_ &other){
        point = other.point;
        id = other.id;
        name = other.name;
        return *this;
    }
    VectorXf &getPoint() const { return point; }
    int getId() const { return id; }
    string &getName() const { return name; }
    int getDimension() const { return ndim; }
    float distance(const Record_ &other) const {
        return (point - other.point).norm();
    }
    float distance(VectorXf &other) const {
        return (point - other).norm();
    }
    friend ostream &operator<<(ostream &os, Record_ &record){
        os << record.name << "\t" << record.point.transpose();
        return os;
    }
};

#endif // RECORD_H