//
//  hockey.hpp
//  hiopengl
//
//  Created by tienv on 2019-07-31.
//  Copyright © 2019 tienv. All rights reserved.
//

#ifndef hockey_hpp
#define hockey_hpp

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "common.hpp"

#endif /* hockey_hpp */

namespace IceHockey {
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void processInput(GLFWwindow *window);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    int main();
}
