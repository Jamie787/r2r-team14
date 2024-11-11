
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
    switch (i) {
      case 0:
        Serial.print("Forward ");
        break;
      case 1:
        Serial.print("Right ");
        break;
      case 2:
        Serial.print("Behind ");
        break;
      case 3:
        Serial.print("Left ");
        break;
    }

    Serial.println(findSensorDistance(forwardTrig, forwardEcho));
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
