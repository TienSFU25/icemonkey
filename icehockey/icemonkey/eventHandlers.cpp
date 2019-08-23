//
//  eventHandlers.cpp
//  icehockey
//
//  Created by tienv on 2019-08-22.
//  Copyright © 2019 tienv. All rights reserved.
//

#include <glad/glad.h>
#include "eventHandlers.hpp"
#include "common.hpp"

void EventHandler::handleMouseClick(GLFWwindow* window, int button, int action, int mode) {
    //        std::cout << "left mouse click!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::vector< unsigned char > pixels( 1 * 1 * 4 );
        int x = lastX;
        int y = SCR_HEIGHT - lastY;
        
        glm::mat4 defaultModel;
        glm::vec4 worldPoint = Utils::screenToWorldSpace(glm::vec2(x, y), defaultModel, view, projection);
        std::cout << x << ", " << y << ". " << worldPoint.x << ", " << worldPoint.y << ", " << worldPoint.z << std::endl;

        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
        r = (int)pixels[0];
        g = (int)pixels[1];
        b = (int)pixels[2];
        
        for (std::map<SelectedObject, glm::vec3>::iterator it=objectColors.begin(); it!=objectColors.end(); ++it) {
            SelectedObject obj = it -> first;
            glm::vec3 color = it -> second;
            
            if (colorCompare(r, g, b, color)) {
                currentObject = obj;
                currentColor = color;
                
                if (currentObject == SliderHandle) {
                    isDragging = true;
                    dragX = lastX;
                    dragY = lastY;
                }
                
                return;
            }
        }
        
        currentObject = None;
        currentColor = objectColors[currentObject];
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        std::cout << "released!" << std::endl;
        isDragging = false;
        lastTranslate = handleTranslate;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, defaultBuffer);
}

bool EventHandler::colorCompare(int r, int g, int b, glm::vec3 to) {
    float nr = r / 255.0;
    float ng = g / 255.0;
    float nb = b / 255.0;
    
    if (fabs(to.r - nr) < 0.01f && fabs(to.g - ng) < 0.01f && fabs(to.b - nb) < 0.01f)
        return true;
    
    return false;
}
