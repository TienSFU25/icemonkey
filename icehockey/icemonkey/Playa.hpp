#ifndef PLAYA_H
#define PLAYA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <iostream>
#include "utils.h"

const Pair defaultInitialPosition = {.X = -2.0f, .Y = 2.0f};

class Playa
{
public:
    bool isHomeTeam;
    
    Playa(Pair initialPosition = defaultInitialPosition, bool _isHomeTeam = false)
    {
        xPos = initialPosition.X;
        yPos = initialPosition.Y;
        isHomeTeam = _isHomeTeam;
    }
    
    void setMoveTo(Pair _destination, float overTime) {
        destination = _destination;
        
        timeRemain = overTime;
    }
    
    void setMoveOffscreen(float overTime) {
        setMoveTo(defaultInitialPosition, overTime);
    }
    
    bool move(float deltaTime) {
        float xRemain = destination.X - xPos;
        float yRemain = destination.Y - yPos;
        
        // move X, Y
        if (timeRemain > 0.01f && (abs(xRemain) > 0.01f || abs(yRemain) > 0.01f)) {
            float stepX = (deltaTime / timeRemain) * xRemain;
            float stepY = (deltaTime / timeRemain) * yRemain;
            
            xPos += stepX;
            yPos += stepY;
            timeRemain -= deltaTime;
            
            return true;
        }
        
        return false;
    }
    
    glm::vec3 getTranslationMatrix() {
        return glm::vec3(xPos, yPos, 0.0f);
    }

    
private:
    float xPos = 0.0f;
    float yPos = 0.0f;
    float timeRemain = 10.0f;
    Pair destination;
};
#endif
