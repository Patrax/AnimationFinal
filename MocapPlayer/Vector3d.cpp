#include "Vector3d.h"

/*
	This file implements the methods for the Vector3d class.
*/	

Vector3d::~Vector3d(){
}//nothing to do here...


//constructors
Vector3d::Vector3d(): ThreeTuple(){

}

Vector3d::Vector3d(const Point3d &p) : ThreeTuple(p.x, p.y, p.z){

}

Vector3d::Vector3d(double x, double y, double z) : ThreeTuple(x,y,z){
}

Vector3d::Vector3d(double x, double y) : ThreeTuple(x,y){
}


Vector3d::Vector3d(const Point3d &p1, const Point3d &p2) : ThreeTuple(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z){
}

Vector3d::Vector3d(const Vector3d &other) : ThreeTuple(other.x, other.y, other.z){
}//done with constructors


