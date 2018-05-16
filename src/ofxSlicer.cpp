//
//  ofxSlicer.cpp
//
//
//  Created by Frikk H Fossdal on 07.05.2018.
//

#include "ofxSlicer.h"

ofxSlicer::ofxSlicer(){
    layerHeight = 0.01;
    sliceFinished = false;
    hasModel = false;
    isActive = false;
}
void ofxSlicer::loadFile(string _pathToFile){
    //Try/catch not working. Fix it!
    try {
        model.loadModel(_pathToFile);
        hasModel = true;
        
    } catch (exception& e) {
        std::cout << ("load failed. Check file if file exist at given location.") << endl;
        hasModel = false; 
    }
}

void ofxSlicer::buildTriangles(){
    int meshIndex = 0;
    ofMesh mesh = model.getMesh(meshIndex);
    
    ofMatrix4x4 modelMatrix = model.getModelMatrix();
    ofMatrix4x4 meshMatrix = model.getMeshHelper(meshIndex).matrix;
    ofMatrix4x4 concatMatrix;
    concatMatrix.preMult(modelMatrix);
    concatMatrix.preMult(meshMatrix);
    
    for(int i = 0; i < mesh.getNumVertices(); i++){
        ofVec3f & vert = mesh.getVertices()[i];
        vert.set(concatMatrix.preMult(vert));
        mesh.setVertex(i, vert);
    }
    std::vector<ofMeshFace> faces = mesh.getUniqueFaces();
    //loop through the faces and create triangle objects
    for(auto f = faces.begin(); f != faces.end(); f++){
        Triangles newTriangle(f->getVertex(0),f->getVertex(1), f->getVertex(2));
        allTriangles.push_back(newTriangle);
    }
    sortTriangles();
}
struct compareVector{
    bool operator()(Triangles &a, Triangles &b)
    {
        if(a.zMin < b.zMin){
            return true;
        }
        else{
            return false;
        }
    }
};
void ofxSlicer::sortTriangles(){
    std::sort(allTriangles.begin(), allTriangles.end(), compareVector());
    findPerim();
}
void ofxSlicer::createLayers(){
    int numberOfLayers = (layerMax-layerMin)/layerHeight;
    for(int i = 0; i < numberOfLayers; i++){
        layers.push_back(Layer(layerHeight*i));
    }
}
void ofxSlicer::findIntersectionPoints(std::vector<Layer> _layers){
    activeTriangles = allTriangles;
    for(auto l = _layers.begin(); l != _layers.end(); l++){
        //loop trough all triangles for each layer
        for(auto t = activeTriangles.begin(); t != activeTriangles.end();){
            //remove non-active triangles
            if(t->zMin > l->layerHeight){
                t = activeTriangles.erase(t);
            }
            //calculate intersectionPoints for layer
            else{
                std::vector<ofVec3f> topSide;
                std::vector<ofVec3f> bottomSide;
                std::vector<ofVec3f> onPlane;
                
                //loop trough all points in a triangle object
                for(auto p = t->points.begin(); p != t->points.end(); p++){
                    if(p->z > layerHeight){
                        //point is located over plane
                        topSide.push_back(ofVec3f(p->x, p->y, p->z));
                    }
                    else if(p->z < layerHeight){
                        //point is located under plane
                        bottomSide.push_back(ofVec3f(p->x, p->y, p->z));
                    }
                    else{
                        //point is located on plane
                        onPlane.push_back(ofVec3f(p->x, p->y, p->z));
                    }
                }
                //use the points to calculate intersectionPoints
                if(topSide.size() > 1){
                    //Two points on topside.
                    if(bottomSide.size() > 0){
                        intersectionCalc(topSide[0], topSide[1], bottomSide[0], *l);
                    }
                }
                else if(bottomSide.size() > 1){
                    //Two points on bottomside
                    if(topSide.size() > 0){
                        intersectionCalc(bottomSide[0], bottomSide[1], topSide[0], *l);
                    }
                }
                else{
                    //This would be the case where there are points that are coincident with
                    //the layer plane. Ignore for now.
                }
                t++;
            }
        }
    }
}
void ofxSlicer::findPerim(){
    Triangles lastTriangle = allTriangles.back();
    Triangles firstTriangle = allTriangles.front();
    layerMax = lastTriangle.zMax;
    layerMin = firstTriangle.zMin;
}
void ofxSlicer::showAssimpModel(){
    ofSetColor(255, 15);
    model.drawWireframe();
}
void ofxSlicer::showSegments(int _layer){
    layers[_layer].show(); 
    //draw all segments for each layer
}
void ofxSlicer::cleanSlicer(){
    allTriangles.clear();
    layers.clear();
}
void ofxSlicer::intersectionCalc(ofVec3f _target0, ofVec3f _target1, ofVec3f _orig, Layer &currentLayer){
    ofVec3f vec0 = _orig.operator-(_target0);
    ofVec3f vec1 = _orig.operator-(_target1);
    
    float t0 = (layerHeight-_target0.z)/vec0.z;
    float t1 = (layerHeight-_target1.z)/vec1.z;
    float x0 =_target0.x+vec0.x*t0;
    float x1 =_target1.x+vec1.x*t1;
    float y0 =_target0.y+vec0.y*t0;
    float y1 =_target1.y+vec1.y*t1;
    
    
    ofVec3f interPoint0 = ofVec3f(x0,y0,layerHeight);
    ofVec3f interPoint1 = ofVec3f(x1,y1, layerHeight);
    
    currentLayer.intersectionpoints.push_back(interPoint0);
    currentLayer.intersectionpoints.push_back(interPoint1);
}

// ---------------------THREADING-------------------------
void ofxSlicer::startSlice(){
    startThread();
    isActive = true;
}
void ofxSlicer::stopSlice(){
    stopThread();
    isActive = false; 
}
void ofxSlicer::threadedFunction(){
    while(isThreadRunning())
    {
        sliceFinished = false;
        std:: cout << "i am a thread and i am running" << endl;
        cleanSlicer(); 
        buildTriangles();
        createLayers();
        findIntersectionPoints(layers);
        //createJobs()
        //for each layer, create segments and jobs. Should this be handled directly by ofxSlicer (instead of layer)
        stopSlice();
        sliceFinished = true;
        std::cout << "sliced!" << endl; 
    }
}
