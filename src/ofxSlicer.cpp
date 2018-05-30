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
    
    for(int l = 0; l < layers.size(); l++){
        for(auto t = activeTriangles.begin(); t != activeTriangles.end();){
            //remove non-active triangles
            if(t->zMin > layers[l].layerHeight){
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
                        intersectionCalc(topSide[0], topSide[1], bottomSide[0], layers[l]);
                    }
                }
                else if(bottomSide.size() > 1){
                    //Two points on bottomside
                    if(topSide.size() > 0){
                        intersectionCalc(bottomSide[0], bottomSide[1], topSide[0], layers[l]);
                    }
                }
                else{
                    //This would be the case where there are points that are coincident with
                    //the layer plane. Ignore for now.
                }
                t++;
            }
        }
        //IntersectionPoints on layer found. Next calculate contours and make them polygons
        createContours(layers[l]);
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
    activeTriangles.clear(); 
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
    
    
    ofVec3f interPoint0 = ofVec3f(x0,y0,currentLayer.layerHeight);
    ofVec3f interPoint1 = ofVec3f(x1,y1, currentLayer.layerHeight);
    
    currentLayer.intersectionpoints.push_back(interPoint0);
    currentLayer.intersectionpoints.push_back(interPoint1);
    
    //create line segment
    ofPolyline line;
    line.begin();
    line.addVertex(interPoint0.x, interPoint0.y, currentLayer.layerHeight);
    line.addVertex(interPoint1.x, interPoint1.y, currentLayer.layerHeight);
    line.end();
    currentLayer.segments.push_back(line);
}
void ofxSlicer::createContours(Layer &currentLayer){
    //create the an initial hash table
    //    typedef std::multimap<char, int>::iterator MMAPIterator;
    typedef pair<ofVec3f, ofVec3f> vec_pair;
    map<vec2key, vec_pair> hash;
    
    for(auto s = currentLayer.segments.begin(); s != currentLayer.segments.end(); s++){
        //fill the hash table with segments and one blank space seg(u,v) -> hash(key = u, value {v, *} and  hash(key = v, value {u, *}
        ofPolyline q = *s;
        if(q[0].distance(q[1]) > 0.001){
            insertHash(hash, q[0], q[1]);
            insertHash(hash, q[1], q[0]);
        }
    }
    std::cout << "initial hash built... " << endl;
    std::cout << "constructing contours..." << endl;
    //loop trough hash and build contours
    while(!hash.empty()){
        ofPolyline line;
        ofPolyline line2;
        line.addVertex(line2[0]);
        //startLoop(map<vec2key, vec_pair> &_hash)
        //extendLoop(ofPolyline _contour, map<vec2key,vec_pair> &_hash)
        //1. start with random point (set this as intital point
        //2. search for neighboor point add it to line.
        //3. if point == initial point, contour is closed.
        //4. remove from hash till complete
        hash.cbegin();
    }
}
void ofxSlicer::insertHash(map<vec2key,pair<ofVec3f, ofVec3f>> &_hash, ofVec3f u, ofVec3f v){
    auto search = _hash.find(vec2key(u.x, u.y,u.z));
    if(search == _hash.end()){
        //key does not exist. Make it
        _hash.insert(make_pair(vec2key(u.x,u.y,u.z), make_pair(v, ofVec3f(0))));
    }
    else{
        //key exist. Modify it with new value
        //_hash.insert(make_pair(vec2key(v.x,v.y,v.z), make_pair(u, ofVec3f(0))));
        (*search).second.second = v;
    }
}
ofVec3f ofxSlicer::startLoop(map<vec2key, pair<ofVec3f, ofVec3f> > &_hash){
    ofVec3f P;
    //create first point
    auto it = _hash.begin();
    ofVec3f u = ofVec3f((*it).first.x,(*it).first.y, (*it).first.z);
    std::vector<ofVec3f> uw = {(*it).second.first, (*it).second.second};
    ofVec3f v = uw[0];
    return P;
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
        
        //Do slicing and put information into each layer
        cleanSlicer(); 
        buildTriangles();
        createLayers();
        findIntersectionPoints(layers);
        stopSlice();
        sliceFinished = true;
        std::cout << "sliced!" << endl;
        //Run slicer animation and update relevant GUI.
    }
}
