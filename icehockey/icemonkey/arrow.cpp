//
//  arrow.cpp
//  icehockey
//
//  Created by tienv on 2019-08-27.
//  Copyright © 2019 tienv. All rights reserved.
//

#include "arrow.hpp"
#include "common.hpp"

Arrow::Arrow(std::vector<ArrowState> states) {
    arrowStates = states;
    currentState = states[0];
    counter = 0;
    setNextDest();
    
    fixHeadLength();
    regenVertices();
}

std::vector<float> Arrow::getVertices() {
    return vertices;
}

void Arrow::move(float deltaTime) {
    glm::vec2 startRemain = nextState.Start - currentState.Start;
    glm::vec2 endRemain = nextState.End - currentState.End;
    
    // move X, Y
    if (timeRemain > 0.01f) {
        glm::vec2 stepX = (deltaTime / timeRemain) * startRemain;
        glm::vec2 stepY = (deltaTime / timeRemain) * endRemain;
        
        currentState.Start += stepX;
        currentState.End += stepY;
        timeRemain -= deltaTime;
        
        if (headW < 0.1f)
            fixHeadLength();
        regenVertices();
    } else {
        if (counter < arrowStates.size()) {
            if (headW < 0.1f)
                fixHeadLength();

            setNextDest();
            move(deltaTime);
        }
    }
}

void Arrow::setNextDest() {
    counter += 1;
    nextState = arrowStates[counter];
    ArrowState previousState = arrowStates[counter - 1];

    float timeExceeded = timeRemain * -1;
    
    timeRemain = (nextState.Time - previousState.Time) / Sim_Speed;
    timeRemain -= timeExceeded;
}

void Arrow::fixHeadLength() {
    float dist = glm::distance(currentState.Start, currentState.End);
    headW = dist * ( 1 - defaultRatio );
}

void Arrow::regenVertices() {
    glm::vec2 start = currentState.Start;
    glm::vec2 end = currentState.End;

    // recalc angle
    theta = glm::atan(end.y - start.y, end.x - start.x);

    float dist = glm::distance(start, end);
    tailW = dist - headW;
    
    glm::vec2 tailBottomLeft = glm::vec2(start.x, start.y - h / 2);
    glm::vec2 headBottomLeft = glm::vec2(start.x + tailW, start.y - h / 2);
    
    std::vector<float> lineVertices = Utils::genQuadVertices(tailBottomLeft, tailW, h, bottomLeftTexture, textureTailW, 1, 2);
    std::vector<float> moreLineVertices = Utils::genQuadVertices(headBottomLeft, headW, h, bottomLeftTexture2, textureHeadW, 1, 2);
    lineVertices.insert( lineVertices.end(), moreLineVertices.begin(), moreLineVertices.end() );
    //    lineVertices = moreLineVertices;
    vertices = Utils::addColors(lineVertices, Colors::Black);
}

glm::mat4 Arrow::getRotationMatrix() {
    glm::vec2 start = currentState.Start;
    glm::vec2 end = currentState.End;
    
    glm::mat4 identity;
    glm::vec3 v = glm::vec3(0, 0, 1);
    glm::mat4 translateBack = glm::translate(identity, glm::vec3(-start.x, -start.y, 0));
    glm::mat4 rotate = glm::rotate(identity, theta, v);
    glm::mat4 translateForward = glm::translate(identity, glm::vec3(start.x, start.y, 0));
    
    return translateForward * rotate * translateBack;
}
