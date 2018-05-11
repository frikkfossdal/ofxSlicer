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

class ofxSlicer : public ofThread{
public:
    //methods
    ofxSlicer();

    void slice();
    void loadFile(string _pathToFile);
    void showCalculatedTriangles();
    void showAssimpModel();
    void showSegments();
    
    //variables
    float layerHeight;
    float layerMin;
    float layerMax;
    ofxAssimpModelLoader model;
    std::vector<Triangles> triangles;
    std::vector<Layer> layers;
private:
    void buildTriangles();
    void sortTriangles();
    void createLayers();
    void findPerim();
};
//TODO:
//1. Add segments
//2. Create jobs 
