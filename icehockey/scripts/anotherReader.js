let giantFile = require('./giantFile');
const fs = require('fs');
const LIM = 10;
const EnterMargin = 5;
const LeaveMargin = 5;
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

let puckId = 302;
let playerTimeLine = {};

const addOrAppend = (obj, key, val) => {
    if (!obj[key]) {
        obj[key] = [];
    }

    obj[key].push(val);
};

addOrAppend(playerTimeLine, puckId, {
    X: 0,
    Y: 0,
    T: 0,
    S: 'E'
});

for (let i = 0; i < giantFile.events.length; i++) {
    const theEvent = giantFile.events[i];
    const gameTime = roundy(theEvent.gameTime);
    const playerTeam = theEvent.playersOnIce['18'].indexOf(theEvent.playerId) > 0 ? 0 : 1;
    const X = roundy(normalize(parseFloat(theEvent.xCoord)), 10);
    const Y = roundy(normalize(parseFloat(theEvent.yCoord)), 10);

    // enter before move, so the second pass is correct
    // guys who entered
    addOrAppend(playerTimeLine, puckId, {
        X,
        Y,
        T: gameTime,
        S: 'M'
    });
}

let pids = Object.keys(playerTimeLine);

// convert shit to strings
for (let i = 0; i < pids.length; i++) {
    let aTimeLine = playerTimeLine[pids[i]];

    for (let j = 0; j < aTimeLine.length; j++) {
        let thingThatHappened = aTimeLine[j];
        thingThatHappened.X += '';
        thingThatHappened.Y += '';
        thingThatHappened.T += '';
    }
}

fs.writeFile('puck.json', JSON.stringify(playerTimeLine, null, 4), (err) => {
    if (err) {
        console.log(err);
    } else {
        console.log('puck was saved!');
    }
})