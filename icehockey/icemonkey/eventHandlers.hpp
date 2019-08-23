//
//  eventHandlers.h
//  icehockey
//
//  Created by tienv on 2019-08-22.
//  Copyright © 2019 tienv. All rights reserved.
//

#ifndef eventHandlers_h
#define eventHandlers_h

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <map>

#include "Constants.h"
#include "globalVariables.hpp"

class EventHandler
{
public:
    static void handleMouseClick(GLFWwindow* window, int button, int action, int mode);
private:
    static bool colorCompare(int r, int g, int b, glm::vec3 to);
};

#endif /* eventHandlers_h */
