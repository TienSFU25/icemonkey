#ifndef PLAYA_H
#define PLAYA_H

#include <glm/glm.hpp>
#include <vector>
#include "utils.h"

const Pair defaultInitialPosition = {.X = -2.0f, .Y = 2.0f, .T = 0.0f};

class Playa
{
public:
    bool isHomeTeam;
    std::vector<Pair> destinations;
    bool isGoingToTeleport = false;

    Playa(std::vector<Pair> d, int pid = -1, bool _isHomeTeam = false)
    {
        destinations = d;
        id = pid;
        isHomeTeam = _isHomeTeam;

        counter = 1;
        nextDestination = destinations[0];

        xPos = defaultInitialPosition.X;
        yPos = defaultInitialPosition.Y;
//        xPos = 0 + destinations[0].X;
//        yPos = 0 + destinations[0].Y;
        timeRemain = nextDestination.T / Sim_Speed;
        
        if (nextDestination.S == "E") {
            xPos = nextDestination.X;
            yPos = nextDestination.Y;
        }
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
        
        if (isGoingToTeleport) {
            timeRemain -= deltaTime;
            
            if (timeRemain < 0.01f) {
                isGoingToTeleport = false;
                xPos = destinations[counter - 1].X;
                yPos = destinations[counter - 1].Y;

//                nextDestination = destinations[counter];
//                setNextDest();
                return move(deltaTime);
            }
            
            return;
        }
        
        // move X, Y
        if (timeRemain > 0.01f) {
            float stepX = (deltaTime / timeRemain) * xRemain;
            float stepY = (deltaTime / timeRemain) * yRemain;
            
            xPos += stepX;
            yPos += stepY;
            timeRemain -= deltaTime;
        } else {
            if (counter < destinations.size()) {
                nextDestination = destinations[counter];
                isGoingToTeleport = false;
                
                // "teleport" and just stand there
                if (nextDestination.S == "E") {
//                    xPos = nextDestination.X;
//                    yPos = nextDestination.Y;
                    isGoingToTeleport = true;
                    setNextDest();
//                    counter -= 1;
                    return move(deltaTime);
                } else if (nextDestination.S == "L") {
//                    isGoingToTeleport = true;
//                    setNextDest();
//                    counter -= 1;
//                    return move(deltaTime);
                }

//                std::cout << "moving " << id << " to (" << nextDestination.X << ", " << nextDestination.Y << ")" << std::endl;
                setNextDest();
                move(deltaTime);
            }
        }
    }
    
    void setNextDest() {
        float timeExceeded = timeRemain * -1;
        Pair previousDestination;
        
        if (counter > 0) {
            previousDestination = destinations[counter - 1];
        } else {
            previousDestination = defaultInitialPosition;
        }
        
        timeRemain = (nextDestination.T - previousDestination.T) / Sim_Speed;
        timeRemain -= timeExceeded;
        counter += 1;
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
