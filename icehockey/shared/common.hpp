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

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

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
}
