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
}
void ofxSlicer::loadFile(string _pathToFile){
    //Try/catch not working. Fix it!
    try {
        model.loadModel(_pathToFile);
    } catch (exception& e) {
        std::cout << ("load failed. Check file if file exist at given location.") << endl;
    }
}
void ofxSlicer::slice(){
    //reset all vectors with geometry
    triangles.clear();
    layers.clear();
    //rebuild triangle-vector
    buildTriangles();
    //rebuild layer-vector
    createLayers();
    //parse trough all layers and do slicing
    for(auto it = layers.begin(); it!=layers.end(); it++){
        it->calculate(triangles);
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
        triangles.push_back(newTriangle);
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
    std::sort(triangles.begin(), triangles.end(), compareVector());
    findPerim();
}
void ofxSlicer::createLayers(){
    int numberOfLayers = (layerMax-layerMin)/layerHeight;
    for(int i = 0; i < numberOfLayers; i++){
        layers.push_back(Layer(layerHeight*i));
    }
}
void ofxSlicer::findPerim(){
    Triangles lastTriangle = triangles.back();
    Triangles firstTriangle = triangles.front();
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
    triangles.clear();
    layers.clear();
}

// ---------------------THREADING-------------------------
void ofxSlicer::startSlice(){
    startThread();
}
void ofxSlicer::stopSlice(){
    stopThread();
}
void ofxSlicer::threadedFunction(){
    while(isThreadRunning())
    {
        sliceFinished = false;
        std:: cout << "i am a thread and i am running" << endl;
        buildTriangles();
        createLayers();
        for(auto it = layers.begin(); it!=layers.end(); it++){
            it->calculate(triangles);
        }
        stopSlice();
        sliceFinished = true;
        std::cout << "sliced!" << endl; 
    }
}
