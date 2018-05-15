     //
//  Layer.cpp
//  slicer
//
//  Created by Frikk Fossdal on 20.02.2018.
//

#include "Layer.hpp"
Layer::Layer(){
};

Layer::Layer(float _layerHeight){
    layerHeight = _layerHeight;
}
void Layer::show(){
    for(auto it = segments.begin(); it !=segments.end(); it++){
        ofSetColor(200);
        it->draw();
    }
    for(auto it = intersectionpoints.begin(); it!=intersectionpoints.end(); it++)
    {
        ofSetColor(255, 0, 0);
        ofDrawEllipse(it->x, it->y, it->z, .1, .1);
    }
}
//Not in use yet. Implement later to increase calculation speed
std::vector<Triangles> Layer::findActive(std::vector<Triangles> _activeTriangles, std::vector<Triangles> _triangleList){
    for(auto t = _triangleList.begin(); t!=_triangleList.end();){
        if(t->zMin < layerHeight){
            _activeTriangles.push_back(*t);
        }
        t++;
    }
    return _activeTriangles;
}
void Layer::calculate(std::vector<Triangles> triangles){
    for(auto t = triangles.begin(); t != triangles.end();){
        if(t->zMax > layerHeight && t->zMin < layerHeight)
        {
            //create temp buff of points that is used in intersection calculation
            std::vector<ofVec3f> topSide;
            std::vector<ofVec3f> bottomSide;
            std::vector<ofVec3f> onPlane;
            
            //loop trough all points in triangle object
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
            //Based on the result, calculate intersection points accordingly. Keep in mind that
            //you are ignoring some potential cases here.
            if(topSide.size() > 1){
                //Two points on topside.
                if(bottomSide.size() > 0){
                    calculateInterPoints(topSide[0], topSide[1], bottomSide[0]);
                }
            }
            else if(bottomSide.size() > 1){
                //Two points on bottomside
                if(topSide.size() > 0){
                    calculateInterPoints(bottomSide[0], bottomSide[1], topSide[0]);
                }
            }
            else{
                //This would be the case where there are points that are coincident with
                //the layer plane. Ignore for now.
            }
        }
        t++;
    }
}
//Calculates the intersections points between triangle and layer.
void Layer::calculateInterPoints(ofVec3f target0, ofVec3f target1, ofVec3f orig)
{
    ofVec3f vec0 = orig.operator-(target0);
    ofVec3f vec1 = orig.operator-(target1);
    
    float t0 = (layerHeight-target0.z)/vec0.z;
    float t1 = (layerHeight-target1.z)/vec1.z;
    float x0 = target0.x+vec0.x*t0;
    float x1 = target1.x+vec1.x*t1;
    float y0 = target0.y+vec0.y*t0;
    float y1 = target1.y+vec1.y*t1;
    
    ofVec3f interPoint0 = ofVec3f(x0,y0,layerHeight);
    ofVec3f interPoint1 = ofVec3f(x1,y1, layerHeight);

    intersectionpoints.push_back(interPoint0);
    intersectionpoints.push_back(interPoint1);
    
    ofPolyline line;
    line.addVertex(interPoint0.x, interPoint0.y,interPoint0.z);
    line.addVertex(interPoint1.x, interPoint1.y,interPoint1.z);
    segments.push_back(line);
}
void calculateLines(){
}
