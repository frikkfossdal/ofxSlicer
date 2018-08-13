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
        std::cout << ("load failed. Check if file exist at given location.") << endl;
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
void ofxSlicer::findIntersectionPoints(std::vector<Layer> &_layers){
    activeTriangles = allTriangles;
    //loop trough all layers
    for(int l = 0; l < _layers.size(); l++){
        //add relevant triangles to active triangle vector
        for(auto t = activeTriangles.begin(); t != activeTriangles.end();){
            if(t->zMax < _layers[l].layerHeight){
                //entire triangle above layerHeight.
                activeTriangles.erase(t);
            }
            else if(t->zMin > _layers[l].layerHeight){
                //entire triangle below layerHeight.
            }
            else{
                //the sweet spot. Calculate intersection points
                intersectionCalc(t->points[0], t->points[1], t->points[2], layers[l]);
            }
            t++;
        }
        createContours(_layers[l]);
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
void ofxSlicer::showIntersections(int _layer){
    layers[_layer].showIntersections();
}

void ofxSlicer::showSegments(int _layer){
    layers[_layer].show(); 
    //draw all segments for each layer
}
void ofxSlicer::showTriangles(){
    for(auto it = allTriangles.begin(); it != allTriangles.end(); it++)
    {
        ofSetColor(255, 0, 0, 15);
        ofDrawTriangle(it->points[0].x, it->points[0].y, it->points[0].z, it->points[1].x, it->points[1].y, it->points[1].z, it->points[2].x, it->points[2].y, it->points[2].z);
    }
}
void ofxSlicer::cleanSlicer(){
    allTriangles.clear();
    activeTriangles.clear(); 
    layers.clear();
}
void ofxSlicer::intersectionCalc(ofVec3f &p0, ofVec3f&p1, ofVec3f &p2, Layer &currentLayer){
    std::vector<ofVec3f> intersectionPoints;
    //t for P0P1
    float t0 = (currentLayer.layerHeight - p0.z) / (p1.z-p0.z);
    //t for P0P2
    float t1 = (currentLayer.layerHeight - p0.z) / (p2.z-p0.z);
    //t for P1P2
    float t2 = (currentLayer.layerHeight - p1.z) / (p2.z-p1.z);
    
    //test t values and calculate coordinates for intersection
    if(t0 < 1 && t0 > 0){
        float x = p0.x + t0*(p1.x-p0.x);
        float y = p0.y + t0*(p1.y-p0.y);
        intersectionPoints.push_back(ofVec3f(x,y,currentLayer.layerHeight));
        currentLayer.intersectionpoints.push_back(ofVec3f(x,y,currentLayer.layerHeight));
    }
    if(t1 < 1 && t1 > 0){
        float x = p0.x + t1*(p2.x-p0.x);
        float y = p0.y + t1*(p2.y-p0.y);
        intersectionPoints.push_back(ofVec3f(x,y,currentLayer.layerHeight));
        currentLayer.intersectionpoints.push_back(ofVec3f(x,y,currentLayer.layerHeight));
    }
    if(t2 < 1 && t2 > 0){
        float x = p1.x + t2*(p2.x-p1.x);
        float y = p1.y + t2*(p2.y-p1.y);
        intersectionPoints.push_back(ofVec3f(x,y,currentLayer.layerHeight));
        currentLayer.intersectionpoints.push_back(ofVec3f(x,y,currentLayer.layerHeight));
    }
    //Create a polyline segment
    if(intersectionPoints.size() > 1){
        ofPolyline line;
        line.begin();
        line.addVertex(intersectionPoints[0].x, intersectionPoints[0].y,intersectionPoints[0].z);
        line.addVertex(intersectionPoints[1].x, intersectionPoints[1].y,intersectionPoints[1].z);
        line.end();
        currentLayer.segments.push_back(line);
    }
//    ofVec3f vec0 = _orig.operator-(_target0);
//    ofVec3f vec1 = _orig.operator-(_target1);
//
//    float t0 = (layerHeight-_target0.z)/vec0.z;
//    float t1 = (layerHeight-_target1.z)/vec1.z;
//    float x0 =_target0.x+vec0.x*t0;
//    float x1 =_target1.x+vec1.x*t1;
//    float y0 =_target0.y+vec0.y*t0;
//    float y1 =_target1.y+vec1.y*t1;
//
//    ofVec3f interPoint0 = ofVec3f(x0,y0,currentLayer.layerHeight);
//    ofVec3f interPoint1 = ofVec3f(x1,y1, currentLayer.layerHeight);
//
//    //add intersection points to vector
//    currentLayer.intersectionpoints.push_back(interPoint0);
//    currentLayer.intersectionpoints.push_back(interPoint1);
//
//    //create line segment
//    ofPolyline line;
//    line.begin();
//    line.addVertex(interPoint0.x, interPoint0.y, currentLayer.layerHeight);
//    line.addVertex(interPoint1.x, interPoint1.y, currentLayer.layerHeight);
//    line.end();
//    currentLayer.segments.push_back(line);
}

//Creates contour from intersection points
//review this function and its sub-function. Something is fishy...
void ofxSlicer::createContours(Layer &currentLayer){
    //create the an initial hash table
    //    typedef std::multimap<char, int>::iterator MMAPIterator;
    typedef pair<ofVec3f, ofVec3f> vec_pair;
    map<vec2key, vec_pair> hash;
    
    for(auto s = currentLayer.segments.begin(); s != currentLayer.segments.end(); s++){
        //fill the hash table with segments and one blank space seg(u,v) -> hash(key = u, value {v, *} and  hash(key = v, value {u, *}
        ofPolyline q = *s;
        if(q[0].distance(q[1]) > 0.1){
            insertHash(hash, q[0], q[1]);
            insertHash(hash, q[1], q[0]);
        }
    }
    //loop trough hash and build contours
    while(!hash.empty()){
        std::vector<ofVec3f> currentContour = startLoop(hash);
        //stay in addToLoop till contour is finishes or breaks
        addToLoop(currentContour, hash);
        //prototype create contour
        ofPolyline contour;
        contour.begin();
        for(int i = 0; i < currentContour.size(); i++){
            contour.addVertex(currentContour[i]);
        }
        contour.end();
        currentLayer.contours.push_back(contour);
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
std::vector<ofVec3f> ofxSlicer::startLoop(map<vec2key, pair<ofVec3f, ofVec3f> > &_hash){
    std::vector<ofVec3f> P;
    //create first point
    auto it = _hash.begin();
    ofVec3f u = ofVec3f((*it).first.x,(*it).first.y, (*it).first.z);
    std::vector<ofVec3f> uw = {(*it).second.first, (*it).second.second};
    ofVec3f v = uw[0];
    P.push_back(u);
    P.push_back(v);
    _hash.erase(vec2key(u.x,u.y,u.z));
    return P;
}

void ofxSlicer::addToLoop(std::vector<ofVec3f> &_contour, map<vec2key, pair<ofVec3f, ofVec3f> > &_hash){
    ofVec3f current = _contour.back();
    ofVec3f first = _contour.front();
    ofVec3f last;
    //find next vertices
    while(true){
        auto it = _hash.find(vec2key(current.x,current.y,current.z));
        if(it == _hash.end()){
            //dead end. Break loop
            break;
            std::cout << "fuckup!" << endl;
        }
        ofVec3f key1 = ofVec3f(it->first.x, it->first.y, it->first.z);
        //get next unused neighboor of current
        std::vector<ofVec3f> vw = {(*it).second.first, (*it).second.second};
        ofVec3f next = vw.at(0); //first unused neighbor of current
        _contour.push_back(next);
        
        //remove the used segment from hash.
        //continue to next segment
        _hash.erase(vec2key(current.x, current.y, current.z));
        if(next == first){
            //contour closed.
            break;
        }
        current = next;
    }
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
        currentTask = "slicer initiating";
        std:: cout << "i am a thread and i am running" << endl;
        //Do slicing and put information into each layer
        currentTask = "cleaning memory";
        cleanSlicer();
        currentTask = "building triangles";
        buildTriangles();
        currentTask = "creating layers";
        createLayers();
        currentTask = "locating intersection points";
        findIntersectionPoints(layers);
        stopSlice();
        sliceFinished = true;
        std::cout << "sliced!" << endl;
        currentTask = ""; 
        //Run slicer animation and update relevant GUI.
    }
}
