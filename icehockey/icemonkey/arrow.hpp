//
//  arrow.hpp
//  icehockey
//
//  Created by tienv on 2019-08-27.
//  Copyright © 2019 tienv. All rights reserved.
//

#ifndef arrow_hpp
#define arrow_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <vector>
#include "Constants.h"
#include "globalVariables.hpp"

class Arrow
{
public:
    Arrow();
    std::vector<float> getVertices();
    
    glm::vec2 getTailStart();
    glm::vec2 getHeadEnd();
    
    void setTailStart(glm::vec2 newTail);
    void setHeadEnd(glm::vec2 newHead);
    glm::mat4 getRotationMatrix();
private:
    std::vector<float> vertices;
    glm::vec2 tailStart;
    glm::vec2 headEnd;
    float theta;
    float tailW;
    float headW;
    
    const float h = 0.2;
    const float defaultRatio = 2.0 / 3.0;
    const glm::vec2 defaultStart = glm::vec2(0.0, 0.0);
    const glm::vec2 defaultEnd = glm::vec2(0.2, -0.5);
    
    // this texture stuff never changes
    const float textureTailW = 0.68;
    const float textureHeadW = 1 - textureTailW;
    const glm::vec2 bottomLeftTexture = glm::vec2(0, 0);
    const glm::vec2 bottomLeftTexture2 = glm::vec2(textureTailW, 0);
    
    void calculateAngle();
    void fixHeadLength();
    void regenVertices();
};

#endif /* arrow_hpp */
