//
//  hockey.cpp
//  hiopengl
//
//  Created by tienv on 2019-07-31.
//  Copyright © 2019 tienv. All rights reserved.
//

#include <glm/glm.hpp>
#include <boost/format.hpp>
#include <nlohmann/json.hpp>

#include "hockey.hpp"
#include "Camera.h"
#include "Playa.hpp"
#include "Reada.h"

#include <iostream>

// for convenience
using json = nlohmann::json;

namespace IceHockey {
    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;
    bool loop = false;
    static const int numVComponents = 3;
    static const int numTComponents = 2;
    float CIRCLE_RADIUS = 0.07f;

    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    GLuint TextVBO, TextVAO;
    
    std::map<GLchar, Character> Characters;
    
    std::vector<float> initCircleVertices() {
        int circle_points = 50;
        double dtheta = 2*  3.1416f/circle_points ;
        double theta = 0.0;
        std::vector<float> circleVertices = {};
        
        // add the first point
        circleVertices.push_back(CIRCLE_RADIUS * cos(0));
        circleVertices.push_back(CIRCLE_RADIUS * sin(0));
        circleVertices.push_back(0.1f);
        circleVertices.push_back(Utils::ndcToTexCoord(cos(0)));
        circleVertices.push_back(Utils::ndcToTexCoord(sin(0)));
        
        for (int i = 0; i < circle_points; i++)
        {
            circleVertices.push_back(CIRCLE_RADIUS * cos(theta));
            circleVertices.push_back(CIRCLE_RADIUS * sin(theta));
            circleVertices.push_back(0.1f);
            
            // texture coords
            circleVertices.push_back(Utils::ndcToTexCoord(cos(theta)));
            circleVertices.push_back(Utils::ndcToTexCoord(sin(theta)));
            
            theta += dtheta;
        }
        
        // add the first point again, to complete circle
        circleVertices.push_back(CIRCLE_RADIUS * cos(0));
        circleVertices.push_back(CIRCLE_RADIUS * sin(0));
        circleVertices.push_back(0.1f);
        circleVertices.push_back(Utils::ndcToTexCoord(cos(0)));
        circleVertices.push_back(Utils::ndcToTexCoord(sin(0)));
        
        return circleVertices;
    }
    
    int main()
    {
        Reada r;
        r.readTheDatFile();
        
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
        
        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ice hockey demo", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, IceHockey::framebuffer_size_callback);
        glfwSetCursorPosCallback(window, IceHockey::mouse_callback);
        glfwSetScrollCallback(window, IceHockey::scroll_callback);
        
        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }
        
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        
        // build and compile our shader zprogram
        // ------------------------------------
        std::string loc =  "/Users/user/Documents/361/opengl/icehockey/icehockey/shaders/";
        std::string F1 = loc + "rink.vs";
        std::string F2 = loc + "rink.fs";
        std::string F3 = loc + "circle.fs";
        std::string F4 = loc + "text.vs";
        std::string F5 = loc + "text.fs";
        
        Shader lightingShader(F1, F2);
        Shader circleShader(F1, F3);
        Shader textShader(F4, F5);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float normalizedRinkWidth = Utils::normalize(Rink_Width_Max);
        float normalizedRinkHeight = Utils::normalize(Rink_Height_Max);

        std::vector<float> planeVertices = {
            -normalizedRinkWidth, -normalizedRinkHeight,  0.0f,  0.0f, 0.0f, // bottom-left
            normalizedRinkWidth, -normalizedRinkHeight,  0.0f,  1.0f, 0.0f, // bottom-right
            normalizedRinkWidth,  normalizedRinkHeight,  0.0f,  1.0f, 1.0f, // top-right
            normalizedRinkWidth,  normalizedRinkHeight,  0.0f,  1.0f, 1.0f, // top-right
            -normalizedRinkWidth,  normalizedRinkHeight,  0.0f,  0.0f, 1.0f, // top-left
            -normalizedRinkWidth, -normalizedRinkHeight,  0.0f,  0.0f, 0.0f, // bottom-left
        };
        
        int numAttribPerVertex = 5;
        int numPlaneVertices = (int)planeVertices.size() / numAttribPerVertex;
        
        // configure plane VAO (and VBO)
        unsigned int planeVBO, planeVAO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), &planeVertices[0], GL_STATIC_DRAW);
        
        glBindVertexArray(planeVAO);
        
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // texture attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        // circle vertex
        std::vector<float> circleVertices = initCircleVertices();
        int numCircleVertices = (int)circleVertices.size() / 3;
        std::cout << numCircleVertices << std::endl;
        
        // circle uses different vertices and VAO
        unsigned int circleVBO, circleVAO;
        glGenBuffers(1, &circleVBO);
        glGenVertexArrays(1, &circleVAO);

        glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
        glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), &circleVertices[0], GL_STATIC_DRAW);

        glBindVertexArray(circleVAO);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // texture attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        // load textures
        unsigned int someTexture = Utils::loadTexture("/Users/user/Documents/361/opengl/icehockey/icehockey/icemonkey/rink.png");

        unsigned int homeTexture = Utils::loadTexture("/Users/user/Documents/361/opengl/icehockey/icehockey/icemonkey/linha.jpg");

        unsigned int awayTexture = Utils::loadTexture("/Users/user/Documents/361/opengl/icehockey/icehockey/icemonkey/tiena.jpg");
        
        glfwSetKeyCallback(window, IceHockey::key_callback);
        
        // font shit
        Characters = Utils::loadFonts();

        // setup the text quads
        glGenVertexArrays(1, &TextVAO);
        glGenBuffers(1, &TextVBO);
        glBindVertexArray(TextVAO);
        glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * (numVComponents + numTComponents), NULL, GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, numVComponents, GL_FLOAT, GL_FALSE, (numVComponents + numTComponents) * sizeof(GLfloat), 0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, numTComponents, GL_FLOAT, GL_FALSE, (numVComponents + numTComponents) * sizeof(GLfloat), (void*)(numVComponents * sizeof(float)));
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        // render loop
        // -----------
        while (!glfwWindowShouldClose(window))
        {
            // per-frame time logic
            // --------------------
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            if (!loop) {
                // input
                // -----
                IceHockey::processInput(window);
                
                r.move(deltaTime);

                // render
                // ------
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                // be sure to activate shader when setting uniforms/drawing objects
                lightingShader.use();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, someTexture);
                
                // mvp
                glm::mat4 model = glm::mat4(1.0f);
                lightingShader.setMat4("model", model);
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
                glm::mat4 view = camera.GetViewMatrix();
                lightingShader.setMat4("projection", projection);
                lightingShader.setMat4("view", view);
                
                glBindVertexArray(planeVAO);
                glDrawArrays(GL_TRIANGLES, 0, numPlaneVertices);

                // set the text projection
                GLfloat W = static_cast<GLfloat>(SCR_WIDTH);
                GLfloat H = static_cast<GLfloat>(SCR_HEIGHT);
                glm::mat4 textProjection = glm::ortho(-W / 2, W / 2, -H / 2, H / 2);
                
                textShader.use();
                glm::mat4 identity;
                textShader.setMat4("model", identity);
                textShader.setMat4("view", identity);
                textShader.setMat4("projection", textProjection);

                std::string s = (boost::format("time: %4.5f ms") % currentFrame).str();
                IceHockey::RenderStaticText(textShader, s, -W / 2 + 50, -H / 2 + 50, 0.5, glm::vec3(0.3, 0.7f, 0.9f));

                std::map<int, Playa>::iterator it = r.players.begin();

                // draw the circles
                circleShader.use();
                circleShader.setMat4("projection", projection);
                circleShader.setMat4("view", view);

                while(it != r.players.end()) {
                    circleShader.use();
                    glBindVertexArray(circleVAO);

                    int playerId = it -> first;
                    Playa playa = it -> second;
                    
                    glm::mat4 circleModel = glm::mat4(1.0f);
                    glm::vec3 trans = playa.getTranslationMatrix();
                    circleModel = glm::translate(circleModel, trans);
                    
                    if (playa.getState() == Entering) {
                        circleShader.setVec3("aColor", Colors::LawnGreen);
                    } else if (playa.getState() == Leaving) {
                        circleShader.setVec3("aColor", Colors::OrangeRed);
                    } else {
                        if (playa.isHomeTeam) {
                            glActiveTexture(GL_TEXTURE0);
                            glBindTexture(GL_TEXTURE_2D, homeTexture);
                            circleShader.setVec3("aColor", Colors::LightPurp);
                        } else {
                            glActiveTexture(GL_TEXTURE0);
                            glBindTexture(GL_TEXTURE_2D, awayTexture);
                            circleShader.setVec3("aColor", Colors::Orange);
                        }
                    }
                    
                    if (playa.isHomeTeam) {
                        // avoid z fighting
                        circleModel = glm::translate(circleModel, glm::vec3(0.0, 0.0, 0.1f));
                    }
                    
                    circleShader.setMat4("model", circleModel);
                    glDrawArrays(GL_TRIANGLE_FAN, 0, numCircleVertices);
                    
                    textShader.use();
                    glm::mat4 textModel;
                    glm::vec3 translateText = glm::vec3(-CIRCLE_RADIUS / 2, -CIRCLE_RADIUS / 3, 0);
                    textModel = glm::translate(circleModel, translateText);
                    
                    textShader.setMat4("model", textModel);
                    textShader.setMat4("view", view);
                    textShader.setMat4("projection", projection);

                    IceHockey::RenderText(textShader, std::to_string(playerId), 0.0, 0.0, 0.75, glm::vec3(0.0, 0.0, 0.0));
                    it++;
                }
            }
            
            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        glDeleteVertexArrays(1, &planeVAO);
        glDeleteVertexArrays(1, &circleVAO);
        glDeleteBuffers(1, &circleVBO);
        glDeleteBuffers(1, &planeVBO);
        
        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        glfwTerminate();
        return 0;
    }
    
    void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
    {
        // assume constant text height for now
        GLfloat h = CIRCLE_RADIUS * scale;
        GLfloat w = CIRCLE_RADIUS * 0.6 * scale;
        GLfloat z = 0.11;
        
        // Activate corresponding render state
        shader.use();
        glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(TextVAO);
        
        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];
            // Update VBO for each character
            GLfloat vertices[6][numVComponents + numTComponents] = {
                { x,     y + h, z,   0.0, 0.0 },
                { x, y,    z,   0.0, 1.0 },
                { x + w, y,    z,   1.0, 1.0 },
                
                { x, y + h, z,  0.0, 0.0 },
                { x + w, y, z,   1.0, 1.0 },
                { x + w, y + h, z,  1.0, 0.0 }
            };
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            x += w;
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void RenderStaticText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
    {
        // Activate corresponding render state
        shader.use();
        glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(TextVAO);
        
        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];
            
            GLfloat xpos = x + ch.Bearing.x * scale;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            
            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;
            // Update VBO for each character
            GLfloat vertices[6][numVComponents + numTComponents] = {
                { xpos,     ypos + h, 1.0,   0.0, 0.0 },
                { xpos,     ypos,    1.0,   0.0, 1.0 },
                { xpos + w, ypos,    1.0,   1.0, 1.0 },
                
                { xpos,     ypos + h, 1.0,  0.0, 0.0 },
                { xpos + w, ypos,     1.0,   1.0, 1.0 },
                { xpos + w, ypos + h, 1.0,  1.0, 0.0 }
            };
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
            loop = !loop;
    }
    
    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    
    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }
    
    
    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        
        lastX = xpos;
        lastY = ypos;
        
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    
    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera.ProcessMouseScroll(yoffset);
    }
}
