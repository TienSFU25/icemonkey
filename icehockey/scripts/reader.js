let giantFile = require('./giantFile');
const fs = require('fs');
const LIM = 10;
const Rink_Width_Max = 100;
const Rink_Height_Max = 42.5;
const Rink_Max_Dim = Math.max(Rink_Width_Max, Rink_Height_Max);

const normalize = (f) => {
    return f / Rink_Max_Dim;
};

const roundy = (f, t = 2) => {
    let expo = Math.pow(10, t);
    return Math.round( parseFloat(f) * expo) / expo;
};

let arr = [];
for (let i = 0; i < LIM; i++) {
    // console.log(giantFile.events[i].playersOnIce);
    arr.push(giantFile.events[i]);
}
let thing = {events: arr};
let stringified = JSON.stringify(thing, null, 4);

const union = (a, b) => {
    return new Set([...a, ...b]);
};

const intersection = (a, b) => {
    return new Set(
        [...a].filter(x => b.has(x)));
};

const difference = (a, b) => {
    return new Set(
        [...a].filter(x => !b.has(x)));
};

let giantString = '';

// let firstTeam = new Set(giantFile.events[0].playersOnIce['18']);
// let secondTeam = new Set(giantFile.events[0].playersOnIce['19']);
let firstTeam = new Set();
let secondTeam = new Set();
let playersInLastFrame = union(firstTeam, secondTeam);

let playersInFirstTeam = new Set();
let playersInSecondTeam = new Set();

let playerTimeLine = {};

const addOrAppend = (obj, key, val) => {
    if (!obj[key]) {
        obj[key] = [];

        // add initial "offscreen" coord
        // obj[key].push({
        //     X: -2.0,
        //     Y: -2.0,
        //     T: 0.0
        // });
    }

    obj[key].push(val);
};

for (let i = 0; i < giantFile.events.length; i++) {
    const theEvent = giantFile.events[i];
    const gameTime = roundy(theEvent.gameTime);
    const playerTeam = theEvent.playersOnIce['18'].indexOf(theEvent.playerId) > 0 ? 0 : 1;
    const X = roundy(normalize(parseFloat(theEvent.xCoord)), 10);
    const Y = roundy(normalize(parseFloat(theEvent.yCoord)), 10);
    
    let playerThisFrame = theEvent.playerId;
    let firstTeamThisFrame = new Set(theEvent.playersOnIce['18']);
    let secondTeamThisFrame = new Set(theEvent.playersOnIce['19']);
    let playersInThisFrame = union(firstTeamThisFrame, secondTeamThisFrame);

    // sometimes this is messed up and a guy appears out of nowhere
    playersInThisFrame.add(playerThisFrame);

    let leave = difference(playersInLastFrame, playersInThisFrame);
    let enter = difference(playersInThisFrame, playersInLastFrame);

    playersInFirstTeam = union(playersInFirstTeam, firstTeamThisFrame);
    playersInSecondTeam = union(playersInSecondTeam, secondTeamThisFrame);

    playersInLastFrame = playersInThisFrame;

    // giantString += `${playerTeam} - ${theEvent.playerId}: ${theEvent.xCoord}, ${theEvent.yCoord}\n`;
    // cvsish
    giantString += `${gameTime}|${theEvent.playerId}|${X}|${Y}|${[...leave]}|${[...enter]}\n`;

    // add this guy
    if (!playerTimeLine[playerThisFrame]) {
        playerTimeLine[playerThisFrame] = [];
    }

    // enter before move, so the second pass is correct
    // guys who entered
    for (let pid of enter.entries()) {
        addOrAppend(playerTimeLine, pid[0], {
            X,
            Y,
            T: gameTime,
            S: 'E'
        });
    }

    // only this guy moved
    addOrAppend(playerTimeLine, playerThisFrame, {
        X,
        Y,
        T: gameTime,
        S: 'M'
    });

    // add guys who left
    for (let pid of leave.entries()) {
        addOrAppend(playerTimeLine, pid[0], {
            X,
            Y,
            T: gameTime,
            S: 'L'
        });
    }
}

let pids = Object.keys(playerTimeLine);

for (let i = 0; i < pids.length; i++) {
    let aTimeLine = playerTimeLine[pids[i]];

    for (let j = 0; j < aTimeLine.length; j++) {
        let thingThatHappened = aTimeLine[j];

        if (thingThatHappened.S == 'L') {
            thingThatHappened.X = -2.0;
            thingThatHappened.Y = -2.0;
        }

        if (thingThatHappened.S == 'E') {
            if (j < aTimeLine.length - 1) {
                let nextThingThatHappened = aTimeLine[j + 1];

                if (nextThingThatHappened.S == 'M') {
                    let X2 = nextThingThatHappened.X;
                    let Y2 = nextThingThatHappened.Y;
    
                    if (Y2 > 0) {
                        thingThatHappened.Y = Y2 + 2;
                    } else {
                        thingThatHappened.Y = Y2 - 2;
                    }
    
                    thingThatHappened.X = X2;    
                } else {
                    // this guy just entered and left the screen(?)
                    // just going to put him off screen for now
                    thingThatHappened.X = -2;
                    thingThatHappened.Y = -2;
                    console.log(`player ${pids[i]} at time ${thingThatHappened.T} entered and just left`);
                }
            } else {
                console.log(`player ${pids[i]} at time ${thingThatHappened.T} entered clgt?`);
            }
        }

        thingThatHappened.X += '';
        thingThatHappened.Y += '';
        thingThatHappened.T += '';

        // thingThatHappened.S = undefined;
    };
};

let playerst = '';
playerst = [...playersInSecondTeam] + '\n' + giantString;
playerst = [...playersInFirstTeam] + '\n' + giantString;

fs.writeFile("movements.dat", playerst, function(err) {
    if(err) {
        return console.log(err);
    }

    // console.log("Movements was saved!");
}); 

fs.writeFile('timeline.json', JSON.stringify(playerTimeLine, null, 4), (err) => {
    if (err) {
        console.log(err);
    }
})