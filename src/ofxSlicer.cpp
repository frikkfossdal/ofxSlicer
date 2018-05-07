//
//  ofxSlicer.cpp
//
//
//  Created by Frikk H Fossdal on 07.05.2018.
//

#include "ofxSlicer.h"

ofxSlicer::ofxSlicer(){
    
}
void ofxSlicer::loadFile(string _pathToFile){
    try {
        model.loadModel(_pathToFile);
    } catch (exception& e) {
        std::cout << ("load failed. Check file if file exist at given location.") << endl;
    }
}

