
void moveThroughMaze() {
    // TODO: adjust depending on the maze
    moveDistance(MAZE_DISTANCE);
    turnDirection('r');
    moveDistance(MAZE_DISTANCE);
    turnDirection('l');
    moveDistance(MAZE_DISTANCE);

    // turn 180 degrees
    turnDirection('r'); //moveRobotDistance(TURNING_DISTANCE, r);
    turnDirection('r');

    // TODO: go back through the maze
    moveDistance(MAZE_DISTANCE);
    turnDirection('r');
    moveDistance(MAZE_DISTANCE);
    turnDirection('l');
    moveDistance(MAZE_DISTANCE);
};

void senseSurroundings() {
    for (int i = 0; i < 4; i++) {
        // check surroundings and turn
        checkUltrasonic();
        turnDirection('r');
    }
};

void traverseStairs() {
    // go up the stairs
    moveDistance(STAIR_DISTANCE);

    // turn 180 degrees
    turnDirection('r');
    turnDirection('r');

    // go down the stairs
    moveDistance(STAIR_DISTANCE);

};
