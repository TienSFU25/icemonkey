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
using namespace std;

namespace IceHockey {
    unsigned int frameBuffer;
    unsigned int defaultBuffer = 0;
    
    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;
    bool loop = false;
    static const int numVComponents = 3;
    static const int numTComponents = 2;
    float CIRCLE_RADIUS = 0.07f;
    
    Reada rd;
    
    float CIRC_ID = 1.0;
    float RINK_ID = 2.0;
    
    int _r, _g, _b;
    int& r = _r;
    int& b = _b;
    int& g = _g;
    
    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    // font shit
    GLuint planeVBO, planeVAO, circleVBO, circleVAO, TextVBO, TextVAO;
    int numPlaneVertices, numCircleVertices;
    std::map<GLchar, Character> Characters;
    
    // textures
    unsigned int rinkTexture, homeTexture, awayTexture;
    
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
        circleVertices.push_back(CIRC_ID);
        
        for (int i = 0; i < circle_points; i++)
        {
            circleVertices.push_back(CIRCLE_RADIUS * cos(theta));
            circleVertices.push_back(CIRCLE_RADIUS * sin(theta));
            circleVertices.push_back(0.1f);
            
            // texture coords
            circleVertices.push_back(Utils::ndcToTexCoord(cos(theta)));
            circleVertices.push_back(Utils::ndcToTexCoord(sin(theta)));
            circleVertices.push_back(CIRC_ID);

            theta += dtheta;
        }
        
        // add the first point again, to complete circle
        circleVertices.push_back(CIRCLE_RADIUS * cos(0));
        circleVertices.push_back(CIRCLE_RADIUS * sin(0));
        circleVertices.push_back(0.1f);
        circleVertices.push_back(Utils::ndcToTexCoord(cos(0)));
        circleVertices.push_back(Utils::ndcToTexCoord(sin(0)));
        circleVertices.push_back(CIRC_ID);

        return circleVertices;
    }
    
    int main()
    {
        rd.readTheDatFile();
        
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
//        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        
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
        
        // load textures
        rinkTexture = Utils::loadTexture("/Users/user/Documents/361/opengl/icehockey/icehockey/icemonkey/rink.png");
        
        homeTexture = Utils::loadTexture("/Users/user/Documents/361/opengl/icehockey/icehockey/icemonkey/linha.jpg");
        
        awayTexture = Utils::loadTexture("/Users/user/Documents/361/opengl/icehockey/icehockey/icemonkey/tiena.jpg");
        
        // build and compile our shader zprogram
        // ------------------------------------
        std::string loc =  "/Users/user/Documents/361/opengl/icehockey/icehockey/shaders/";
        std::string F1 = loc + "scene.vs";
        std::string F2 = loc + "color.fs";
        std::string F3 = loc + "colorOrTexture.fs";
        std::string F4 = loc + "text.vs";
        std::string F5 = loc + "text.fs";
        std::string F6 = loc + "quad.vs";
        
        Shader sceneShader(F1, F3);
        Shader colorOnlyShader(F1, F2);
        Shader textShader(F4, F5);
        Shader quadShader(F6, F3);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float normalizedRinkWidth = Utils::normalize(Rink_Width_Max);
        float normalizedRinkHeight = Utils::normalize(Rink_Height_Max);

        std::vector<float> planeVertices = {
            -normalizedRinkWidth, -normalizedRinkHeight,  0.0f,  0.0f, 0.0f, RINK_ID, // bottom-left
            normalizedRinkWidth, -normalizedRinkHeight,  0.0f,  1.0f, 0.0f, RINK_ID, // bottom-right
            normalizedRinkWidth,  normalizedRinkHeight,  0.0f,  1.0f, 1.0f, RINK_ID, // top-right
            normalizedRinkWidth,  normalizedRinkHeight,  0.0f,  1.0f, 1.0f, RINK_ID, // top-right
            -normalizedRinkWidth,  normalizedRinkHeight,  0.0f,  0.0f, 1.0f, RINK_ID, // top-left
            -normalizedRinkWidth, -normalizedRinkHeight,  0.0f,  0.0f, 0.0f, RINK_ID // bottom-left
        };
        
        int numAttribPerVertex = 5;
        numPlaneVertices = (int)planeVertices.size() / numAttribPerVertex;
        
        // configure plane VAO (and VBO)
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), &planeVertices[0], GL_STATIC_DRAW);
        
        glBindVertexArray(planeVAO);
        
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // texture attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // object ID attribute
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
        // circle vertex
        std::vector<float> circleVertices = initCircleVertices();
        numCircleVertices = (int)circleVertices.size() / 3;
        
        // circle uses different vertices and VAO
        glGenBuffers(1, &circleVBO);
        glGenVertexArrays(1, &circleVAO);

        glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
        glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), &circleVertices[0], GL_STATIC_DRAW);

        glBindVertexArray(circleVAO);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // texture attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        // object ID attribute
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
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
        
        glfwSetMouseButtonCallback(window, IceHockey::mouse_button_callback);
        
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f, 0.5, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.5, 0.0f, 0.0f,
            1.0f, -1.0f, 0.5, 1.0f, 0.0f,

            -1.0f,  1.0f, 0.5, 0.0f, 1.0f,
            1.0f, -1.0f, 0.5, 1.0f, 0.0f,
            1.0f,  1.0f, 0.5, 1.0f, 1.0f
        };
        
        // screen quad VAO
        unsigned int quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        
        // framebuffer configuration
        // -------------------------
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        // create a color attachment texture
        unsigned int textureColorbuffer;
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
        
        // attach color/depth/stencil "attachments" (wtf?) to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        bool d = true;
        
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
                
                rd.move(deltaTime);

                // render
                // first render to (invisible) "color instancing" framebuffer
                glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
                glEnable(GL_DEPTH_TEST);
                IceHockey::RenderToBuffer(colorOnlyShader, textShader, currentFrame);
                
                // now render the scene
                glBindFramebuffer(GL_FRAMEBUFFER, defaultBuffer);
                glEnable(GL_DEPTH_TEST);
                IceHockey::RenderToBuffer(sceneShader, textShader, currentFrame);
                
                // "indicator" circle
                sceneShader.use();
                glBindVertexArray(circleVAO);
                glm::mat4 indicatorModel = glm::mat4(1.0f);
                glm::vec3 trans = glm::vec3(0.5, 0.5, 0.0);
                indicatorModel = glm::translate(indicatorModel, trans);
                
                sceneShader.setVec3("aColor", glm::vec3(r / 255.0, g / 255.0, b / 255.0));
                sceneShader.setMat4("model", indicatorModel);
                glDrawArrays(GL_TRIANGLE_FAN, 0, numCircleVertices);
                
                // if necessary, could dump color instance frame to viewbuffer
//                glBindFramebuffer(GL_FRAMEBUFFER, 0);
//                glDisable(GL_DEPTH_TEST);
//                glClear(GL_COLOR_BUFFER_BIT);
//
//                quadShader.use();
//                glBindVertexArray(quadVAO);
//                glBindTexture(GL_TEXTURE_2D, textureColorbuffer);    // use the color attachment texture as the texture of the quad plane
//                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            
            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
            
            if (d) {
                // center mr mouse
                glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
                d = false;
            }
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
    
    void RenderToBuffer(Shader shader, Shader textShader, float currentFrame)
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // draw the texturized rink
        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rinkTexture);
        
        // mvp
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        
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
        
        std::map<int, Playa>::iterator it = rd.players.begin();
        
        // draw the circles
        while(it != rd.players.end()) {
            shader.use();
            glBindVertexArray(circleVAO);
            
            int playerId = it -> first;
            Playa playa = it -> second;
            
            glm::mat4 circleModel = glm::mat4(1.0f);
            glm::vec3 trans = playa.getTranslationMatrix();
            circleModel = glm::translate(circleModel, trans);
            
            if (playa.getState() == Entering) {
                shader.setVec3("aColor", Colors::LawnGreen);
            } else if (playa.getState() == Leaving) {
                shader.setVec3("aColor", Colors::OrangeRed);
            } else {
                if (playa.isHomeTeam) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, homeTexture);
                    shader.setVec3("aColor", Colors::LightPurp);
                } else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, awayTexture);
                    shader.setVec3("aColor", Colors::Orange);
                }
            }
            
            if (playa.isHomeTeam) {
                // avoid z fighting
                circleModel = glm::translate(circleModel, glm::vec3(0.0, 0.0, 0.1f));
            }
            
            shader.setMat4("model", circleModel);
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
//        std::cout << lastX << ", " << lastY << std::endl;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
    
    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera.ProcessMouseScroll(yoffset);
    }
    
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mode) {
//        std::cout << "left mouse click!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            vector< unsigned char > pixels( 1 * 1 * 4 );
            int x = lastX;
            int y = SCR_HEIGHT - lastY;
//            std::cout << x << ", " << y << std::endl;

            glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
            r = (int)pixels[0];
            g = (int)pixels[1];
            b = (int)pixels[2];

            cout << "r: " << r << endl;
            cout << "g: " << g << endl;
            cout << "b: " << b << endl;
            cout << "a: " << (int)pixels[3] << endl;
            cout << endl;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultBuffer);
    }
}
