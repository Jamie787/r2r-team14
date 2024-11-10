
void moveThroughMaze() {
    // TO DO: adjust depending on the maze
    // eg.
    moveDistance(MAZE_DISTANCE, f);
    moveDistance(TURNING_DISTANCE, r);
    moveDistance(MAZE_DISTANCE, f);
    moveDistance(TURNING_DISTANCE, l);
    moveDistance(MAZE_DISTANCE, f);

    // turn 180 degrees
    moveDistance(TURNING_DISTANCE, r);
    moveDistance(TURNING_DISTANCE, r);

    // TO DO: go back through the maze
    // eg.
    moveDistance(MAZE_DISTANCE, f);
    moveDistance(TURNING_DISTANCE, r);
    moveDistance(MAZE_DISTANCE, f);
    moveDistance(TURNING_DISTANCE, l);
    moveDistance(MAZE_DISTANCE, f);
};

void senseSurroundings() {
    for (int i = 0; i < 4; i++) {
        // check surroundings
        checkUltrasonic();
    
        // turn
        moveDistance(TURNING_DISTANCE, r);
    }
};

void traverseStairs() {
    // go up the stairs
    moveDistance(STAIR_DISTANCE, f);

    // turn 180 degrees
    moveDistance(TURNING_DISTANCE, r);
    moveDistance(TURNING_DISTANCE, r);

    // go down the stairs
    moveDistance(STAIR_DISTANCE, f);

};
