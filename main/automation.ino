
void moveThroughMaze() {
    // TODO: adjust depending on the maze
    moveRobotDistance(MAZE_DISTANCE, f);
    turnDirection(r);
    moveRobotDistance(MAZE_DISTANCE, f);
    turnDirection(l);
    moveRobotDistance(MAZE_DISTANCE, f);

    // turn 180 degrees
    turnDirection(r); //moveRobotDistance(TURNING_DISTANCE, r);
    turnDirection(r);

    // TODO: go back through the maze
    moveRobotDistance(MAZE_DISTANCE, f);
    turnDirection(r);
    moveRobotDistance(MAZE_DISTANCE, f);
    turnDirection(l);
    moveRobotDistance(MAZE_DISTANCE, f);
};

void senseSurroundings() {
    for (int i = 0; i < 4; i++) {
        // check surroundings and turn
        checkUltrasonic();
        turnDirection(r);
    }
};

void traverseStairs() {
    // go up the stairs
    moveRobotDistance(STAIR_DISTANCE, f);

    // turn 180 degrees
    turnDirection(r);
    turnDirection(r);

    // go down the stairs
    moveRobotDistance(STAIR_DISTANCE, f);

};
