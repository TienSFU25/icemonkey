//
//  arrow.cpp
//  icehockey
//
//  Created by tienv on 2019-08-27.
//  Copyright © 2019 tienv. All rights reserved.
//

#include "arrow.hpp"
#include "common.hpp"

Arrow::Arrow() {
    tailStart = defaultStart;
    headEnd = defaultEnd;
//    theta = glm::radians(-90.0);
    
    calculateAngle();
    fixHeadLength();
    regenVertices();
}

std::vector<float> Arrow::getVertices() {
    return vertices;
}

void Arrow::calculateAngle() {
    theta = glm::atan(headEnd.y - tailStart.y, headEnd.x - tailStart.x);
//    std::cout << "x, y: " << headEnd.x - tailStart.x << ", " << headEnd.y - tailStart.y << ", theta is " << theta << std::endl;
}

void Arrow::fixHeadLength() {
    float dist = glm::distance(tailStart, headEnd);
    headW = dist * ( 1 - defaultRatio );
}

void Arrow::regenVertices() {
//    glm::vec2 headStart = glm::mix(tailStart, headEnd, defaultRatio);
    float dist = glm::distance(tailStart, headEnd);
    tailW = dist - headW;
    
//    glm::vec2 headStart = glm::vec2(tailStart + tailW, 0);
//    std::cout << "moving head to " << tailStart.x + tailW << std::endl;
    glm::vec2 tailBottomLeft = glm::vec2(tailStart.x, tailStart.y - h / 2);
    glm::vec2 headBottomLeft = glm::vec2(tailStart.x + tailW, tailStart.y - h / 2);
    
    std::vector<float> lineVertices = Utils::genQuadVertices(tailBottomLeft, tailW, h, bottomLeftTexture, textureTailW, 1, 2);
    std::vector<float> moreLineVertices = Utils::genQuadVertices(headBottomLeft, headW, h, bottomLeftTexture2, textureHeadW, 1, 2);
    lineVertices.insert( lineVertices.end(), moreLineVertices.begin(), moreLineVertices.end() );
    //    lineVertices = moreLineVertices;
    vertices = Utils::addColors(lineVertices, Colors::Black);
}

void Arrow::setTailStart(glm::vec2 newTail) {
    tailStart = newTail;
    calculateAngle();
    regenVertices();
}

void Arrow::setHeadEnd(glm::vec2 newHead) {
    headEnd = newHead;
    calculateAngle();
    regenVertices();
}

glm::vec2 Arrow::getTailStart() {
    return tailStart;
}

glm::vec2 Arrow::getHeadEnd() {
    glm::vec4 headEndFlat = glm::vec4(tailStart.x + tailW + headW, tailStart.y, 0, 1);
    glm::vec4 v = getRotationMatrix() * headEndFlat;
    
    return glm::vec2(v.x, v.y);
}

glm::mat4 Arrow::getRotationMatrix() {
    glm::mat4 identity;
    glm::vec3 v = glm::vec3(0, 0, 1);
    glm::mat4 translateBack = glm::translate(identity, glm::vec3(-tailStart.x, -tailStart.y, 0));
    glm::mat4 rotate = glm::rotate(identity, theta, v);
    glm::mat4 translateForward = glm::translate(identity, glm::vec3(tailStart.x, tailStart.y, 0));
    
    return translateForward * rotate * translateBack;
}
