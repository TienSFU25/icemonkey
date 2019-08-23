//
//  globalVariables.h
//  icehockey
//
//  Created by tienv on 2019-08-22.
//  Copyright © 2019 tienv. All rights reserved.
//

#ifndef globalVariables_h
#define globalVariables_h

#include "Constants.h"
#include <glm/glm.hpp>
#include <map>

extern unsigned int _frameBuffer;
extern unsigned int _defaultBuffer;
extern unsigned int& frameBuffer;
extern unsigned int& defaultBuffer;

extern int _r, _g, _b;
extern int& r;
extern int& b;
extern int& g;

extern float _lastX, _lastY;
extern float& lastX;
extern float& lastY;

extern float _dragX, _dragY;
extern float& dragX;
extern float& dragY;

extern bool _isDragging;
extern bool& isDragging;

extern std::map<SelectedObject, glm::vec3> objectColors;
extern SelectedObject currentObject;
extern glm::vec3 currentColor;

extern glm::vec3 lastTranslate;
extern glm::vec3 handleTranslate;

extern glm::mat4 view;
extern glm::mat4 projection;

#endif /* globalVariables_h */
