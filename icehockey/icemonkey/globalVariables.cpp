//
//  globalVariables.h
//  icehockey
//
//  Created by tienv on 2019-08-22.
//  Copyright © 2019 tienv. All rights reserved.
//

#include "Constants.h"
#include "globalVariables.hpp"

unsigned int _frameBuffer;
unsigned int _defaultBuffer = 0;
unsigned int& frameBuffer = _frameBuffer;
unsigned int& defaultBuffer = _defaultBuffer;

int _r, _g, _b;
int& r = _r;
int& b = _b;
int& g = _g;

float _lastX = SCR_WIDTH / 2.0f;
float _lastY = SCR_HEIGHT / 2.0f;

float& lastX = _lastX;
float& lastY = _lastY;

float _dragX = 0.0;
float _dragY = 0.0;
float& dragX = _dragX;
float& dragY = _dragY;

bool _isDragging = false;
bool& isDragging = _isDragging;

std::map<SelectedObject, glm::vec3> objectColors = {
    {Circle, Colors::LightPurp},
    {Rink, Colors::LawnGreen},
    {Slider, Colors::Blue},
    {SliderHandle, Colors::Orange},
    {None, Colors::Pink}
};

SelectedObject currentObject;
glm::vec3 currentColor;
glm::vec3 handleTranslate = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 lastTranslate = glm::vec3(0.0, 0.0, 0.0);

glm::mat4 view;
glm::mat4 projection;
