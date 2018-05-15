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
    //constructor
    ofxSlicer();
    
    //methods
    void slice();
    void loadFile(string _pathToFile);
    void showAssimpModel();
    void showSegments(int _layer);
    void cleanSlicer(); 
    void startSlice();
    void stopSlice();
    void threadedFunction();
    
    //variables
    float layerHeight;
    float layerMin;
    float layerMax;
    bool sliceFinished; 
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
