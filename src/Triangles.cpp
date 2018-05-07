//
//  Triangles.cpp
//  slicer
//
//  Created by Frikk Fossdal on 20.02.2018.
//

#include "Triangles.hpp"

Triangles::Triangles(){
}
Triangles::Triangles(ofVec3f _v0, ofVec3f _v1, ofVec3f _v2){
    
    points.push_back(_v0);
    points.push_back(_v1);
    points.push_back(_v2);
    
    for(auto it = points.begin(); it !=points.end(); it++)
    {
        setZmin(it->z);
        setZmax(it->z);
    }
    sortPoints();
}
void Triangles::setZmin(float z){
    if(z<zMin){
        zMin = z;
    }
}
void Triangles::setZmax(float z){
    if(z>zMax){
        zMax = z;
    }
}
void Triangles::show(){
    ofNoFill();
    ofSetColor(150, 15);
    ofDrawTriangle(points[0].x, points[0].y, points[0].z, points[1].x, points[1].y, points[1].z,points[2].x, points[2].y, points[2].z);
}
//sorts the points in triangle after z height. important for triangle intersection sometime later
struct compareVector{
    bool operator()(ofVec3f &a, ofVec3f &b)
    {
        if(a.z < b.z){
            return true;
        }
        else{
            return false;
        }
    }
};
void Triangles::sortPoints(){
    ofSort(points, compareVector());
}


