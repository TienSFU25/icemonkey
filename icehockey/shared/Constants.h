//
//  Constants.h
//  icehockey
//
//  Created by tienv on 2019-08-15.
//  Copyright © 2019 tienv. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

#include <glm/glm.hpp>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float SLIDER_LENGTH = 1.0;

// X game seconds = 1 sim second
const float Sim_Speed = 1.0;

// rink sizing
const float Rink_Width_Max = 100;
const float Rink_Height_Max = 42.5;

const float DragSensitivity = 100.0;

const glm::vec3 SliderPosition = glm::vec3(0.5, 0.5, 0.3);

enum SelectedObject {
    Circle,
    Rink,
    Slider,
    SliderHandle,
    None
};

namespace Colors {
    const glm::vec3 Red = glm::vec3(1, 0, 0);
    const glm::vec3 Green = glm::vec3(0, 1, 0);
    const glm::vec3 Blue = glm::vec3(0, 0, 1);
    const glm::vec3 Black = glm::vec3(0, 0, 0);
    
    const glm::vec3 LightPurp = glm::vec3(138.0 / 255.0, 43.0 / 255.0, 226.0 / 255.0);
    const glm::vec3 Orange = glm::vec3(255.0 / 255.0, 140.0 / 255.0, 0.0 / 255.0);
    const glm::vec3 LawnGreen = glm::vec3(124 / 255.0, 252 / 255.0, 0);
    const glm::vec3 OrangeRed = glm::vec3(1.0, 69.0 / 255.0, 0);
    const glm::vec3 Pink = glm::vec3(255.0 / 255.0, 192 / 255.0, 203 / 255.0);
}

#endif /* Constants_h */
