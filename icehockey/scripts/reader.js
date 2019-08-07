let giantFile = require('./giantFile');
const fs = require('fs');
const LIM = 10;

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

let firstTeam = new Set(giantFile.events[0].playersOnIce['18']);
let secondTeam = new Set(giantFile.events[0].playersOnIce['19']);
let playersInLastFrame = union(firstTeam, secondTeam);

let playersInFirstTeam = new Set();
let playersInSecondTeam = new Set();

// true means same
let arrCompare = (a, b) => {
    let o1 = {};
    let adiff = [];
    let same = [];
    let bdiff = [];

    for (let i = 0; i < a.length; i++) {
        o1[a[i]] = {
            bSeen: false
        };
    }

    for (let i = 0; i < b.length; i++) {
        if (o1[b[i]]) {
            same.push(b[i]);
            o1[b[i]].bSeen = true;
        } else {
            bdiff.push(b[i]);
        }
    }

    for (let i = 0; i < a.length; i++) {
        if (!o1[a[i]].bSeen) {
            adiff.push(a[i]);
        }
    }

    return {same, adiff, bdiff};
};

for (let i = 0; i < giantFile.events.length; i++) {
    const theEvent = giantFile.events[i];
    const gameTime = Math.round( parseFloat(theEvent.gameTime) * 100) / 100;
    const playerTeam = theEvent.playersOnIce['18'].indexOf(theEvent.playerId) > 0 ? 0 : 1;

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
    giantString += `${gameTime}|${theEvent.playerId}|${theEvent.xCoord}|${theEvent.yCoord}|${[...leave]}|${[...enter]}\n`;
}

giantString = [...playersInSecondTeam] + '\n' + giantString;
giantString = [...playersInFirstTeam] + '\n' + giantString;

// fs.writeFile("smaller.js", stringified, function(err) {
//     if(err) {
//         return console.log(err);
//     }

//     console.log("The file was saved!");
// }); 

fs.writeFile("movements.dat", giantString, function(err) {
    if(err) {
        return console.log(err);
    }

    console.log("Movements was saved!");
}); 
