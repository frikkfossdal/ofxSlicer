//
//  Triangles.hpp
//  slicer
//
//  Created by Frikk Fossdal on 20.02.2018.
//

#ifndef Triangles_hpp
#define Triangles_hpp

#include <stdio.h>
#include "ofMain.h"

class Triangles{
public:
    
    std::vector<ofVec3f> points;
    ofVec3f normal;
    
    float zMin = +999999.9;
    float zMax = -999999.9;
    
    //Constructor
    Triangles();
    Triangles(ofVec3f _v0, ofVec3f _v1, ofVec3f _v2);
    
    //Methods
    void show();
    void sortPoints();
    
private:
    void setZmin(float z);
    void setZmax(float z);
};

#endif /* Triangles_hpp */
