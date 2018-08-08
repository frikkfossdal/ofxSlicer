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
#include "vec2key.hpp"

class ofxSlicer : public ofThread{
public:
    //constructor
    ofxSlicer();
    
    //methods
    void slice();
    void loadFile(string _pathToFile);
    void showAssimpModel();
    void showSegments(int _layer);
    void showIntersections(int _layer);
    void showTriangles();
    void cleanSlicer(); 
    void startSlice();
    void stopSlice();
    void threadedFunction();
    
    //variables
    float layerHeight;
    float layerMin;
    float layerMax;
    bool sliceFinished;
    bool isActive;
    bool hasModel;
    bool abortFlag;
    int currentProcessingLayer;
    string currentTask; 
    ofxAssimpModelLoader model;
    std::vector<Triangles> allTriangles;
    std::vector<Triangles> activeTriangles;
    std::vector<Layer> layers;
private:
    void buildTriangles();
    void sortTriangles();
    void createLayers();
    void findPerim();
    void findIntersectionPoints(std::vector<Layer> &_layers);
    void findJobs(std::vector<Layer> _layers);
    void intersectionCalc(ofVec3f &p0, ofVec3f &p1, ofVec3f &p2, Layer &currentLayer);
    void createContours(Layer &currentLayer);
    void insertHash(map<vec2key,pair<ofVec3f, ofVec3f>> &_hash, ofVec3f v, ofVec3f u);
    std::vector<ofVec3f> startLoop (map<vec2key, pair<ofVec3f, ofVec3f>> &_hash);
    void addToLoop(std::vector<ofVec3f> &_contour, map<vec2key, pair<ofVec3f, ofVec3f>> &_hash);
};
//TODO:
//1. Add segments
//2. Create jobs 
