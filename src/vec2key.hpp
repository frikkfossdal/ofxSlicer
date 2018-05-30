//
//  vec2key.hpp
//  fluffy-octo-potato
//
//  Created by Frikk Fossdal on 30.05.2018.
//

#ifndef vec2key_hpp
#define vec2key_hpp

#include <stdio.h>

class vec2key{
public:
    float x,y,z;
    vec2key(float xValue, float yValue, float zValue){
        x = xValue;
        y = yValue;
        z = zValue;
    }
    bool operator < (const vec2key& other) const{
        if( x == other.x){
            return y < other.y;
        }
        return x < other.x;
    }
};

#endif /* vec2key_hpp */
