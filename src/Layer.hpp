//
//  Layer.hpp
//  slicer
//
//  Created by Frikk Fossdal on 20.02.2018.
//

#ifndef Layer_hpp
#define Layer_hpp

#include "ofMain.h"
#include "Triangles.hpp"
#include "Job.hpp"
#include <stdio.h>
class Layer{
public:
    //Variables
    float layerHeight;
    std::vector<ofVec3f> intersectionpoints;
    std::vector<ofPolyline> segments; //remove when ready
    std::vector<ofPolyline> contours; //remove when ready
    std::vector<Job> jobs; 
    
    //Constructor
    Layer();
    Layer(float _layerHeight);

    //Method
    void show();
    void calculate(std::vector<Triangles> triangles);
    std::vector<Triangles> findActive(std::vector<Triangles> _activeTriangles, std::vector<Triangles> _triangleList);
private:
    void calculateInterPoints(ofVec3f target0, ofVec3f target1, ofVec3f orig);
    void calculateLines(); 
};

#endif /* Layer_hpp */
