//
//  Reada.h
//  hiopengl
//
//  Created by tienv on 2019-08-01.
//  Copyright © 2019 tienv. All rights reserved.
//

#ifndef Reada_h
#define Reada_h

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "Playa.hpp"

using namespace std;

// this file is only to "deserialize" the movements.dat file
struct TimeStep {
    int playerId;
    float X;
    float Y;
    vector<int> playersLeave;
    
    // player ID - estimated appearance
    map<int, Pair> playersEnter;
};

class Reada {
public:
    vector<TimeStep> timeSteps;
    map<int, Playa> players;
    float overTime = 0.3f;
    float timeLeft = 0.0f;
    int counter = -1;
    
    void move(float deltaTime) {
//        if (timeLeft < 0.01f) {
//            // reset some counters and indexes
//            timeLeft = overTime;
//            counter += 1;
//            playersMovingThisStep.clear();
//
//            TimeStep thisStep = timeSteps[counter];
//            int currentPlayerId = thisStep.playerId;
//            Pair nextDestination = {.X = thisStep.X, .Y = thisStep.Y};
//
//            cout << "moving guy " << currentPlayerId << " to " << nextDestination.X << ", " << nextDestination.Y << endl;
//            players[currentPlayerId].setMoveTo(nextDestination, overTime);
//            playersMovingThisStep.push_back(currentPlayerId);
//
//            for (int const& playerId: thisStep.playersLeave) {
//                players[playerId].setMoveOffscreen(overTime);
//                playersMovingThisStep.push_back(playerId);
//            }
//
//            map<int, Pair> playersEnterMap = thisStep.playersEnter;
//            for (map<int,Pair>::iterator it=playersEnterMap.begin(); it!=playersEnterMap.end(); ++it) {
//                int pid = it -> first;
//                Pair startingCoord = it -> second;
//
//                players[pid].setMoveTo(startingCoord, overTime);
//                playersMovingThisStep.push_back(pid);
//            }
//        } else {
//            timeLeft -= deltaTime;
//        }
//
//        for (int const& playerId: playersMovingThisStep) {
//            players[playerId].move(deltaTime);
//        }
        vector<int> playersLeavingThisStep;
        vector<int> allPlayers;
        for (map<int,Playa>::iterator it=players.begin(); it!=players.end(); ++it) {
            int pid = it -> first;
            Playa& player = it -> second;
            player.move(deltaTime);
        }
        
        vector<int> playersMovingThisStep = allPlayers;
//        if (!playersLeavingThisStep.empty()) {
//            playersMovingThisStep = playersLeavingThisStep;
//
//            string s = "moving this step: ";
//            for (int pid: playersMovingThisStep) {
//                s += std::to_string(pid) + ", ";
//            }
//
//            cout << s << endl;
//        }
        
//        for (int pid : playersMovingThisStep) {
//            Playa& player = players.find(pid) -> second;
//            player.move(deltaTime);
//        }
    }
    
    void readTheDatFile() {
//        Pair a = {.X = -0.5f, .Y = -0.5f, .T = 0.0f};
//        Pair b = {.X = 0.5f, .Y = 0.0f, .T = 200.0f};
//
//        std::vector<Pair> ds;
//        ds.push_back(a);
//        ds.push_back(b);
//
//        Playa newPlayer(ds, false);
//        int pid = 0;
//        players.insert(pair<int, Playa>(pid, newPlayer));

//        players[pid] = newPlayer;
        
        string wholeThing;
        string line;
        ifstream myfile ("/Users/user/Documents/361/opengl/icehockey/icehockey/scripts/_timeline.json");
        ifstream myfile2 ("/Users/user/Documents/361/opengl/icehockey/icehockey/scripts/players.dat");
        
        set<int> playersInFirstTeam;
        
        if (myfile2.is_open()) {
            getline(myfile2, line);
            vector<string> splitted = split(line, ',');
            
            for (string const& s: splitted) {
                int playerId = stringToInt(s);
                playersInFirstTeam.insert(playerId);
            }
            
            myfile2.close();
        }

        
        if (myfile.is_open())
        {
            int i = 0;

            while ( getline (myfile,line) )
            {
                wholeThing += line;
            }
            
            map<string, vector<map<string, string>>> parsed = json::parse(wholeThing);;
            for (map<string, vector<map<string, string>>>::iterator it=parsed.begin(); it!=parsed.end(); ++it) {
                int playerId = stringToInt(it -> first);
                vector<Pair> destinations;
                
                vector<map<string, string>> playerTimeline = it -> second;
                
                for (map<string, string>& position: playerTimeline) {
                    Pair p = {.X = stringToFloat(position["X"]),
                              .Y = stringToFloat(position["Y"]),
                              .T = stringToFloat(position["T"]),
                              .S = position["S"]
                    };
                    destinations.push_back(p);
                }
                
                bool isThisGuyHomeTeam = playersInFirstTeam.find(playerId) != playersInFirstTeam.end();
                Playa newPlayer(destinations, playerId, isThisGuyHomeTeam);
                players.insert(pair<int, Playa>(playerId, newPlayer));
            }
                        
            myfile.close();
        }
        

//                } else {
//                    // 0|1152|0.312866|0.752941||||
//                    // isHome | playerId | x | y | homeLeave | homeEnter | awayLeave | awayEnter
//                    vector<string> splitted = split(line, '|');
//                    int playerId = stringToInt(splitted[1]);
//
//                    string leave = splitted[4];
//                    string enter = splitted[5];
//
//                    vector<string> playersLeftStr = split(leave, ',');
//                    vector<int> playersLeft;
//
//                    for (string const& pid: playersLeftStr)  {
//                        playersLeft.push_back(stringToInt(pid));
//                    }
//
//                    map<int, Pair> playersEnter;
//
//                    if (!enter.empty()) {
//                        vector<string> playersEnterStr = split(enter, ',');
//                        for (string const& pid: playersEnterStr)  {
//                            playersEnter[stringToInt(pid)] = defaultInitialPosition;
//                        }
//
//                        timeStepsToUpdate.push_back(i - 2);
//                    }
//
//                    // (float)(int) to avoid floating point 1E+29 shenanigans
//                    TimeStep t = {.playerId = playerId,
//                        .X = normalize((float)(int)stringToFloat(splitted[2])),
//                        .Y = normalize((float)(int)stringToFloat(splitted[3])),
//                        .playersLeave = playersLeft,
//                        .playersEnter = playersEnter
//                    };
//
//                    timeSteps.push_back(t);
//                }
//                i++;
//            }
//            myfile.close();
//
//            for (int const& c: timeStepsToUpdate) {
//                TimeStep& ts = timeSteps[c];
//                map<int, Pair> playersEnterMap = ts.playersEnter;
//
//                for (map<int,Pair>::iterator it=playersEnterMap.begin(); it!=playersEnterMap.end(); ++it) {
//                    int playerId = it -> first;
//                    int currentIndex = c;
//
//                    while (currentIndex < i - 2) {
//                        currentIndex += 1;
//                        if (timeSteps[currentIndex].playerId == playerId) {
//                            ts.playersEnter[playerId] = Pair {.X = timeSteps[currentIndex].X, .Y = timeSteps[currentIndex].Y };
//
//                            timeStepsToRemove.push_back(currentIndex);
//                            break;
//                        }
//                    }
//                }
//            }
//
//            sort(timeStepsToRemove.begin(), timeStepsToRemove.end());
//            for (int k = timeStepsToRemove.size() - 1; k >= 0; k--) {
//                timeSteps.erase(timeSteps.begin() + timeStepsToRemove[k]);
//            }
//        }
//        
        else cout << "Unable to open file";
    }
    
private:
};

#endif /* Reada_h */
