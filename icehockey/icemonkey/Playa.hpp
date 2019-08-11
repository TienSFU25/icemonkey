#ifndef PLAYA_H
#define PLAYA_H

#include <glm/glm.hpp>
#include <vector>
#include "utils.h"

const Pair defaultInitialPosition = {.X = -2.0f, .Y = 2.0f};

class Playa
{
public:
    bool isHomeTeam;
    std::vector<Pair> destinations;
    
    Playa(std::vector<Pair> d, int pid = -1, bool _isHomeTeam = false)
    {
        destinations = d;
        id = pid;
        isHomeTeam = _isHomeTeam;

        counter = 1;
        nextDestination = destinations[counter];
        
        xPos = 0 + destinations[0].X;
        yPos = 0 + destinations[0].Y;
        timeRemain = (nextDestination.T - destinations[0].T) / Sim_Speed;
    }
    
//    void setMoveTo(Pair _destination, float overTime) {
//        destination = _destination;
//
//        timeRemain = overTime;
//    }
//
//    void setMoveOffscreen(float overTime) {
//        setMoveTo(defaultInitialPosition, overTime);
//    }
    
//    void leave(float deltaTime, float overTime) {
//        if (state == Leaving) {
//            timeRemain = overTime;
//            state = Moving;
//            nextDestination = defaultInitialPosition;
//            counter += 1;
//            move(deltaTime);
//        }
//    }
//
//    void enter(float deltaTime, float globalTime) {
//        if (state == Entering) {
////            if (counter == 0) {
//            nextDestination = destinations[counter];
//            timeRemain = nextDestination.T - globalTime;
//            state = Moving;
//            move(deltaTime);
//
//            counter += 1;
////            } else {
////                counter += 1;
////
////                if (counter < destinations.size()) {
////                    nextDestination = destinations[counter];
////                    timeRemain = nextDestination.T - globalTime;
////                    state = Moving;
////                    move(deltaTime);
////                }
////            }
//        }
//    }
    
    void move(float deltaTime) {
        float xRemain = nextDestination.X - xPos;
        float yRemain = nextDestination.Y - yPos;
        
        // move X, Y
        if (timeRemain > 0.01f && (abs(xRemain) > 0.01f || abs(yRemain) > 0.01f)) {
            float stepX = (deltaTime / timeRemain) * xRemain;
            float stepY = (deltaTime / timeRemain) * yRemain;
            
            xPos += stepX;
            yPos += stepY;
            timeRemain -= deltaTime;
        } else {
            if (counter < destinations.size()) {
                nextDestination = destinations[counter];
                
                // "teleport" and just stand there
                if (nextDestination.isEnter) {
                    xPos = nextDestination.X;
                    yPos = nextDestination.Y;
                }
//                std::cout << "moving " << id << " to (" << nextDestination.X << ", " << nextDestination.Y << ")" << std::endl;
                timeRemain = (nextDestination.T - destinations[counter - 1].T) / Sim_Speed;
                counter += 1;
            }
        }
    }
    
    glm::vec3 getTranslationMatrix() {
        return glm::vec3(xPos, yPos, 0.0f);
    }

    
private:
    int id;
    float xPos = 0.0f;
    float yPos = 0.0f;

    // in seconds
    float timeRemain = 1.0f;
    
    // next dest to visit
    unsigned int counter = 0;
    Pair nextDestination;
};
#endif
