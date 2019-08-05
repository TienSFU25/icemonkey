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

// rink sizing
const float Rink_Width_Max = 100;
const float Rink_Height_Max = 42.5;
float rinkMaxDimension = fmax(Rink_Width_Max, Rink_Height_Max);

struct Pair {
    float X;
    float Y;
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

//    std::istringstream iss(str);
//    std::copy(std::istream_iterator<std::string>(iss),
//              std::istream_iterator<std::string>(),
//              std::back_inserter(container));

//    std::stringstream ss(str);
//    std::string token;
//    while (std::getline(ss, token, delim)) {
//        container.push_back(token);
//    }

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

#endif /* utils_h */
