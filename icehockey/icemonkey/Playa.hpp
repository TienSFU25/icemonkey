#ifndef PLAYA_H
#define PLAYA_H

#include <glm/glm.hpp>
#include <vector>

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
        timeRemain = nextDestination.T / Sim_Speed;
        
        if (nextDestination.S == "E") {
            xPos = nextDestination.X;
            yPos = nextDestination.Y;
        }
    }

    void move(float deltaTime) {
        float xRemain = nextDestination.X - xPos;
        float yRemain = nextDestination.Y - yPos;
        
        if (isGoingToTeleport) {
            timeRemain -= deltaTime;
            
            if (timeRemain < 0.01f) {
                isGoingToTeleport = false;
                xPos = destinations[counter - 1].X;
                yPos = destinations[counter - 1].Y;

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
                    isGoingToTeleport = true;
                    setNextDest();
                    return move(deltaTime);
                }

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
    
    PlayerState getState() {
        if (nextDestination.S == "E") {
            return Entering;
        } else if (nextDestination.S == "L") {
            return Leaving;
        } else {
            return Moving;
        }
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
