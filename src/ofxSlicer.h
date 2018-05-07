//
//  ofxSlicer.h
//
//
//  Created by Frikk H Fossdal on 07.05.2018.
//
#include "ofMain.h"
#include "Layer.hpp"
#include "Triangles.hpp"
#include "ofxAssimpModelLoader.h"

class ofxSlicer{
public:
    //methods
    ofxSlicer();
    ofxSlicer(string path);
    
    //variables
    ofxAssimpModelLoader model;
    std::vector<Triangles> triangles;
    std::vector<Layer> layers;
    float layerHeight;
private:
    void buildTriangles();
    void sortTriangles();
};