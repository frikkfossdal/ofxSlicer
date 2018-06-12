//
//  Job.hpp
//  fluffy-octo-potato
//
//  Created by Frikk Fossdal on 14.05.2018.
//

#ifndef Job_hpp
#define Job_hpp

#include <stdio.h>
#include "ofMain.h"
class Job{
public:
    //constructor
    Job();
    
    //variables
    ofPolyline contour;
    int timeSec;
    //methods
    
private:
    void calculateTime();
};
#endif /* Job_hpp */
