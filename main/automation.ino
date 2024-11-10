
void moveMaze() {
    // TO DO: adjust depending on the maze
    moveDistance(MAZE_DISTANCE, f);
    moveDistance(TURNING_DISTANCE, r);
    moveDistance(MAZE_DISTANCE, f);
    moveDistance(TURNING_DISTANCE, l);
    moveDistance(MAZE_DISTANCE, f);

    // turn 180 degrees
    moveDistance(TURNING_DISTANCE, r);
    moveDistance(TURNING_DISTANCE, r);

    // TO DO: go back through the maze
    moveDistance(MAZE_DISTANCE, f);
    moveDistance(TURNING_DISTANCE, r);
    moveDistance(MAZE_DISTANCE, f);
    moveDistance(TURNING_DISTANCE, l);
    moveDistance(MAZE_DISTANCE, f);
};
