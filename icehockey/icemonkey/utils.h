//
//  utils.h
//  hiopengl
//
//  Created by tienv on 2019-08-01.
//  Copyright © 2019 tienv. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <sstream>
#include <algorithm>
#include <iterator>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// X game seconds = 1 sim second
const float Sim_Speed = 10.0;

// rink sizing
const float Rink_Width_Max = 100;    
const float Rink_Height_Max = 42.5;
float rinkMaxDimension = fmax(Rink_Width_Max, Rink_Height_Max);

//enum State {
//    Move,
//    Leave
//};

struct Pair {
    float X;
    float Y;
    float T;
    bool isEnter;
};

Pair normalize(Pair p) {
    Pair r = {.X = p.X / rinkMaxDimension, .Y = p.Y / rinkMaxDimension};
    return r;
}

float normalize(float f) {
    return f / rinkMaxDimension;
}

std::vector<std::string> split(const std::string& str, char delim = ' ')
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

std::string string_format(const std::string &fmt, ...) {
    int n, size=100;
    std::string str;
    va_list ap;
    
    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf(&str[0], size, fmt.c_str(), ap);
        va_end(ap);
        
        if (n > -1 && n < size)
            return str;
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
}

#endif /* utils_h */
