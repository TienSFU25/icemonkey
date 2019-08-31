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
    
    void readTheDatFile() {
        string wholeThing;
        string line;
        ifstream myfile ("/Users/user/Documents/361/opengl/icehockey/icehockey/scripts/puck.json");
        ifstream myfile2 ("/Users/user/Documents/361/opengl/icehockey/icehockey/scripts/players.dat");
        
        set<int> playersInFirstTeam;
        
        if (myfile2.is_open()) {
            getline(myfile2, line);
            vector<string> splitted = Utils::split(line, ',');
            
            for (string const& s: splitted) {
                int playerId = Utils::stringToInt(s);
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
                int playerId = Utils::stringToInt(it -> first);
                vector<Pair> destinations;
                
                vector<map<string, string>> playerTimeline = it -> second;
                
                for (map<string, string>& position: playerTimeline) {
                    Pair p = {.X = Utils::stringToFloat(position["X"]),
                              .Y = Utils::stringToFloat(position["Y"]),
                              .T = Utils::stringToFloat(position["T"]),
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
        
        else cout << "Unable to open file";
    }
    
private:
};

#endif /* Reada_h */
