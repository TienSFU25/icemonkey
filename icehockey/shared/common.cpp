//
//  common.cpp
//  hiopengl
//
//  Created by tienv on 2019-07-13.
//  Copyright © 2019 tienv. All rights reserved.
//
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "common.hpp"
#include "stb_image.h"

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

float rinkMaxDimension = fmax(Rink_Width_Max, Rink_Height_Max);

namespace Utils {
    // utility function for loading a 2D texture from file
    // ---------------------------------------------------
    unsigned int loadTexture(const char* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        
        int width, height, nrComponents;
        stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }
        
        return textureID;
    }
    
    std::map<GLchar, Character> loadFonts() {
        std::map<GLchar, Character> Characters;
        
        // FreeType
        FT_Library ft;
        // All functions return a value different than 0 whenever an error occurred
        if (FT_Init_FreeType(&ft))
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

        // Load font as face
        FT_Face face;
        std::string fontLoc = "/Users/user/Documents/361/opengl/icehockey/icehockey/arial.ttf";

        if (FT_New_Face(ft, fontLoc.c_str(), 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

        // Set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // Set OpenGL options
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Load first 128 characters of ASCII set
        for (GLubyte c = 0; c < 128; c++)
        {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // Generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                         GL_TEXTURE_2D,
                         0,
                         GL_RED,
                         face->glyph->bitmap.width,
                         face->glyph->bitmap.rows,
                         0,
                         GL_RED,
                         GL_UNSIGNED_BYTE,
                         face->glyph->bitmap.buffer
                         );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            Characters.insert(std::pair<GLchar, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        
        return Characters;
    }
    
    std::vector<std::string> split(const std::string& str, char delim)
    {
        std::vector<std::string> container;
        
        int start = 0;
        int end = str.find(delim);
        while (end != std::string::npos)
        {
            //        std::cout << s.substr(start, end - start) << std::endl;
            container.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delim, start);
        }
        
        container.push_back(str.substr(start, end - start));
        
        return container;
    }
    
    Pair normalize(Pair p) {
        Pair r = {.X = p.X / rinkMaxDimension, .Y = p.Y / rinkMaxDimension};
        return r;
    }
    
    float normalize(float f) {
        return f / rinkMaxDimension;
    }
    
    float stringToFloat(std::string s) {
        std::stringstream geek(s);
        
        float x = 0;
        geek >> x;
        
        return x;
    }
    
    int stringToInt(std::string s) {
        std::stringstream geek(s);
        
        int x = 0;
        geek >> x;
        
        return x;
    }
    
    float ndcToTexCoord(float f) {
        return (f + 1) / 2;
    }
    
    std::vector<float> genCubeVertices(float bottomX, float bottomY, float bottomZ, float w, float h, float d, float objectId)
    {        
        std::vector<float> cubeVertices = {
            // positions          // texture Coords
            bottomX, bottomY, bottomZ,  0.0f, 0.0f, objectId,
            bottomX + w, bottomY, bottomZ,  1.0f, 0.0f, objectId,
            bottomX + w, bottomY + h, bottomZ,  1.0f, 1.0f, objectId,
            bottomX + w, bottomY + h, bottomZ,  1.0f, 1.0f, objectId,
            bottomX, bottomY + h, bottomZ,  0.0f, 1.0f, objectId,
            bottomX, bottomY, bottomZ, 0.0f, 0.0f, objectId,

            bottomX, bottomY, bottomZ + d, 0.0f, 0.0f, objectId,
            bottomX + w, bottomY, bottomZ + d, 1.0f, 0.0f, objectId,
            bottomX + w, bottomY + h, bottomZ + d, 1.0f, 1.0f, objectId,
            bottomX + w, bottomY + h, bottomZ + d, 1.0f, 1.0f, objectId,
            bottomX, bottomY + h, bottomZ + d, 0.0f, 1.0f, objectId,
            bottomX, bottomY, bottomZ + d, 0.0f, 0.0f, objectId,
            
            bottomX, bottomY + h, bottomZ + d, 1.0f, 0.0f, objectId,
            bottomX, bottomY + h, bottomZ, 1.0f, 1.0f, objectId,
            bottomX, bottomY, bottomZ, 0.0f, 1.0f, objectId,
            bottomX, bottomY, bottomZ, 0.0f, 1.0f, objectId,
            bottomX, bottomY, bottomZ + d, 0.0f, 0.0f, objectId,
            bottomX, bottomY + h, bottomZ + d, 1.0f, 0.0f, objectId,
            
            bottomX + w, bottomY + h, bottomZ + d,  1.0f, 0.0f, objectId,
            bottomX + w, bottomY + h, bottomZ,  1.0f, 1.0f, objectId,
            bottomX + w, bottomY, bottomZ, 0.0f, 1.0f, objectId,
            bottomX + w, bottomY, bottomZ, 0.0f, 1.0f, objectId,            
            bottomX + w, bottomY, bottomZ + d, 0.0f, 0.0f, objectId,
            bottomX + w, bottomY + h, bottomZ + d,  1.0f, 0.0f, objectId,
            
            bottomX, bottomY, bottomZ + d, 0.0f, 1.0f, objectId,
            bottomX + w, bottomY, bottomZ, 1.0f, 1.0f, objectId,
            bottomX + w, bottomY, bottomZ + d, 1.0f, 0.0f, objectId,
            bottomX + w, bottomY, bottomZ + d, 1.0f, 0.0f, objectId,
            bottomX, bottomY, bottomZ + d, 0.0f, 0.0f, objectId,
            bottomX, bottomY, bottomZ + d, 0.0f, 1.0f, objectId,
            
            bottomX, bottomY + h, bottomZ, 0.0f, 1.0f, objectId,
            bottomX + w, bottomY + h, bottomZ, 1.0f, 1.0f, objectId,
            bottomX + w, bottomY + h, bottomZ + d, 1.0f, 0.0f, objectId,
            bottomX + w, bottomY + h, bottomZ + d, 1.0f, 0.0f, objectId,
            bottomX, bottomY + h, bottomZ + d,  0.0f, 0.0f, objectId,
            bottomX, bottomY + h, bottomZ, 0.0f, 1.0f, objectId,
        };
        
        return cubeVertices;
    }
    
    std::vector<float> genQuadVertices(glm::vec2 bottomLeft, float w, float h, glm::vec2 tBottomLeft, float tw, float th, float objectId) {
        
        float tblx = tBottomLeft.x;
        float tbly = tBottomLeft.y;
        
        std::vector<float> quadVertices = {
            bottomLeft.x, bottomLeft.y, 0, tblx, tbly, objectId,
            bottomLeft.x + w, bottomLeft.y, 0, tblx + tw, tbly, objectId,
            bottomLeft.x + w, bottomLeft.y + h, 0, tblx + tw, tbly + th, objectId,
            bottomLeft.x, bottomLeft.y, 0, tblx, tbly, objectId,
            bottomLeft.x + w, bottomLeft.y + h, 0, tblx + tw, tbly + th, objectId,
            bottomLeft.x, bottomLeft.y + h, 0, tblx, tbly + th, objectId
        };
        
        return quadVertices;
    }
    
    std::vector<float> addColors(std::vector<float> src, glm::vec3 color, int step) {
        std::vector<float> withColors;
        
        for (unsigned int i = 0; i < src.size(); i+= step) {
            for (int j = 0; j < step; j++) {
                float f = src[i+j];
                withColors.push_back(f);
            }

            withColors.push_back(color.r);
            withColors.push_back(color.g);
            withColors.push_back(color.b);
        }
        
        return withColors;
    }
    
    // dangit :|
    glm::vec4 screenToWorldSpace(glm::vec2 point, glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
        glm::vec4 v;
        v.x = point.x / SCR_WIDTH;
        v.y = point.y / SCR_HEIGHT;
        v.z = SliderPosition.z;
        v.w = 1.0;
        
        // to clip space
        v.x = (v.x - 0.5) * 2;
        v.y = (v.y - 0.5) * 2;
        
        // world space
//        v = glm::inverse(model) * glm::inverse(view) * glm::inverse(projection) * v;
        glm::mat4 inv = glm::inverse(projection * view * model);
        v = v * inv;
//        v.x /= v.w;
//        v.y /= v.w;
//        v.z /= v.w;
        
        return v;
    }
}
