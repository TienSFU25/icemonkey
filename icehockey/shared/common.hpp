//
//  common.hpp
//  hiopengl
//
//  Created by tienv on 2019-07-13.
//  Copyright © 2019 tienv. All rights reserved.
//

#ifndef common_hpp
#define common_hpp

#endif /* common_hpp */

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include "Constants.h"

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

enum PlayerState {
    Moving,
    Leaving,
    Entering
};

struct Pair {
    float X;
    float Y;
    float T;
    std::string S;
};

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    FT_Pos Advance;    // Horizontal offset to advance to next glyph
};

namespace Utils {
    unsigned int loadTexture(const char* path);
    std::map<GLchar, Character> loadFonts();
    std::vector<std::string> split(const std::string& str, char delim = ' ');
    Pair normalize(Pair p);
    float normalize(float f);
    float stringToFloat(std::string s);
    int stringToInt(std::string s);
    float ndcToTexCoord(float f);
    
    std::vector<float> genCubeVertices(float bottomX, float bottomY, float bottomZ, float w, float h, float d, float objectId);
    std::vector<float> genQuadVertices(glm::vec2 bottomLeft, float w, float h, glm::vec2 tBottomLeft, float tw, float th, float objectId);
    
    std::vector<float> addColors(std::vector<float> src, glm::vec3 color, int step = 6);
    
    glm::vec4 screenToWorldSpace(glm::vec2 point, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
}
