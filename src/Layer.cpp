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
    for(auto it = contours.begin(); it !=contours.end(); it++){
        ofSetColor(200);
        it->draw();
    }

}
void Layer::showIntersections(){
    for(auto it = intersectionpoints.begin(); it!=intersectionpoints.end(); it++)
    {
        ofSetColor(255, 0, 0);
        ofDrawEllipse(it->x, it->y, it->z, .1, .1);
    }
}

//Not in use yet. Implement later to increase calculation speed

